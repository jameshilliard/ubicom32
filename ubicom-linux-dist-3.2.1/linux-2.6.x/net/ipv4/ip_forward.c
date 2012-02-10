/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		The IP forwarding functionality.
 *
 * Authors:	see ip.c
 *
 * Fixes:
 *		Many		:	Split from ip.c , see ip_input.c for
 *					history.
 *		Dave Gregorich	:	NULL ip_rt_put fix for multicast
 *					routing.
 *		Jos Vos		:	Add call_out_firewall before sending,
 *					use output device for accounting.
 *		Jos Vos		:	Call forward firewall after routing
 *					(always use output device).
 *		Mike McLagan	:	Routing by source
 */

#include <linux/types.h>
#include <linux/mm.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/netdevice.h>
#include <linux/slab.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/netfilter_ipv4.h>
#include <net/checksum.h>
#include <linux/route.h>
#include <net/route.h>
#include <net/xfrm.h>

#ifdef CONFIG_IPV6_TUNNEL_DSLITE
/* Softwire (DS-Lite) */
static int is_device_name_softwire(struct sk_buff *skb);

/* for is_device_name_softwire function */
 static  int GlobalSoftWireMTU = 1300;
 int SoftWireToggle = 0; /* SoftWireToggle =1 enabled Phase 1- fragmentation step 3*/
 int overridesmtu =0; /* Use default as original values*/
 static char softwire_device_name[20]="softwire";
 
 int l2tpoverridesmtu =0;
 int l2tpsmtu =1300;
 
void SetSoftWireGloball2tpMTU(int SoftPMTU)
{
	l2tpsmtu = SoftPMTU;
}
EXPORT_SYMBOL(SetSoftWireGloball2tpMTU);

void Setl2tpOverridesmtu(int localoverridesmtu)
{
	l2tpoverridesmtu = localoverridesmtu;
}
EXPORT_SYMBOL(Setl2tpOverridesmtu);

 void SetSoftWireGlobalMTU(int SoftPMTU)
{
	/*
	  This function is called by the ip6_tunnel.c to set the pathmtu
	  no checks are done at this end bcos its assumed that the source setting this pathmtu is
	  doing this checks by itself before it writes to this function
	*/
	GlobalSoftWireMTU = SoftPMTU;
}
EXPORT_SYMBOL(SetSoftWireGlobalMTU);


void SetSoftwireDeviceName(char array[])
{
	strcpy(softwire_device_name, array);
}
EXPORT_SYMBOL(SetSoftwireDeviceName);

void SetSoftWireToggle(int toggle)
{
	/*
	  This function is called by the ip6_tunnel.c to set the SoftwireToggle
	*/
	SoftWireToggle = toggle;
}
EXPORT_SYMBOL(SetSoftWireToggle);

void SetOverridesmtu(int localoverridesmtu)
{
	/*
	  This function is called by the ip6_tunnel.c to set the SoftwireToggle
	*/
	overridesmtu = localoverridesmtu;
}
EXPORT_SYMBOL(SetOverridesmtu);
/* Softwire (DS-Lite) */
#endif

static int ip_forward_finish(struct sk_buff *skb)
{
	struct ip_options * opt	= &(IPCB(skb)->opt);

	IP_INC_STATS_BH(dev_net(skb_dst(skb)->dev), IPSTATS_MIB_OUTFORWDATAGRAMS);

	if (unlikely(opt->optlen))
		ip_forward_options(skb);

	return dst_output(skb);
}

int ip_forward(struct sk_buff *skb)
{
	struct iphdr *iph;	/* Our header */
	struct rtable *rt;	/* Route we use */
	struct ip_options * opt	= &(IPCB(skb)->opt);

#ifdef CONFIG_IPV6_TUNNEL_DSLITE
	/* Softwire (DS-Lite) */
	int local_mtu;
	struct udphdr *testudpheader;
	int flag =0;	
	testudpheader = udp_hdr(skb);
	/* Softwire (DS-Lite) */
#endif

	if (skb_warn_if_lro(skb))
		goto drop;

	if (!xfrm4_policy_check(NULL, XFRM_POLICY_FWD, skb))
		goto drop;

	if (IPCB(skb)->opt.router_alert && ip_call_ra_chain(skb))
		return NET_RX_SUCCESS;

	if (skb->pkt_type != PACKET_HOST)
		goto drop;

	skb_forward_csum(skb);

	/*
	 *	According to the RFC, we must first decrease the TTL field. If
	 *	that reaches zero, we must reply an ICMP control message telling
	 *	that the packet's lifetime expired.
	 */
	if (ip_hdr(skb)->ttl <= 1)
		goto too_many_hops;

	if (!xfrm4_route_forward(skb))
		goto drop;

	rt = skb_rtable(skb);

	if (opt->is_strictroute && rt->rt_dst != rt->rt_gateway)
		goto sr_failed;

#ifdef CONFIG_IPV6_TUNNEL_DSLITE
	/* Check if the device is softwire (DS-Lite) */
	if(is_device_name_softwire(skb) )
     	{				
		if (unlikely(overridesmtu ==1 ))   //IPIP only
			local_mtu = GlobalSoftWireMTU;
		else
			local_mtu = dst_mtu(&rt->dst); 
				
		if(unlikely((skb->len > local_mtu) && (ip_hdr(skb)->frag_off & htons(IP_DF)) && (!skb->local_df ) ))
		{
		  if (unlikely(SoftWireToggle ==1)) 
		  {
			    //IP_INC_STATS(IPSTATS_MIB_FRAGFAILS);
			   IP_INC_STATS(dev_net(rt->dst.dev), IPSTATS_MIB_FRAGFAILS); // for 2.6.30.10 compatibitlity
			   icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
			   htonl(local_mtu));
	    	  }
		  else 
		  {
			   //IP_INC_STATS(IPSTATS_MIB_FRAGFAILS);
			   IP_INC_STATS(dev_net(rt->dst.dev), IPSTATS_MIB_FRAGFAILS); // for 2.6.30.10 compatibitlity
			   icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
			   htonl(local_mtu));
		       goto drop;
			
		  } 
		}
    	}	/* Not softwire or DS-Lite*/
   	else
	{
		 /*  normal execution path not softwire */
		 if (unlikely(skb->len > dst_mtu(&rt->dst) && !skb_is_gso(skb) &&
			     (ip_hdr(skb)->frag_off & htons(IP_DF))) && !skb->local_df) 
		 {
			//IP_INC_STATS(IPSTATS_MIB_FRAGFAILS);
			IP_INC_STATS(dev_net(rt->dst.dev), IPSTATS_MIB_FRAGFAILS); // for 2.6.30.10 compatibitlity
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
				  htonl(dst_mtu(&rt->dst)));
			goto drop;
		 }   
	}

#else
	if (unlikely(skb->len > dst_mtu(&rt->dst) && !skb_is_gso(skb) &&
		     (ip_hdr(skb)->frag_off & htons(IP_DF))) && !skb->local_df) {
		IP_INC_STATS(dev_net(rt->dst.dev), IPSTATS_MIB_FRAGFAILS);
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
			  htonl(dst_mtu(&rt->dst)));
		goto drop;
	}
#endif

	/* We are about to mangle packet. Copy it! */
	if (skb_cow(skb, LL_RESERVED_SPACE(rt->dst.dev)+rt->dst.header_len))
		goto drop;
	iph = ip_hdr(skb);

	/* Decrease ttl after skb cow done */
	ip_decrease_ttl(iph);

	/*
	 *	We now generate an ICMP HOST REDIRECT giving the route
	 *	we calculated.
	 */
	if (rt->rt_flags&RTCF_DOREDIRECT && !opt->srr && !skb_sec_path(skb))
		ip_rt_send_redirect(skb);

	skb->priority = rt_tos2priority(iph->tos);

	return NF_HOOK(NFPROTO_IPV4, NF_INET_FORWARD, skb, skb->dev,
		       rt->dst.dev, ip_forward_finish);

sr_failed:
	/*
	 *	Strict routing permits no gatewaying
	 */
	 icmp_send(skb, ICMP_DEST_UNREACH, ICMP_SR_FAILED, 0);
	 goto drop;

too_many_hops:
	/* Tell the sender its packet died... */
	IP_INC_STATS_BH(dev_net(skb_dst(skb)->dev), IPSTATS_MIB_INHDRERRORS);
	icmp_send(skb, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, 0);
drop:
	kfree_skb(skb);
	return NET_RX_DROP;
}

#ifdef CONFIG_IPV6_TUNNEL_DSLITE
/* Softwire (DS-Lite) */
static int is_device_name_softwire(struct sk_buff *skb)
{
	//struct dst_entry *dst = skb->dst;
	//struct net_device *dev = dst->dev;  
	
	struct net_device *dev = skb_dst(skb)->dev;
	//if(strcmp(dev->name,softwire_device_name) == 0)
	if(strstr (dev->name,softwire_device_name))
	{
		return 1;	
	}
	else
	{
		return 0;	
	}
}
/* Softwire (DS-Lite) */
#endif
