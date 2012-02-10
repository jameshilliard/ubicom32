/* Kernel module to match a string into a packet.
 *
 * Copyright (C) 2000 Emmanuel Roger  <winfield@freegates.be>
 *
 * ChangeLog
 *	19.02.2002: Gianni Tedesco <gianni@ecsc.co.uk>
 *		Fixed SMP re-entrancy problem using per-cpu data areas
 *		for the skip/shift tables.
 *	02.05.2001: Gianni Tedesco <gianni@ecsc.co.uk>
 *		Fixed kernel panic, due to overrunning boyer moore string
 *		tables. Also slightly tweaked heuristic for deciding what
 * 		search algo to use.
 * 	27.01.2001: Gianni Tedesco <gianni@ecsc.co.uk>
 * 		Implemented Boyer Moore Sublinear search algorithm
 * 		alongside the existing linear search based on memcmp().
 * 		Also a quick check to decide which method to use on a per
 * 		packet basis.
 */

//#include <linux/smp.h>
#include <linux/module.h>
#include <linux/skbuff.h>
//#include <linux/file.h>
//#include <net/sock.h>

#include <net/ip.h>
#include <linux/tcp.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_urlfilter.h>

#define HTTP_GET	"GET "
#define HTTP_POST	"POST "
#define HTTP_HEAD	"HEAD "
#define HOST_TOKEN	"Host: "


MODULE_LICENSE("GPL");

/* Linear string search based on memcmp() */
char *search_linear (char *needle, char *haystack, int needle_len, int haystack_len)
{
	char *k = haystack + (haystack_len-needle_len);
	char *t = haystack;

	while ( t <= k ) {
		if (memcmp(t, needle, needle_len) == 0)
			return t;
		t++;
	}
	return NULL;
}

static unsigned char urlHostEndPatten[]={0x0d, 0x0a};

/* Chun comment out*/
#if 0
static int
match(const struct sk_buff *skb,
      const struct net_device *in,
      const struct net_device *out,
      const void *matchinfo,
      int offset,
      unsigned int protoff,
      int *hotdrop)
{
	const struct ipt_urlfilter_info *info = matchinfo;
	proc_ipt_search search=search_linear;
	unsigned char *pattern;
	unsigned char logUrl[50];
	char *urlPtr;

	struct iphdr *iph = (skb)->nh.iph;
	struct tcphdr *tcph = (void *)iph + iph->ihl*4;
	unsigned char *data = (void *)tcph + tcph->doff*4;
	unsigned int data_len = (skb)->len - (iph->ihl*4) - (tcph->doff*4);
	int flag;
	memset(logUrl, 0 ,50);

	if(data_len < 50 )
		return 0;

	if(memcmp(data, HTTP_GET, sizeof(HTTP_GET)-1) ==0)
		flag = sizeof(HTTP_GET)-1;
	else if(memcmp(data, HTTP_HEAD, sizeof(HTTP_HEAD)-1) ==0)
		flag = sizeof(HTTP_HEAD)-1;
	else if(memcmp(data, HTTP_POST, sizeof(HTTP_POST)-1) ==0 )
		flag = sizeof(HTTP_POST)-1;
	else
		return 0;

	/* Deny all http connections if host is all */
	if( memcmp(info->host, "ALL", sizeof("ALL") ) ==0 )
		return 1^info->invert;

	/* Search "Host: " */
	if( !(pattern = search(HOST_TOKEN, data, sizeof(HOST_TOKEN)-1, data_len)) )
		return 0;

	urlPtr = strstr(pattern + sizeof(HOST_TOKEN)-1, &urlHostEndPatten[0]);
	strncpy(logUrl, (pattern + sizeof(HOST_TOKEN)-1), ((unsigned char*)urlPtr - (pattern + sizeof(HOST_TOKEN)-1)));
	logUrl[((unsigned char*)urlPtr - (pattern + sizeof(HOST_TOKEN)-1))] = '/0';
	   printk(KERN_INFO "url addr = %s \n",logUrl);
	/* compare host */
/* Chun modify: for url filter can filter keyword*/
	//if(memcmp( (pattern + sizeof(HOST_TOKEN)-1), info->host, info->host_len) !=0)
	if(strstr( (pattern + sizeof(HOST_TOKEN)-1), info->host) ==0)
		return 0^info->invert;

	/* if there is no url, it match */
	if( info->url_len ==0)
		return 1^info->invert;

	/* compare url */
	if( memcmp( (data+flag), info->url, info->url_len ) ==0)
		return 1^info->invert;


	return 0;
}
#endif

/* Chun add for add log message of web site and src ip for access control function */
/* transfer number to network format, ie, -1062731676=192.168.0.100*/
u8 ip_string[16]={0};
char *ntoa(u32 ip_hex)
{
	//NIPQUAD ip_hex ??
	sprintf(ip_string,"%d.%d.%d.%d",(ip_hex & (0xFF000000))>>24,(ip_hex &(0x00FF0000))>>16,(ip_hex &(0x0000FF00))>>8,(ip_hex &(0x000000FF)));
	return ip_string;
}

/* We only log message with length <128
 * return 0: fail since length >128
 * return 1: success
 */
int do_log(unsigned char *pattern_host,unsigned char *pattern_url,int accept_url, u32 src_ip)
{
	char *url_end_ptr = NULL, *action_end_ptr = NULL;
	int url_len = 0, action_len = 0;
	unsigned char logUrl[128]={0};

	/* Get pattern_host info from packet and save it in logUrl */
	url_end_ptr = strstr(pattern_host, &urlHostEndPatten[0]);
	if(url_end_ptr)
	{
		url_len = (unsigned char*)url_end_ptr - pattern_host ;
		if(url_len < 128)
			strncpy(logUrl, pattern_host , url_len);
		else
			return 0;
	}
	else
		return 0;

	/* Get pattern_url info from packet and append it in logUrl */
	action_end_ptr = strchr(pattern_url , ' ');
	if(action_end_ptr)
	{
		action_len = (unsigned char*)action_end_ptr - pattern_url ;
		if( url_len + action_len < 128 )
		{
			strncpy(logUrl + url_len, pattern_url , action_len);
			logUrl[url_len + action_len] = '\0';
		}
		else
			return 0;
	}
	else
		return 0;

	if(accept_url)
		printk(KERN_NOTICE "%s accessed from %s\n",logUrl,ntoa(src_ip));
	else
		printk(KERN_NOTICE "%s blocked for %s\n",logUrl,ntoa(src_ip));

	return 1;
}
/*
 * content of packet		ptr to packet	   iptables url
 *---------------------------------------------------------------
 * GET/HEAD/POST: XXXXXX	=pattern_url	<=>tmpinfo.url
 * HOST: XXX.XXX.XXX.XXX	=pattern_host	<=>tmpinfo.host
 *
 * tmpinfo.host records if we need to log the packet or not
 * a)XXX	: not log it
 * b)XXX%1	: log it and the action is ACCEPT
 * c)XXX%0	: log it and the action is DROP
 *
 */

static int urlfilter_mt(const struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct ipt_urlfilter_info *info = par->matchinfo;
	struct ipt_urlfilter_info tmpinfo = *info;
	proc_ipt_search search = search_linear;
	unsigned char *pattern_host, *pattern_url;
	int info_host_len, need_log=0, accept_url=0;

	const struct iphdr *iph = ip_hdr(skb);
	struct tcphdr _tcph, *tcph;

	unsigned char *data;
	unsigned int data_len;
	int flag;

	tcph = skb_header_pointer(skb, ip_hdrlen(skb), sizeof(_tcph), &_tcph);
	if(!tcph){
		return false;
	}
	data = (void *)tcph + tcph->doff*4;
	data_len = (skb)->len - (iph->ihl*4) - (tcph->doff*4);

	if (data_len < 50 ) {
		return false;
	}

	if(memcmp(data, HTTP_GET, sizeof(HTTP_GET)-1) ==0)
	{
		flag = sizeof(HTTP_GET)-1;

		/* Search "GET " and put pattern_url pointing to the last character of it */
		pattern_url = search(HTTP_GET, data, sizeof(HTTP_GET)-1, data_len);
		pattern_url += flag;
	}
	else if(memcmp(data, HTTP_HEAD, sizeof(HTTP_HEAD)-1) ==0)
	{
		flag = sizeof(HTTP_HEAD)-1;

		/* Search "HEAD " and put pattern_url pointing to the last character of it */
		pattern_url = search(HTTP_HEAD, data, sizeof(HTTP_HEAD)-1, data_len);
		pattern_url += flag;
	}
	else if(memcmp(data, HTTP_POST, sizeof(HTTP_POST)-1) ==0 )
	{
		flag = sizeof(HTTP_POST)-1;

		/* Search "POST " and put pattern_url pointing to the last character of it */
		pattern_url = search(HTTP_POST, data, sizeof(HTTP_POST)-1, data_len);
		pattern_url += flag;
	}
	else
		return false;

	/* Search "Host: " and put pattern_host pointing to the last character of it */
	if( !(pattern_host = search(HOST_TOKEN, data, sizeof(HOST_TOKEN)-1, data_len)) ) {
		return false;
	}

	pattern_host += sizeof(HOST_TOKEN)-1;

	/* If tmpinfo.host="XXX%1", remove the last two characters and turn tmpinfo.host="XXX".
	 * Otherwise, tmpinfo.host="XXX" means we don't need to log it.
	 */
	info_host_len = strlen(tmpinfo.host);
	if(*(tmpinfo.host + info_host_len - 2) == '%')
	{
		need_log=1;

		if(*(tmpinfo.host + info_host_len - 1) == '1')
			accept_url=1;
		else
			accept_url=0;

		*(tmpinfo.host + info_host_len - 2)='\0';
		*(tmpinfo.host + info_host_len - 1)='\0';
	}

	/* Match only when
	 * a) tmpinfo.host == ALL
	 * b) tmpinfo.host == pattern_host and there is no tmpinfo.url
	 * c) tmpinfo.host == pattern_host and tmpinfo.url == pattern_url
	 */

	/* If tmpinfo.host==ALL, then they match anyway! */
	if(strstr(tmpinfo.host, "ALL"))
	{
		if(need_log)
			do_log(pattern_host,pattern_url,accept_url,iph->saddr);

		return 1^info->invert;
	}

	/* if tmpinfo.host==host */
	if(strstr( pattern_host , tmpinfo.host))
	{
		/* if there is no url or tmpinfo.url == pattern_url, then they match! */
//		if( ( tmpinfo.url_len == 0)  ||
//			( memcmp( pattern_url, tmpinfo.url, tmpinfo.url_len ) == 0))
//		{
			if(need_log)
				do_log(pattern_host,pattern_url,accept_url,iph->saddr);

			return 1^tmpinfo.invert;
//		}
	}

	/* Otherwise, they don't match */
	return 0^tmpinfo.invert;
}
/* Chun end */

static int urlfilter_checkentry(const struct xt_mtchk_param *par)
{
//	if (matchsize != IPT_ALIGN(sizeof(struct ipt_urlfilter_info)))
//		return 0;

	return 0;
}

static struct xt_match urlfilter_match __read_mostly = {
	.name		= "urlfilter",
	.family		= NFPROTO_IPV4,
	.match		= urlfilter_mt,
	.matchsize	= sizeof(struct ipt_urlfilter_info),
	.checkentry	= urlfilter_checkentry,
	.me			= THIS_MODULE,
};

static int __init init(void)
{
	return xt_register_match(&urlfilter_match);
}

static void __exit fini(void)
{
	xt_unregister_match(&urlfilter_match);
}

module_init(init);
module_exit(fini);
