/*
 * drivers/net/ubi32_na.h
 *	Ubicom32 Network Accelerator driver.
 *
 * (C) Copyright 2011, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 */

/*
 * Custom types recognised within the Network Accelerator (Linux independent)
 */
typedef uint8_t mac_addr_t[6];
typedef uint32_t ipv4_addr_t;
typedef uint32_t ipv6_addr_t[4];
#define IN6_ADDR_TO_IPV6_ADDR(ipv6, in6) \
	{ \
		((uint32_t *)ipv6)[0] = in6.in6_u.u6_addr32[0]; \
		((uint32_t *)ipv6)[1] = in6.in6_u.u6_addr32[1]; \
		((uint32_t *)ipv6)[2] = in6.in6_u.u6_addr32[2]; \
		((uint32_t *)ipv6)[3] = in6.in6_u.u6_addr32[3]; \
	}
#define IPV6_ADDR_TO_IN6_ADDR(in6, ipv6) \
	{ \
		in6.in6_u.u6_addr32[0] = ((uint32_t *)ipv6)[0]; \
		in6.in6_u.u6_addr32[1] = ((uint32_t *)ipv6)[1]; \
		in6.in6_u.u6_addr32[2] = ((uint32_t *)ipv6)[2]; \
		in6.in6_u.u6_addr32[3] = ((uint32_t *)ipv6)[3]; \
	}

#define IPV6_ADDR_OCTAL_FMT "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"
#define IPV6_ADDR_TO_OCTAL(ipv6) ((uint16_t *)ipv6)[0], ((uint16_t *)ipv6)[1], ((uint16_t *)ipv6)[2], ((uint16_t *)ipv6)[3], ((uint16_t *)ipv6)[4], ((uint16_t *)ipv6)[5], ((uint16_t *)ipv6)[6], ((uint16_t *)ipv6)[7]

/*
 * IPv4 connection creation structure.
 */
struct ubicom_na_ipv4_create {
	int32_t src_interface_num;
	int32_t dest_interface_num;
	int protocol;
	uint32_t flags;
	uint32_t src_ip;
	int32_t src_port;
	uint32_t src_ip_xlate;
	int32_t src_port_xlate;
	uint32_t dest_ip;
	int32_t dest_port;
	uint32_t dest_ip_xlate;
	int32_t dest_port_xlate;
	uint8_t src_mac[6];
	uint8_t dest_mac[6];
	uint8_t src_mac_xlate[6];
	uint8_t dest_mac_xlate[6];
	uint8_t flow_window_scale;
	uint32_t flow_max_window;
	uint32_t flow_end;
	uint32_t flow_max_end;
	uint8_t return_window_scale;
	uint32_t return_max_window;
	uint32_t return_end;
	uint32_t return_max_end;
	uint8_t spo_needed;
	uint32_t param_a0;
	uint32_t param_a1;
	uint32_t param_a2;
	uint32_t param_a3;
	uint32_t param_a4;
};

/*
 * IPv4 connection flags.
 */
#define UBICOM_NA_IPV4_CREATE_FLAG_NO_SEQ_CHECK 0x1
 					/* Indicates that we should not check sequence numbers */

/*
 * IPv4 connection destruction structure.
 */
struct ubicom_na_ipv4_destroy {
	int protocol;
	uint32_t src_ip;
	int32_t src_port;
	uint32_t dest_ip;
	int32_t dest_port;
};

/*
 * IPv6 connection creation structure.
 */
struct ubicom_na_ipv6_create {
	int32_t src_interface_num;
	int32_t dest_interface_num;
	int protocol;
	uint32_t flags;
	uint32_t src_ip[4];
	int32_t src_port;
	uint32_t dest_ip[4];
	int32_t dest_port;
	uint8_t src_mac[6];
	uint8_t dest_mac[6];
	uint8_t flow_window_scale;
	uint32_t flow_max_window;
	uint32_t flow_end;
	uint32_t flow_max_end;
	uint8_t return_window_scale;
	uint32_t return_max_window;
	uint32_t return_end;
	uint32_t return_max_end;
};

/*
 * IPv6 connection flags.
 */
#define UBICOM_NA_IPV6_CREATE_FLAG_NO_SEQ_CHECK 0x1
 					/* Indicates that we should not check sequence numbers */

/*
 * IPv6 connection destruction structure.
 */
struct ubicom_na_ipv6_destroy {
	int protocol;
	uint32_t src_ip[4];
	int32_t src_port;
	uint32_t dest_ip[4];
	int32_t dest_port;
};

/*
 * struct ubicom_na_ipv4_sync
 *	Update packet stats (bytes / packets seen over a connection) and also keep alive.
 *
 * NOTE: The addresses here are NON-NAT addresses, i.e. the true endpoint addressing.
 * 'src' is the creator of the connection.
 */
struct ubicom_na_ipv4_sync {
	int protocol;			/* IP protocol number (IPPROTO_...) */
	ipv4_addr_t src_addr;		/* Non-NAT source address, i.e. the creator of the connection */
	int32_t src_port;		/* Non-NAT source port */
	ipv4_addr_t src_addr_xlate;	/* NAT translated source address, i.e. the creator of the connection */
	int32_t src_port_xlate;		/* NAT translated source port */
	ipv4_addr_t dest_addr;		/* Non-NAT destination address, i.e. the to whom the connection was created */ 
	int32_t dest_port;		/* Non-NAT destination port */
	ipv4_addr_t dest_addr_xlate;	/* NAT translated destination address, i.e. the to whom the connection was created */ 
	int32_t dest_port_xlate;	/* NAT translated destination port */
	uint32_t flow_max_window;
	uint32_t flow_end;
	uint32_t flow_max_end;
	uint32_t flow_packet_count;
	uint32_t flow_byte_count;
	uint32_t return_max_window;
	uint32_t return_end;
	uint32_t return_max_end;
	uint32_t return_packet_count;
	uint32_t return_byte_count;
	unsigned long int delta_jiffies;
					/* Time in Linux jiffies to be added to the current timeout to keep the connection alive */
	uint8_t final_sync;		/* Non-zero when the NA has ceased to accelerate the given connection */
	uint32_t param_a0;
	uint32_t param_a1;
	uint32_t param_a2;
	uint32_t param_a3;
	uint32_t param_a4;
};

/*
 * struct ubicom_na_ipv6_sync
 *	Update packet stats (bytes / packets seen over a connection) and also keep alive.
 *
 * NOTE: The addresses here are NON-NAT addresses, i.e. the true endpoint addressing.
 * 'src' is the creator of the connection.
 */
struct ubicom_na_ipv6_sync {
	int protocol;			/* IP protocol number (IPPROTO_...) */
	ipv6_addr_t src_addr;		/* Non-NAT source address, i.e. the creator of the connection */
	int32_t src_port;		/* Non-NAT source port */
	ipv6_addr_t dest_addr;		/* Non-NAT destination address, i.e. the to whom the connection was created */ 
	int32_t dest_port;		/* Non-NAT destination port */
	uint32_t flow_max_window;
	uint32_t flow_end;
	uint32_t flow_max_end;
	uint32_t flow_packet_count;
	uint32_t flow_byte_count;
	uint32_t return_max_window;
	uint32_t return_end;
	uint32_t return_max_end;
	uint32_t return_packet_count;
	uint32_t return_byte_count;
	unsigned long int delta_jiffies;
					/* Time in Linux jiffies to be added to the current timeout to keep the connection alive */
	uint8_t final_sync;		/* Non-zero when the NA has ceased to accelerate the given connection */
};

/*
 * General utilities
 */
extern int32_t ubicom_na_interface_number_get(struct net_device *dev);

/*
 * Methods provided by the NA network device driver for use by connection tracking logic for IPv4.
 */
typedef void (*ubicom_na_ipv4_sync_callback_t)(struct ubicom_na_ipv4_sync *unis);
extern void ubicom_na_net_dev_register_ipv4_mgr(ubicom_na_ipv4_sync_callback_t event_callback);
extern void ubicom_na_net_dev_unregister_ipv4_mgr(void);
extern void ubicom_na_create_ipv4_rule(struct ubicom_na_ipv4_create *unic);
extern void ubicom_na_destroy_ipv4_rule(struct ubicom_na_ipv4_destroy *unid);

/*
 * Methods provided by the NA network device driver for use by connection tracking logic for IPv6.
 */
typedef void (*ubicom_na_ipv6_sync_callback_t)(struct ubicom_na_ipv6_sync *unis);
extern void ubicom_na_net_dev_register_ipv6_mgr(ubicom_na_ipv6_sync_callback_t event_callback);
extern void ubicom_na_net_dev_unregister_ipv6_mgr(void);
extern void ubicom_na_create_ipv6_rule(struct ubicom_na_ipv6_create *unic);
extern void ubicom_na_destroy_ipv6_rule(struct ubicom_na_ipv6_destroy *unid);

