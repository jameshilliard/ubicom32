/*
 * drivers/net/ubi32_na.c
 *	Ubicom32 Network Accelerator driver.
 *
 * (C) Copyright 2010, 2011, Ubicom, Inc.
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
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/sysdev.h>
#include <linux/interrupt.h>

#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <net/arp.h>
#include <net/ndisc.h>
#include <asm/devtree.h>
#include "../../net/bridge/br_private.h"

#include <net/ubi32_na.h>

#ifdef NA_DEBUG_ENABLED
#define na_debug(fmt, args...) printk(KERN_DEBUG fmt, ##args)
#define na_info(fmt, args...) printk(KERN_INFO fmt, ##args)
#define na_warning(fmt, args...) printk(KERN_WARNING fmt, ##args)
#else
#define na_debug(fmt, args...)
#define na_info(fmt, args...)
#define na_warning(fmt, args...)
#endif

/*
 * sk_buff buffer size allocated and handed to the NA.
 */
#define NA_SKB_PAYLOAD_SIZE 1792

/*
 * The NA IPv4 rule creation structure.
 */
struct na_ipv4_rule_create {
	uint8_t protocol;		/* Protocol number */
	int32_t flow_interface_num;	/* Flow interface number */
	uint32_t flow_ip;		/* Flow IP address */
	uint32_t flow_ip_xlate;		/* Translated flow IP address */
	uint32_t flow_ident;		/* Flow ident (e.g. port) */
	uint32_t flow_ident_xlate;	/* Translated flow ident (e.g. port) */
	uint16_t flow_mac[3];		/* Flow MAC address */
	uint8_t flow_window_scale;	/* Flow direction's window scaling factor */
	uint32_t flow_max_window;	/* Flow direction's largest seen window */
	uint32_t flow_end;		/* Flow direction's largest seen sequence + segment length */
	uint32_t flow_max_end;		/* Flow direction's largest seen ack + max(1, win) */
	int32_t return_interface_num;	/* Return interface number */
	uint32_t return_ip;		/* Return IP address */
	uint32_t return_ip_xlate;	/* Translated return IP address */
	uint32_t return_ident;		/* Return ident (e.g. port) */
	uint32_t return_ident_xlate;	/* Translated return ident (e.g. port) */
	uint16_t return_mac[3];		/* Return MAC address */
	uint8_t return_window_scale;	/* Return direction's window scaling factor */
	uint32_t return_max_window;	/* Return direction's largest seen window */
	uint32_t return_end;		/* Return direction's largest seen sequence + segment length */
	uint32_t return_max_end;	/* Return direction's largest seen ack + max(1, win) */
	uint8_t flags;			/* Bit flags associated with the rule */
};

/*
 * NA IPv4 rule creation flags.
 */
#define NA_IPV4_RULE_CREATE_FLAG_NO_SEQ_CHECK 0x01
					/* Do not perform sequence number checks */

/*
 * The NA IPv4 rule destruction structure.
 */
struct na_ipv4_rule_destroy {
	uint8_t protocol;		/* Protocol number */
	uint32_t flow_ip;		/* Flow IP address */
	uint32_t flow_ident;		/* Flow ident (e.g. port) */
	uint32_t return_ip;		/* Return IP address */
	uint32_t return_ident;		/* Return ident (e.g. port) */
};

/*
 * The NA IPv6 rule creation structure.
 */
struct na_ipv6_rule_create {
	uint8_t protocol;		/* Protocol number */
	int32_t flow_interface_num;	/* Flow interface number */
	uint32_t flow_ip[4];		/* Flow IP address */
	uint32_t flow_ident;		/* Flow ident (e.g. port) */
	uint16_t flow_mac[3];		/* Flow MAC address */
	uint8_t flow_window_scale;	/* Flow direction's window scaling factor */
	uint32_t flow_max_window;	/* Flow direction's largest seen window */
	uint32_t flow_end;		/* Flow direction's largest seen sequence + segment length */
	uint32_t flow_max_end;		/* Flow direction's largest seen ack + max(1, win) */
	int32_t return_interface_num;	/* Return interface number */
	uint32_t return_ip[4];		/* Return IP address */
	uint32_t return_ident;		/* Return ident (e.g. port) */
	uint16_t return_mac[3];		/* Return MAC address */
	uint8_t return_window_scale;	/* Return direction's window scaling factor */
	uint32_t return_max_window;	/* Return direction's largest seen window */
	uint32_t return_end;		/* Return direction's largest seen sequence + segment length */
	uint32_t return_max_end;	/* Return direction's largest seen ack + max(1, win) */
	uint8_t flags;			/* Bit flags associated with the rule */
};

/*
 * NA IPv6 rule creation flags.
 */
#define NA_IPV6_RULE_CREATE_FLAG_NO_SEQ_CHECK 0x01
					/* Do not perform sequence number checks */

/*
 * The NA IPv6 rule destruction structure.
 */
struct na_ipv6_rule_destroy {
	uint8_t protocol;		/* Protocol number */
	uint32_t flow_ip[4];		/* Flow IP address */
	uint32_t flow_ident;		/* Flow ident (e.g. port) */
	uint32_t return_ip[4];		/* Return IP address */
	uint32_t return_ident;		/* Return ident (e.g. port) */
};

/*
 * The NA MAC address structure.
 */
struct na_mac_address_set {
	uint8_t mac_addr[6];		/* MAC address */
	int32_t interface;		/* Interface for which MAC address will be set */
};

/*
 * Types of TX metadata.
 */
enum na_tx_metadata_types {
	NA_TX_METADATA_TYPE_IPV4_RULE_CREATE,
	NA_TX_METADATA_TYPE_IPV4_RULE_DESTROY,
	NA_TX_METADATA_TYPE_IPV6_RULE_CREATE,
	NA_TX_METADATA_TYPE_IPV6_RULE_DESTROY,
	NA_TX_METADATA_TYPE_MAC_ADDR_SET,
	NA_TX_METADATA_TYPE_DESTROY_ALL_RULES
};

/*
 * Structure that describes all TX metadata objects.
 */
struct na_tx_metadata_object {
	enum na_tx_metadata_types type;	/* Object type */
	union {				/* Sub-message type */
		struct na_ipv4_rule_create ipv4_rule_create;
		struct na_ipv4_rule_destroy ipv4_rule_destroy;
		struct na_ipv6_rule_create ipv6_rule_create;
		struct na_ipv6_rule_destroy ipv6_rule_destroy;
		struct na_mac_address_set mac_address_set;
	} sub;
};

/*
 * The NA IPv4 rule establish structure.
 */
struct na_ipv4_rule_establish {
	uint32_t index;			/* Slot ID for cache stats to host OS */
	uint8_t protocol;		/* Protocol number */
	int32_t flow_interface;		/* Flow interface number */
	uint32_t flow_ip;		/* Flow IP address */
	uint32_t flow_ip_xlate;		/* Translated flow IP address */
	uint32_t flow_ident;		/* Flow ident (e.g. port) */
	uint32_t flow_ident_xlate;	/* Translated flow ident (e.g. port) */
	int32_t return_interface;	/* Return interface number */
	uint32_t return_ip;		/* Return IP address */
	uint32_t return_ip_xlate;	/* Translated return IP address */
	uint32_t return_ident;		/* Return ident (e.g. port) */
	uint32_t return_ident_xlate;	/* Translated return ident (e.g. port) */
};

/*
 * The NA IPv4 rule sync structure.
 */
struct na_ipv4_rule_sync {
	uint32_t index;			/* Slot ID for cache stats to host OS */
	uint32_t flow_max_window;	/* Flow direction's largest seen window */
	uint32_t flow_end;		/* Flow direction's largest seen sequence + segment length */
	uint32_t flow_max_end;		/* Flow direction's largest seen ack + max(1, win) */
	uint32_t flow_rx_packet_count;	/* Flow interface's RX packet count */
	uint32_t flow_rx_byte_count;	/* Flow interface's RX byte count */
	uint32_t flow_tx_packet_count;	/* Flow interface's TX packet count */
	uint32_t flow_tx_byte_count;	/* Flow interface's TX byte count */
	uint32_t return_max_window;	/* Return direction's largest seen window */
	uint32_t return_end;		/* Return direction's largest seen sequence + segment length */
	uint32_t return_max_end;	/* Return direction's largest seen ack + max(1, win) */
	uint32_t return_rx_packet_count;
					/* Return interface's RX packet count */
	uint32_t return_rx_byte_count;	/* Return interface's RX byte count */
	uint32_t return_tx_packet_count;
					/* Return interface's TX packet count */
	uint32_t return_tx_byte_count;	/* Return interface's TX byte count */
	uint32_t inc_ticks;		/* Number of ticks since the last sync */
	uint32_t reason;		/* Reason for the sync */
};

/*
 * Rule sync reasons.
 */
#define NA_IPV4_RULE_SYNC_REASON_STATS 0
					/* Sync is to synchronize stats */
#define NA_IPV4_RULE_SYNC_REASON_FLUSH 1
					/* Sync is to flush a cache entry */
#define NA_IPV4_RULE_SYNC_REASON_EVICT 2
					/* Sync is to evict a cache entry */
#define NA_IPV4_RULE_SYNC_REASON_DESTROY 3
					/* Sync is to destroy a cache entry (requested by host OS) */

/*
 * The NA IPv6 rule establish structure.
 */
struct na_ipv6_rule_establish {
	uint32_t index;			/* Slot ID for cache stats to host OS */
	uint8_t protocol;		/* Protocol number */
	int32_t flow_interface;		/* Flow interface number */
	uint32_t flow_ip[4];		/* Flow IP address */
	uint32_t flow_ident;		/* Flow ident (e.g. port) */
	int32_t return_interface;	/* Return interface number */
	uint32_t return_ip[4];		/* Return IP address */
	uint32_t return_ident;		/* Return ident (e.g. port) */
};

/*
 * The NA IPv6 rule sync structure.
 */
struct na_ipv6_rule_sync {
	uint32_t index;			/* Slot ID for cache stats to host OS */
	uint32_t flow_max_window;	/* Flow direction's largest seen window */
	uint32_t flow_end;		/* Flow direction's largest seen sequence + segment length */
	uint32_t flow_max_end;		/* Flow direction's largest seen ack + max(1, win) */
	uint32_t flow_rx_packet_count;	/* Flow interface's RX packet count */
	uint32_t flow_rx_byte_count;	/* Flow interface's RX byte count */
	uint32_t flow_tx_packet_count;	/* Flow interface's TX packet count */
	uint32_t flow_tx_byte_count;	/* Flow interface's TX byte count */
	uint32_t return_max_window;	/* Return direction's largest seen window */
	uint32_t return_end;		/* Return direction's largest seen sequence + segment length */
	uint32_t return_max_end;	/* Return direction's largest seen ack + max(1, win) */
	uint32_t return_rx_packet_count;
					/* Return interface's RX packet count */
	uint32_t return_rx_byte_count;	/* Return interface's RX byte count */
	uint32_t return_tx_packet_count;
					/* Return interface's TX packet count */
	uint32_t return_tx_byte_count;	/* Return interface's TX byte count */
	uint32_t inc_ticks;		/* Number of ticks since the last sync */
	uint32_t reason;		/* Reason for the sync */
};

/*
 * Rule sync reasons.
 */
#define NA_IPV6_RULE_SYNC_REASON_STATS 0
					/* Sync is to synchronize stats */
#define NA_IPV6_RULE_SYNC_REASON_FLUSH 1
					/* Sync is to flush a cache entry */
#define NA_IPV6_RULE_SYNC_REASON_EVICT 2
					/* Sync is to evict a cache entry */
#define NA_IPV6_RULE_SYNC_REASON_DESTROY 3
					/* Sync is to destroy a cache entry (requested by host OS) */

/*
 * The NA per-GMAC statistics sync structure.
 */
struct na_gmac_stats_sync {
	int32_t interface;		/* Interface number */
	uint32_t rx_bytes;		/* Number of RX bytes */
	uint32_t rx_packets;		/* Number of RX packets */
	uint32_t rx_errors;		/* Number of RX errors */
	uint32_t rx_receive_errors;	/* Number of RX receive errors */
	uint32_t rx_overflow_errors;	/* Number of RX overflow errors */
	uint32_t rx_descriptor_errors;	/* Number of RX descriptor errors */
	uint32_t rx_watchdog_timeout_errors;
					/* Number of RX watchdog timeout errors */
	uint32_t rx_crc_errors;		/* Number of RX CRC errors */
	uint32_t rx_late_collision_errors;
					/* Number of RX late collision errors */
	uint32_t rx_dribble_bit_errors;	/* Number of RX dribble bit errors */
	uint32_t rx_length_errors;	/* Number of RX length errors */
	uint32_t rx_ip_header_errors;	/* Number of RX IP header errors */
	uint32_t rx_ip_payload_errors;	/* Number of RX IP payload errors */
	uint32_t rx_no_buffer_errors;	/* Number of RX no-buffer errors */
	uint32_t rx_transport_csum_bypassed;
					/* Number of RX packets where the transport checksum was bypassed */
	uint32_t tx_bytes;		/* Number of TX bytes */
	uint32_t tx_packets;		/* Number of TX packets */
	uint32_t tx_collisions;		/* Number of TX collisions */
	uint32_t tx_errors;		/* Number of TX errors */
	uint32_t tx_jabber_timeout_errors;
					/* Number of TX jabber timeout errors */
	uint32_t tx_frame_flushed_errors;
					/* Number of TX frame flushed errors */
	uint32_t tx_loss_of_carrier_errors;
					/* Number of TX loss of carrier errors */
	uint32_t tx_no_carrier_errors;	/* Number of TX no carrier errors */
	uint32_t tx_late_collision_errors;
					/* Number of TX late collision errors */
	uint32_t tx_excessive_collision_errors;
					/* Number of TX excessive collision errors */
	uint32_t tx_excessive_deferral_errors;
					/* Number of TX excessive deferral errors */
	uint32_t tx_underflow_errors;	/* Number of TX underflow errors */
	uint32_t tx_ip_header_errors;	/* Number of TX IP header errors */
	uint32_t tx_ip_payload_errors;	/* Number of TX IP payload errors */
	uint32_t tx_dropped;		/* Number of TX dropped packets */
	uint32_t hw_errs[10];		/* GMAC DMA error counters */
	uint32_t rx_missed;		/* Number of RX packets missed by the DMA */
	uint32_t fifo_overflows;	/* Number of RX FIFO overflows signalled by the DMA */
	uint32_t gmac_total_ticks;	/* Total clock ticks spend inside the GMAC */
	uint32_t gmac_worst_case_ticks;	/* Worst case iteration of the GMAC in ticks */
	uint32_t gmac_iterations;	/* Number of iterations around the GMAC */
};

/*
 * Exeption events from PE
 */
enum exception_events_unknown {
	NA_EXCEPTION_EVENT_UNKNOWN_L2_PROTOCOL,
	NA_EXCEPTION_EVENT_UNKNOWN_LAST
};

/*
 * Exeption events from PE
 */
enum exception_events_ipv4 {
	NA_EXCEPTION_EVENT_IPV4_ICMP_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV4_ICMP_UNHANDLED_TYPE,
	NA_EXCEPTION_EVENT_IPV4_ICMP_IPV4_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV4_ICMP_IPV4_UDP_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV4_ICMP_IPV4_TCP_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV4_ICMP_IPV4_UNKNOWN_PROTOCOL,
	NA_EXCEPTION_EVENT_IPV4_ICMP_NO_ICME,
	NA_EXCEPTION_EVENT_IPV4_ICMP_FLUSH_TO_HOST,
	NA_EXCEPTION_EVENT_IPV4_TCP_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV4_TCP_NO_ICME,
	NA_EXCEPTION_EVENT_IPV4_TCP_IP_OPTION,
	NA_EXCEPTION_EVENT_IPV4_TCP_IP_FRAGMENT,
	NA_EXCEPTION_EVENT_IPV4_TCP_SMALL_TTL,
	NA_EXCEPTION_EVENT_IPV4_TCP_NEEDS_FRAGMENTATION,
	NA_EXCEPTION_EVENT_IPV4_TCP_FLAGS,
	NA_EXCEPTION_EVENT_IPV4_TCP_SEQ_EXCEEDS_RIGHT_EDGE,
	NA_EXCEPTION_EVENT_IPV4_TCP_SMALL_DATA_OFFS,
	NA_EXCEPTION_EVENT_IPV4_TCP_BAD_SACK,
	NA_EXCEPTION_EVENT_IPV4_TCP_BIG_DATA_OFFS,
	NA_EXCEPTION_EVENT_IPV4_TCP_SEQ_BEFORE_LEFT_EDGE,
	NA_EXCEPTION_EVENT_IPV4_TCP_ACK_EXCEEDS_RIGHT_EDGE,
	NA_EXCEPTION_EVENT_IPV4_TCP_ACK_BEFORE_LEFT_EDGE,
	NA_EXCEPTION_EVENT_IPV4_UDP_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV4_UDP_NO_ICME,
	NA_EXCEPTION_EVENT_IPV4_UDP_IP_OPTION,
	NA_EXCEPTION_EVENT_IPV4_UDP_IP_FRAGMENT,
	NA_EXCEPTION_EVENT_IPV4_UDP_SMALL_TTL,
	NA_EXCEPTION_EVENT_IPV4_UDP_NEEDS_FRAGMENTATION,
	NA_EXCEPTION_EVENT_IPV4_WRONG_TARGET_MAC,
	NA_EXCEPTION_EVENT_IPV4_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV4_BAD_TOTAL_LENGTH,
	NA_EXCEPTION_EVENT_IPV4_BAD_CHECKSUM,
	NA_EXCEPTION_EVENT_IPV4_NON_INITIAL_FRAGMENT,
	NA_EXCEPTION_EVENT_IPV4_DATAGRAM_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV4_OPTIONS_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV4_UNKNOWN_PROTOCOL,
	NA_EXCEPTION_EVENT_IPV4_LAST
};

/*
 * Exeption events from PE
 */
enum exception_events_ipv6 {
	NA_EXCEPTION_EVENT_IPV6_ICMP_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV6_ICMP_UNHANDLED_TYPE,
	NA_EXCEPTION_EVENT_IPV6_ICMP_IPV6_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV6_ICMP_IPV6_UDP_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV6_ICMP_IPV6_TCP_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV6_ICMP_IPV6_UNKNOWN_PROTOCOL,
	NA_EXCEPTION_EVENT_IPV6_ICMP_NO_ICME,
	NA_EXCEPTION_EVENT_IPV6_ICMP_FLUSH_TO_HOST,
	NA_EXCEPTION_EVENT_IPV6_TCP_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV6_TCP_NO_ICME,
	NA_EXCEPTION_EVENT_IPV6_TCP_SMALL_HOP_LIMIT,
	NA_EXCEPTION_EVENT_IPV6_TCP_NEEDS_FRAGMENTATION,
	NA_EXCEPTION_EVENT_IPV6_TCP_FLAGS,
	NA_EXCEPTION_EVENT_IPV6_TCP_SEQ_EXCEEDS_RIGHT_EDGE,
	NA_EXCEPTION_EVENT_IPV6_TCP_SMALL_DATA_OFFS,
	NA_EXCEPTION_EVENT_IPV6_TCP_BAD_SACK,
	NA_EXCEPTION_EVENT_IPV6_TCP_BIG_DATA_OFFS,
	NA_EXCEPTION_EVENT_IPV6_TCP_SEQ_BEFORE_LEFT_EDGE,
	NA_EXCEPTION_EVENT_IPV6_TCP_ACK_EXCEEDS_RIGHT_EDGE,
	NA_EXCEPTION_EVENT_IPV6_TCP_ACK_BEFORE_LEFT_EDGE,
	NA_EXCEPTION_EVENT_IPV6_UDP_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV6_UDP_NO_ICME,
	NA_EXCEPTION_EVENT_IPV6_UDP_SMALL_HOP_LIMIT,
	NA_EXCEPTION_EVENT_IPV6_UDP_NEEDS_FRAGMENTATION,
	NA_EXCEPTION_EVENT_IPV6_WRONG_TARGET_MAC,
	NA_EXCEPTION_EVENT_IPV6_HEADER_INCOMPLETE,
	NA_EXCEPTION_EVENT_IPV6_UNKNOWN_PROTOCOL,
	NA_EXCEPTION_EVENT_IPV6_LAST
};

static char *exception_events_unknown_string[NA_EXCEPTION_EVENT_UNKNOWN_LAST] = {
	"UNKNOWN_L2_PROTOCOL"
};

static char *exception_events_ipv4_string[NA_EXCEPTION_EVENT_IPV4_LAST] = {
	"IPV4_ICMP_HEADER_INCOMPLETE",
	"IPV4_ICMP_UNHANDLED_TYPE",
	"IPV4_ICMP_IPV4_HEADER_INCOMPLETE",
	"IPV4_ICMP_IPV4_UDP_HEADER_INCOMPLETE",
	"IPV4_ICMP_IPV4_TCP_HEADER_INCOMPLETE",
	"IPV4_ICMP_IPV4_UNKNOWN_PROTOCOL",
	"IPV4_ICMP_NO_ICME",
	"IPV4_ICMP_FLUSH_TO_HOST",
	"IPV4_TCP_HEADER_INCOMPLETE",
	"IPV4_TCP_NO_ICME",
	"IPV4_TCP_IP_OPTION",
	"IPV4_TCP_IP_FRAGMENT",
	"IPV4_TCP_SMALL_TTL",
	"IPV4_TCP_NEEDS_FRAGMENTATION",
	"IPV4_TCP_FLAGS",
	"IPV4_TCP_SEQ_EXCEEDS_RIGHT_EDGE",
	"IPV4_TCP_SMALL_DATA_OFFS",
	"IPV4_TCP_BAD_SACK",
	"IPV4_TCP_BIG_DATA_OFFS",
	"IPV4_TCP_SEQ_BEFORE_LEFT_EDGE",
	"IPV4_TCP_ACK_EXCEEDS_RIGHT_EDGE",
	"IPV4_TCP_ACK_BEFORE_LEFT_EDGE",
	"IPV4_UDP_HEADER_INCOMPLETE",
	"IPV4_UDP_NO_ICME",
	"IPV4_UDP_IP_OPTION",
	"IPV4_UDP_IP_FRAGMENT",
	"IPV4_UDP_SMALL_TTL",
	"IPV4_UDP_NEEDS_FRAGMENTATION",
	"IPV4_WRONG_TARGET_MAC",
	"IPV4_HEADER_INCOMPLETE",
	"IPV4_BAD_TOTAL_LENGTH",
	"IPV4_BAD_CHECKSUM",
	"IPV4_NON_INITIAL_FRAGMENT",
	"IPV4_DATAGRAM_INCOMPLETE",
	"IPV4_OPTIONS_INCOMPLETE",
	"IPV4_UNKNOWN_PROTOCOL"
};

static char *exception_events_ipv6_string[NA_EXCEPTION_EVENT_IPV6_LAST] = {
	"IPV6_ICMP_HEADER_INCOMPLETE",
	"IPV6_ICMP_UNHANDLED_TYPE",
	"IPV6_ICMP_IPV6_HEADER_INCOMPLETE",
	"IPV6_ICMP_IPV6_UDP_HEADER_INCOMPLETE",
	"IPV6_ICMP_IPV6_TCP_HEADER_INCOMPLETE",
	"IPV6_ICMP_IPV6_UNKNOWN_PROTOCOL",
	"IPV6_ICMP_NO_ICME",
	"IPV6_ICMP_FLUSH_TO_HOST",
	"IPV6_TCP_HEADER_INCOMPLETE",
	"IPV6_TCP_NO_ICME",
	"IPV6_TCP_SMALL_HOP_LIMIT",
	"IPV6_TCP_NEEDS_FRAGMENTATION",
	"IPV6_TCP_FLAGS",
	"IPV6_TCP_SEQ_EXCEEDS_RIGHT_EDGE",
	"IPV6_TCP_SMALL_DATA_OFFS",
	"IPV6_TCP_BAD_SACK",
	"IPV6_TCP_BIG_DATA_OFFS",
	"IPV6_TCP_SEQ_BEFORE_LEFT_EDGE",
	"IPV6_TCP_ACK_EXCEEDS_RIGHT_EDGE",
	"IPV6_TCP_ACK_BEFORE_LEFT_EDGE",
	"IPV6_UDP_HEADER_INCOMPLETE",
	"IPV6_UDP_NO_ICME",
	"IPV6_UDP_SMALL_HOP_LIMIT",
	"IPV6_UDP_NEEDS_FRAGMENTATION",
	"IPV6_WRONG_TARGET_MAC",
	"IPV6_HEADER_INCOMPLETE",
	"IPV6_UNKNOWN_PROTOCOL"
};

/*
 * The NA per-interface statistics sync structure.
 */
struct na_interface_stats_sync {
	int32_t interface;		/* Interface number */
	uint32_t rx_packets;		/* Number of packets received */
	uint32_t rx_bytes;		/* Number of bytes received */
	uint32_t tx_packets;		/* Number of packets transmitted */
	uint32_t tx_bytes;		/* Number of bytes transmitted */
	uint32_t rx_errors;		/* Number of receive errors */
	uint32_t tx_errors;		/* Number of transmit errors */
	uint32_t tx_dropped;		/* Number of TX dropped packets */
	uint32_t collisions;		/* Number of TX and RX collisions */
	uint32_t host_rx_packets;	/* Number of RX packets received by host OS */
	uint32_t host_rx_bytes;		/* Number of RX bytes received by host OS */
	uint32_t host_tx_packets;	/* Number of TX packets sent by host OS */
	uint32_t host_tx_bytes;		/* Number of TX bytes sent by host OS */
	uint32_t rx_length_errors;	/* Number of RX length errors */
	uint32_t rx_overflow_errors;	/* Number of RX overflow errors */
	uint32_t rx_crc_errors;		/* Number of RX CRC errors */
	uint32_t exception_events_unknown[NA_EXCEPTION_EVENT_UNKNOWN_LAST];
					/* Number of unknown protocol exception events */
	uint32_t exception_events_ipv4[NA_EXCEPTION_EVENT_IPV4_LAST];
					/* Number of IPv4 exception events */
	uint32_t exception_events_ipv6[NA_EXCEPTION_EVENT_IPV6_LAST];
					/* Number of IPv6 exception events */
};

/*
 * The NA NA statistics sync structure.
 */
struct na_na_stats_sync {
	uint32_t ipv4_connection_create_requests;
					/* Number of IPv4 connection create requests */
	uint32_t ipv4_connection_create_collisions;
					/* Number of IPv4 connection create requests that collided with existing entries */
	uint32_t ipv4_connection_destroy_requests;
					/* Number of IPv4 connection destroy requests */
	uint32_t ipv4_connection_destroy_misses;
					/* Number of IPv4 connection destroy requests that missed the cache */
	uint32_t ipv4_connection_hash_hits;
					/* Number of IPv4 connection hash hits */
	uint32_t ipv4_connection_hash_reorders;
					/* Number of IPv4 connection hash reorders */
	uint32_t ipv4_connection_flushes;
					/* Number of IPv4 connection flushes */
	uint32_t ipv4_connection_evictions;
					/* Number of IPv4 connection evictions */
	uint32_t ipv6_connection_create_requests;
					/* Number of IPv6 connection create requests */
	uint32_t ipv6_connection_create_collisions;
					/* Number of IPv6 connection create requests that collided with existing entries */
	uint32_t ipv6_connection_destroy_requests;
					/* Number of IPv6 connection destroy requests */
	uint32_t ipv6_connection_destroy_misses;
					/* Number of IPv6 connection destroy requests that missed the cache */
	uint32_t ipv6_connection_hash_hits;
					/* Number of IPv6 connection hash hits */
	uint32_t ipv6_connection_hash_reorders;
					/* Number of IPv6 connection hash reorders */
	uint32_t ipv6_connection_flushes;
					/* Number of IPv6 connection flushes */
	uint32_t ipv6_connection_evictions;
					/* Number of IPv6 connection evictions */
	uint32_t pe_queue_dropped;	/* Number of packets dropped because the PE queue is too full */
	uint32_t pe_total_ticks;	/* Total clock ticks spend inside the PE */
	uint32_t pe_worst_case_ticks;	/* Worst case iteration of the PE in ticks */
	uint32_t pe_iterations;		/* Number of iterations around the PE */
	uint32_t except_queue_dropped;	/* Number of packets dropped because the exception queue is too full */
	uint32_t except_total_ticks;	/* Total clock ticks spend inside the PE */
	uint32_t except_worst_case_ticks;
					/* Worst case iteration of the exception path in ticks */
	uint32_t except_iterations;	/* Number of iterations around the PE */
	uint32_t pbuf_alloc_fails;	/* Number of pbuf allocations that have failed */
	uint32_t pbuf_payload_alloc_fails;
					/* Number of pbuf allocations that have failed because there were no free payloads */
};

/*
 * Types of RX metadata.
 */
enum na_rx_metadata_types {
	NA_RX_METADATA_TYPE_IPV4_RULE_ESTABLISH,
	NA_RX_METADATA_TYPE_IPV4_RULE_SYNC,
	NA_RX_METADATA_TYPE_IPV6_RULE_ESTABLISH,
	NA_RX_METADATA_TYPE_IPV6_RULE_SYNC,
	NA_RX_METADATA_TYPE_GMAC_STATS_SYNC,
	NA_RX_METADATA_TYPE_INTERFACE_STATS_SYNC,
	NA_RX_METADATA_TYPE_NA_STATS_SYNC
};

/*
 * Structure that describes all RX metadata objects.
 */
struct na_rx_metadata_object {
	enum na_rx_metadata_types type;	/* Object type */
	union {				/* Sub-message type */
		struct na_ipv4_rule_establish ipv4_rule_establish;
		struct na_ipv4_rule_sync ipv4_rule_sync;
		struct na_ipv6_rule_establish ipv6_rule_establish;
		struct na_ipv6_rule_sync ipv6_rule_sync;
		struct na_gmac_stats_sync gmac_stats_sync;
		struct na_interface_stats_sync interface_stats_sync;
		struct na_na_stats_sync na_stats_sync;
	} sub;
};

#define NA_MAX_INTERFACES 2		/* Maximum number of interfaces we support */
#define NA_NUM_TX_DESCRIPTORS 128	/* Number of transmit descriptors */
#define NA_NUM_RX_DESCRIPTORS 128	/* Number of receive descriptors */
#define NA_NUM_PUSH_DESCRIPTORS 128	/* Number of push (new) buffer descriptors */
#define NA_NUM_POP_DESCRIPTORS 128	/* Number of pop (old) buffer descriptors */

/*
 * The NA per-interface fields of the NA memory map.
 */
struct na_if_map_interface {
	volatile uint32_t status;	/* Per-interface status words */
	volatile uint32_t control;	/* Control word */
};

/*
 * na_if_map status "register" bit definitions.
 */
#define NA_IF_MAP_INTERFACE_STATUS_LINK_UP 0x00000001
					/* Interface link is up */

/*
 * The NA descriptor structure.  This structure is common for both transmit and receive.
 */
struct na_desc {
	volatile uint8_t status;	/* Status */
	volatile int8_t interface_num;	/* Interface number */
	volatile uint16_t buffer_len;	/* Length of buffer */
	volatile uint32_t buffer;	/* Buffer address */
	volatile uint16_t payload_offs;	/* Offset from the start of the buffer to the payload */
	volatile uint16_t payload_len;	/* Offset from the start of the buffer to the payload */
	volatile uint32_t opaque;	/* Opaque reference used by host OS */
};

#define NA_DESC_STATUS_PKT 0x01		/* Descriptor is for packet data (0x01) or metadata (0x00) */
#define NA_DESC_STATUS_IPV4_IP_CHECKSUM_VALID 0x02
					/* Descriptor is for packet data where IPv4 header checksum is valid */
#define NA_DESC_STATUS_GEN_IPV4_IP_CHECKSUM 0x04
					/* Descriptor is for packet data where the IPv4 header checksum should be generated */
#define NA_DESC_STATUS_IP_TRANSPORT_CHECKSUM_VALID 0x08
					/* Descriptor is for packet data where the IP transport checksum is valid */
#define NA_DESC_STATUS_GEN_IP_TRANSPORT_CHECKSUM 0x10
					/* Descriptor is for packet data where the IP transport checksum should be generated */
#define NA_DESC_STATUS_FIRST_SEGMENT 0x20
					/* Descriptor contains the first segment of a frame */
#define NA_DESC_STATUS_LAST_SEGMENT 0x40
					/* Descriptor contains the last segment of a frame */
#define NA_DESC_STATUS_DISCARD 0x80	/* Descriptor is for a packet payload that should be discarded by the NA */

/*
 * statistics struct
 */
#define IPV4_CONNECTION_ENTRIES 256
#define IPV6_CONNECTION_ENTRIES 256

/*
 * The NA memory map.  This is a set of pseudo registers and descriptor
 * arrays exposed to the host OS.
 */
struct na_if_map {
	/*
	 * Per-interface info.
	 */
	struct na_if_map_interface per_if[NA_MAX_INTERFACES];

	/*
	 * NA status info.
	 */
	volatile uint32_t int_status;	/* Summary interrupt status word */
	volatile uint32_t int_enable;	/* Interrupt enable mask */

	/*
	 * Descriptor indicies and arrays used for packet processing.
	 */
	volatile uint32_t push_host_index;
					/* Push descriptor index for the next packet buffer to be pushed (host view) */
	const volatile uint32_t push_na_index;
					/* Push descriptor index for the next packet buffer to be pushed (NA view) */
	volatile uint32_t tx_host_index;
					/* TX descriptor index for the next packet to be sent (host view) */
	const volatile uint32_t tx_na_index;
					/* TX descriptor index for the next packet to be send (NA view) */
	const volatile uint32_t rx_na_index;
					/* RX descriptor index for the next packet to be received (NA view) */
	volatile uint32_t rx_host_index;
					/* RX descriptor index for the next packet to be received (host view) */
	const volatile uint32_t pop_na_index;
					/* Pop descriptor index for the next packet buffer to be popped (NA view) */
	volatile uint32_t pop_host_index;
					/* Pop descriptor index for the next packet buffer to be popped (host view) */

	struct na_desc rx_desc[NA_NUM_RX_DESCRIPTORS];
	struct na_desc tx_desc[NA_NUM_TX_DESCRIPTORS];
	struct na_desc push_desc[NA_NUM_PUSH_DESCRIPTORS];
	struct na_desc pop_desc[NA_NUM_POP_DESCRIPTORS];

	/*
	 * Info used by the host OS.
	 */
	const uint32_t na_int;		/* Interrupt used by the host OS to signal the NA */
	const uint32_t na_int_bit;	/* Interrupt bit that matches na_int */
	const uint32_t host_int;	/* Interrupt used by the NA to signal the host OS */
	const uint32_t host_int_bit;	/* Interrupt bit that matches host_int */
};

/*
 * na_if_map int_status "register" bit definitions.
 */
#define NA_IF_MAP_INT_STATUS_RX 0x00000001
					/* RX packets available */
#define NA_IF_MAP_INT_STATUS_POP 0x00000002
					/* Packet buffers available to be popped */
#define NA_IF_MAP_INT_STATUS_PUSH 0x00000004
					/* Packet buffers need to be pushed */
#define NA_IF_MAP_INT_STATUS_TX 0x00000008
					/* TX slots available */

/*
 * na_if_map int_enable "register" bit definitions.
 */
#define NA_IF_MAP_INT_ENABLE_RX 0x00000001
					/* RX packets available */
#define NA_IF_MAP_INT_ENABLE_POP 0x00000002
					/* Packet buffers available to be popped */
#define NA_IF_MAP_INT_ENABLE_PUSH 0x00000004
					/* Packet buffers need to be pushed */
#define NA_IF_MAP_INT_ENABLE_TX 0x00000008
					/* TX slots available */

/*
 * na_if_map control "register" bit definitions.
 */
#define NA_IF_MAP_CONTROL_RX_ENABLE 0x00000001
					/* RX enabled */
#define NA_IF_MAP_CONTROL_TX_ENABLE 0x00000002
					/* TX enabled */

/*
 * NA device tree node definition.
 */
struct na_devtree_node {
	struct devtree_node dn;		/* Base class (must be first) */
	uint32_t num_interfaces;	/* Number of interfaces */
	struct na_if_map *map;		/* Pointer to the NA interface memory map */
};

#define UBI32_NA_VP_TX_TIMEOUT (10 * HZ)

struct ubi32_na_private {
	struct net_device *dev;		/* Linux net device structure */
	uint32_t magic;			/* Used to confirm this private area is an NA private area */
	struct na_if_map_interface *nimi;
					/* Per-interface fields in the NA memory map */
	int32_t interface_num;		/* Interface number */

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
	struct vlan_group *vlgrp;	/* VLAN acceleration info */
#endif
	struct rtnl_link_stats64 stats64;
					/* Host OS 64bit statistics structure */
	uint64_t host_rx_packets;	/* Number of RX packets received by host OS */
	uint64_t host_rx_bytes;		/* Number of RX bytes received by host OS */
	uint64_t host_tx_packets;	/* Number of TX packets sent by host OS */
	uint64_t host_tx_bytes;		/* Number of TX bytes sent by host OS */
	uint64_t ipv4_accelerated_rx_packets;
					/* Accelerated IPv4 RX packets */
	uint64_t ipv4_accelerated_rx_bytes;
					/* Accelerated IPv4 RX bytes */
	uint64_t ipv4_accelerated_tx_packets;
					/* Accelerated IPv4 TX packets */
	uint64_t ipv4_accelerated_tx_bytes;
					/* Accelerated IPv4 TX bytes */
	uint64_t ipv6_accelerated_rx_packets;
					/* Accelerated IPv6 RX packets */
	uint64_t ipv6_accelerated_rx_bytes;
					/* Accelerated IPv6 RX bytes */
	uint64_t ipv6_accelerated_tx_packets;
					/* Accelerated IPv6 TX packets */
	uint64_t ipv6_accelerated_tx_bytes;
					/* Accelerated IPv6 TX bytes */
	uint64_t exception_events_unknown[NA_EXCEPTION_EVENT_UNKNOWN_LAST];
					/* Unknown protocol exception events */
	uint64_t exception_events_ipv4[NA_EXCEPTION_EVENT_IPV4_LAST];
					/* IPv4 protocol exception events */
	uint64_t exception_events_ipv6[NA_EXCEPTION_EVENT_IPV6_LAST];
					/* IPv6 protocol exception events */
};

struct na_ipv4_statistics {
	uint8_t protocol;		/* Protocol number */
	int32_t flow_interface;		/* Flow interface number */
	uint32_t flow_ip;		/* Flow IP address */
	uint32_t flow_ip_xlate;		/* Flow IP address after NAT translation */
	uint32_t flow_ident;		/* Flow ident (e.g. port) */
	uint32_t flow_ident_xlate;	/* Flow ident (e.g. port) after NAT translation */
	uint64_t flow_accelerated_rx_packets;
					/* Number of flow interface RX packets accelerated */
	uint64_t flow_accelerated_rx_bytes;
					/* Number of flow interface RX bytes accelerated */
	uint64_t flow_accelerated_tx_packets;
					/* Number of flow interface TX packets accelerated */
	uint64_t flow_accelerated_tx_bytes;
					/* Number of flow interface TX bytes accelerated */
	int32_t return_interface;	/* Return interface number */
	uint32_t return_ip;		/* Return IP address */
	uint32_t return_ip_xlate;	/* Return IP address after NAT translation */
	uint32_t return_ident;		/* Return ident (e.g. port) */
	uint32_t return_ident_xlate;	/* Return ident (e.g. port) after NAT translation */
	uint64_t return_accelerated_rx_packets;
					/* Number of return interface RX packets accelerated */
	uint64_t return_accelerated_rx_bytes;
					/* Number of return interface RX bytes accelerated */
	uint64_t return_accelerated_tx_packets;
					/* Number of return interface TX packets accelerated */
	uint64_t return_accelerated_tx_bytes;
					/* Number of return interface TX bytes accelerated */
	uint64_t last_sync;		/* Last sync time as jiffies */
};

struct na_ipv6_statistics {
	uint8_t protocol;		/* Protocol number */
	int32_t flow_interface;		/* Flow interface number */
	uint32_t flow_ip[4];		/* Flow IP address */
	uint32_t flow_ident;		/* Flow ident (e.g. port) */
	uint64_t flow_accelerated_rx_packets;
					/* Number of flow interface RX packets accelerated */
	uint64_t flow_accelerated_rx_bytes;
					/* Number of flow interface RX bytes accelerated */
	uint64_t flow_accelerated_tx_packets;
					/* Number of flow interface TX packets accelerated */
	uint64_t flow_accelerated_tx_bytes;
					/* Number of flow interface TX bytes accelerated */
	uint32_t return_ip[4];		/* Return IP address */
	uint32_t return_ident;		/* Return ident (e.g. port) */
	int32_t return_interface;	/* Return interface number */
	uint64_t return_accelerated_rx_packets;
					/* Number of return interface RX packets accelerated */
	uint64_t return_accelerated_rx_bytes;
					/* Number of return interface RX bytes accelerated */
	uint64_t return_accelerated_tx_packets;
					/* Number of return interface TX packets accelerated */
	uint64_t return_accelerated_tx_bytes;
					/* Number of return interface TX bytes accelerated */
	uint64_t last_sync;		/* Last sync time as jiffies */
};

struct na_gmac_statistics {
	uint64_t rx_bytes;		/* Number of RX bytes */
	uint64_t rx_packets;		/* Number of RX packets */
	uint64_t rx_errors;		/* Number of RX errors */
	uint64_t rx_receive_errors;	/* Number of RX receive errors */
	uint64_t rx_overflow_errors;	/* Number of RX overflow errors */
	uint64_t rx_descriptor_errors;	/* Number of RX descriptor errors */
	uint64_t rx_watchdog_timeout_errors;
					/* Number of RX watchdog timeout errors */
	uint64_t rx_crc_errors;		/* Number of RX CRC errors */
	uint64_t rx_late_collision_errors;
					/* Number of RX late collision errors */
	uint64_t rx_dribble_bit_errors;	/* Number of RX dribble bit errors */
	uint64_t rx_length_errors;	/* Number of RX length errors */
	uint64_t rx_ip_header_errors;	/* Number of RX IP header errors */
	uint64_t rx_ip_payload_errors;	/* Number of RX IP payload errors */
	uint64_t rx_no_buffer_errors;	/* Number of RX no-buffer errors */
	uint64_t rx_transport_csum_bypassed;
					/* Number of RX packets where the transport checksum was bypassed */
	uint64_t tx_bytes;		/* Number of TX bytes */
	uint64_t tx_packets;		/* Number of TX packets */
	uint64_t tx_collisions;		/* Number of TX collisions */
	uint64_t tx_errors;		/* Number of TX errors */
	uint64_t tx_jabber_timeout_errors;
					/* Number of TX jabber timeout errors */
	uint64_t tx_frame_flushed_errors;
					/* Number of TX frame flushed errors */
	uint64_t tx_loss_of_carrier_errors;
					/* Number of TX loss of carrier errors */
	uint64_t tx_no_carrier_errors;	/* Number of TX no carrier errors */
	uint64_t tx_late_collision_errors;
					/* Number of TX late collision errors */
	uint64_t tx_excessive_collision_errors;
					/* Number of TX excessive collision errors */
	uint64_t tx_excessive_deferral_errors;
					/* Number of TX excessive deferral errors */
	uint64_t tx_underflow_errors;	/* Number of TX underflow errors */
	uint64_t tx_ip_header_errors;	/* Number of TX IP header errors */
	uint64_t tx_ip_payload_errors;	/* Number of TX IP payload errors */
	uint64_t tx_dropped;		/* Number of TX dropped packets */
	uint64_t hw_errs[10];		/* GMAC DMA error counters */
	uint64_t rx_missed;		/* Number of RX packets missed by the DMA */
	uint64_t fifo_overflows;	/* Number of RX FIFO overflows signalled by the DMA */
	uint64_t gmac_total_ticks;	/* Total clock ticks spend inside the GMAC */
	uint32_t gmac_worst_case_ticks;	/* Worst case iteration of the GMAC in ticks */
	uint64_t gmac_iterations;	/* Number of iterations around the GMAC */
};

/*
 * Enumeration of the XML output.
 */
enum ubi32_na_stats_xml_states {
	UBI32_NA_STATS_XML_STATE_IPV4_START,
	UBI32_NA_STATS_XML_STATE_IPV4_STATS,
	UBI32_NA_STATS_XML_STATE_IPV4_CACHE_STATS,
	UBI32_NA_STATS_XML_STATE_IPV4_CACHE_START,
	UBI32_NA_STATS_XML_STATE_IPV4_CACHE_ENTRY,
	UBI32_NA_STATS_XML_STATE_IPV4_CACHE_END,
	UBI32_NA_STATS_XML_STATE_IPV4_END,
	UBI32_NA_STATS_XML_STATE_IPV6_START,
	UBI32_NA_STATS_XML_STATE_IPV6_STATS,
	UBI32_NA_STATS_XML_STATE_IPV6_CACHE_STATS,
	UBI32_NA_STATS_XML_STATE_IPV6_CACHE_START,
	UBI32_NA_STATS_XML_STATE_IPV6_CACHE_ENTRY,
	UBI32_NA_STATS_XML_STATE_IPV6_CACHE_END,
	UBI32_NA_STATS_XML_STATE_IPV6_END,
	UBI32_NA_STATS_XML_STATE_INTERFACES_START,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_START,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_STATS,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_HOST_STATS,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_EXCEPTION,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_IPV4_START,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_IPV4_STATS,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_IPV4_EXCEPTION,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_IPV4_END,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_IPV6_START,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_IPV6_STATS,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_IPV6_EXCEPTION,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_IPV6_END,
	UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_END,
	UBI32_NA_STATS_XML_STATE_INTERFACES_END,
	UBI32_NA_STATS_XML_STATE_PBUFS_START,
	UBI32_NA_STATS_XML_STATE_PBUFS_STATS,
	UBI32_NA_STATS_XML_STATE_PBUFS_END,
	UBI32_NA_STATS_XML_STATE_PE_START,
	UBI32_NA_STATS_XML_STATE_PE_QUEUE,
	UBI32_NA_STATS_XML_STATE_PE_TIMINGS,
	UBI32_NA_STATS_XML_STATE_PE_END,
	UBI32_NA_STATS_XML_STATE_EXCEPT_START,
	UBI32_NA_STATS_XML_STATE_EXCEPT_QUEUE,
	UBI32_NA_STATS_XML_STATE_EXCEPT_TIMINGS,
	UBI32_NA_STATS_XML_STATE_EXCEPT_END,
	UBI32_NA_STATS_XML_STATE_GMACS_START,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_START,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_STATS,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_RX_ERRORS1,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_RX_ERRORS2,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_RX_ERRORS3,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_TX_ERRORS1,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_TX_ERRORS2,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_TX_ERRORS3,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_DMA_ERRORS1,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_DMA_ERRORS2,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_QUEUE,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_TIMINGS,
	UBI32_NA_STATS_XML_STATE_GMACS_GMAC_END,
	UBI32_NA_STATS_XML_STATE_GMACS_END,
	UBI32_NA_STATS_XML_STATE_LINUX_START,
	UBI32_NA_STATS_XML_STATE_LINUX_QUEUE,
	UBI32_NA_STATS_XML_STATE_LINUX_END,
	UBI32_NA_STATS_XML_STATE_DONE
};

/*
 * XML write state.
 */
struct ubi32_na_xml_write_state {
	enum ubi32_na_stats_xml_states state;
					/* XML output file state machine state */
	int index;			/* XML output file index (sequence number preserved across reads) */
	int index2;			/* XML output file index2 (sequence number preserved across reads) */
};

/*
 * Global NA instance object.
 */
struct ubi32_na_instance {
	struct napi_struct napi;
	spinlock_t lock;
	struct na_if_map *regs;
	uint32_t flags;			/* Bit flags */
	ubicom_na_ipv4_sync_callback_t ipv4_sync;
	ubicom_na_ipv6_sync_callback_t ipv6_sync;
	uint64_t skb_alloc_err;
	uint64_t tx_q_full_cnt;
	int irq;			/* IRQ used by the NA driver */
	struct net_device *devices[NA_MAX_INTERFACES];
	bool napi_active;		/* Flag indicating if NAPI is currently active or not */
	uint32_t cache_dev_major;	/* Major number of char device */
	uint64_t last_rx_jiffies;	/* Time of the last RX message from the NA in jiffies */
	uint64_t ipv4_accelerated_rx_packets;
					/* Accelerated IPv4 RX packets */
	uint64_t ipv4_accelerated_rx_bytes;
					/* Accelerated IPv4 RX bytes */
	uint64_t ipv4_accelerated_tx_packets;
					/* Accelerated IPv4 TX packets */
	uint64_t ipv4_accelerated_tx_bytes;
					/* Accelerated IPv4 TX bytes */
	uint64_t ipv4_connection_create_requests;
					/* Number of IPv4 connection create requests */
	uint64_t ipv4_connection_create_collisions;
					/* Number of IPv4 connection create requests that collided with existing entries */
	uint64_t ipv4_connection_destroy_requests;
					/* Number of IPv4 connection destroy requests */
	uint64_t ipv4_connection_destroy_misses;
					/* Number of IPv4 connection destroy requests that missed the cache */
	uint64_t ipv4_connection_hash_hits;
					/* Number of IPv4 connection hash hits */
	uint64_t ipv4_connection_hash_reorders;
					/* Number of IPv4 connection hash reorders */
	uint64_t ipv4_connection_flushes;
					/* Number of IPv4 connection flushes */
	uint64_t ipv4_connection_evictions;
					/* Number of IPv4 connection evictions */
	uint64_t ipv6_accelerated_rx_packets;
					/* Accelerated IPv6 RX packets */
	uint64_t ipv6_accelerated_rx_bytes;
					/* Accelerated IPv6 RX bytes */
	uint64_t ipv6_accelerated_tx_packets;
					/* Accelerated IPv6 TX packets */
	uint64_t ipv6_accelerated_tx_bytes;
					/* Accelerated IPv6 TX bytes */
	uint64_t ipv6_connection_create_requests;
					/* Number of IPv6 connection create requests */
	uint64_t ipv6_connection_create_collisions;
					/* Number of IPv6 connection create requests that collided with existing entries */
	uint64_t ipv6_connection_destroy_requests;
					/* Number of IPv6 connection destroy requests */
	uint64_t ipv6_connection_destroy_misses;
					/* Number of IPv6 connection destroy requests that missed the cache */
	uint64_t ipv6_connection_hash_hits;
					/* Number of IPv6 connection hash hits */
	uint64_t ipv6_connection_hash_reorders;
					/* Number of IPv6 connection hash reorders */
	uint64_t ipv6_connection_flushes;
					/* Number of IPv6 connection flushes */
	uint64_t ipv6_connection_evictions;
					/* Number of IPv6 connection evictions */
	uint64_t pe_queue_dropped;	/* Number of packets dropped because the PE queue is too full */
	uint64_t pe_total_ticks;	/* Total clock ticks spend inside the PE */
	uint32_t pe_worst_case_ticks;	/* Worst case iteration of the PE in ticks */
	uint64_t pe_iterations;		/* Number of iterations around the PE */
	uint64_t except_queue_dropped;	/* Number of packets dropped because the exception queue is too full */
	uint64_t except_total_ticks;	/* Total clock ticks spend inside the PE */
	uint32_t except_worst_case_ticks;
					/* Worst case iteration of the exception path in ticks */
	uint64_t except_iterations;	/* Number of iterations around the PE */
	uint64_t pbuf_alloc_fails;	/* Number of pbuf allocations that have failed */
	uint64_t pbuf_payload_alloc_fails;
					/* Number of pbuf allocations that have failed because there were no free payloads */
	struct na_gmac_statistics na_gmac_statistics[NA_MAX_INTERFACES];
	struct na_ipv4_statistics na_ipv4_statistics[IPV4_CONNECTION_ENTRIES];
	struct na_ipv6_statistics na_ipv6_statistics[IPV6_CONNECTION_ENTRIES];
};

#define UBI32_NA_INSTANCE_FLAGS_TX_STOPPED 0x00000001
					/* Transmit queue was full so we stopped sending */

static struct ubi32_na_instance ubi32_na;

static u8_t na_mac_addr[NA_MAX_INTERFACES][ETH_ALEN] = {
	{0x00, 0x03, 0x64, 0xf3, 0x6c, 0x09},
	{0x00, 0x03, 0x64, 0xf3, 0x6c, 0x0a}
};

#define UBICOM_NA_NETDEV_MAGIC 0x1FEF5511
					/* Special magic used to identify the private area of a net dev as being the NA */

/*
 * SysFS class of the ubi32_na driver
 * SysFS control points can be found at /sys/devices/system/ubi32_na/ubi32_nax/...
 */
static struct sysdev_class ubi32_na_sysclass = {
	.name = "ubi32_na",
};

/*
 * SysFS linkage
 */
static struct sys_device ubi32_na_sys_dev;

typedef bool (*ubi32_na_stats_xml_write_method_t)(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws);

/*
 * ubi32_na_write_allocate_buffer()
 *	Write-allocate a buffer (load it into the cache without taking cache misses).
 */
static inline void ubi32_na_write_allocate_buffer(void *buf, size_t sz)
{
	/*
	 * This code expects to be inlined and subsequently to have the compiler
	 * eliminate all of the runtime size checks.  In general it will be passed
	 * a constant size object which makes this easy to achieve.
	 */
	if (sz > 0) {
		asm volatile (
		"	movea	0(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 32) {
		asm volatile (
		"	movea	32(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 64) {
		asm volatile (
		"	movea	64(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 96) {
		asm volatile (
		"	movea	96(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 128) {
		asm volatile (
		"	movea	128(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 160) {
		asm volatile (
		"	movea	160(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 192) {
		asm volatile (
		"	movea	192(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 224) {
		asm volatile (
		"	movea	224(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 256) {
		asm volatile (
		"	movea	256(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 288) {
		asm volatile (
		"	movea	288(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 320) {
		asm volatile (
		"	movea	320(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 352) {
		asm volatile (
		"	movea	352(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 384) {
		asm volatile (
		"	movea	384(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 416) {
		asm volatile (
		"	movea	416(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 448) {
		asm volatile (
		"	movea	448(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}

	if (sz > 480) {
		asm volatile (
		"	movea	480(%[buf]), #0	\n\t"
			:
			: [buf] "a" (buf)
		);
	}
}
/*
 * ubi32_na_interrupt_na()
 *	Interrupt the NA.
 */
static inline void ubi32_na_interrupt_na(struct na_if_map *nim)
{
	ubicom32_set_interrupt_int_set0_with_mask(nim->na_int_bit);
}

static void ubi32_na_vp_rxtx_start(struct net_device *dev)
{
	struct na_if_map *nim = ubi32_na.regs;

	spin_lock_bh(&ubi32_na.lock);
#if 0
	priv->regs->command = UBI32_NA_VP_CMD_RX_ENABLE | UBI32_NA_VP_CMD_TX_ENABLE;
#endif
	nim->int_enable = NA_IF_MAP_INT_ENABLE_RX | NA_IF_MAP_INT_ENABLE_POP |
				NA_IF_MAP_INT_ENABLE_PUSH | NA_IF_MAP_INT_ENABLE_TX;
	ubi32_na_interrupt_na(nim);
	spin_unlock_bh(&ubi32_na.lock);
}

/*
 * ubi32_na_vp_rxtx_stop()
 */
static void ubi32_na_vp_rxtx_stop(struct net_device *dev)
{
	struct na_if_map *nim = ubi32_na.regs;

	spin_lock_bh(&ubi32_na.lock);
#if 0
	nim->command = 0;
#endif
	nim->int_enable = 0;
	ubi32_na_interrupt_na(nim);
	spin_unlock_bh(&ubi32_na.lock);

#if 0
	/* Wait for graceful shutdown */
	while (ubi32_na.regs->status & (UBI32_NA_VP_STATUS_RX_STATE | UBI32_NA_VP_STATUS_TX_STATE));
#endif
}

/*
 * ubi32_na_push()
 *	Push buffers to the NA.
 */
static void ubi32_na_push(struct na_if_map *nim)
{
	uint32_t push_count = NA_NUM_PUSH_DESCRIPTORS;

	/*
	 * Clear down any "push" interrupt flag.
	 */
	nim->int_status &= ~NA_IF_MAP_INT_STATUS_PUSH;

	/*
	 * Push a block of new buffers to the NA.
	 */
	while (push_count--) {
		uint32_t i = nim->push_host_index;
		uint32_t ni = (i + 1) & (NA_NUM_PUSH_DESCRIPTORS - 1);
		struct na_desc *pushdesc;
		struct sk_buff *skb;

		/*
		 * Are we able to write any more push descriptors?
		 */
		if (unlikely(ni == nim->push_na_index)) {
			break;
		}

		/*
		 * Get the next descriptor and allocate a buffer to write into it.
		 */
		pushdesc = &nim->push_desc[i];
		skb = __dev_alloc_skb(NA_SKB_PAYLOAD_SIZE, GFP_ATOMIC | __GFP_NOWARN);
		if (unlikely(!skb)) {
			spin_lock_bh(&ubi32_na.lock);
			ubi32_na.skb_alloc_err++;
			spin_unlock_bh(&ubi32_na.lock);
			break;
		}

		/*
		 * Fill in the descriptor details.
		 */
		pushdesc->opaque = (uint32_t)skb;
		pushdesc->buffer = virt_to_phys(skb->head);

		/*
		 * We are about to hand the new descriptor entry to the NA so
		 * we must not use pushdesc from here onwards.  We set it to NULL, not
		 * because it does anything useful (and in fact for code that doesn't
		 * try to use pushdesc again the compiler will eliminate this setting
		 * operation as "unused" in optimized builds) but if anyone adds a bad
		 * patch in the future that attempted to use pushdesc it will trigger
		 * a segfault :-)
		 */
		pushdesc = NULL;

		/*
		 * Update our host index so the NA sees we've written a new descriptor.
		 */
		nim->push_host_index = ni;

		/*
		 * Kick the NA awake so it can process our new entry.
		 */
		ubi32_na_interrupt_na(nim);
	}
}

/*
 * ubi32_na_pop()
 *	Pop buffers from the NA.
 */
static void ubi32_na_pop(struct na_if_map *nim)
{
	/*
	 * Clear down any "pop" interrupt flag.
	 */
	nim->int_status &= ~NA_IF_MAP_INT_STATUS_POP;

	/*
	 * Pop a block of unused buffers from the NA.
	 */
	while (1) {
		uint32_t i = nim->pop_host_index;
		uint32_t ni;
		struct na_desc *popdesc;
		struct sk_buff *skb;

		/*
		 * Any more buffers to retrieve?
		 */
		if (i == nim->pop_na_index) {
			break;
		}

		/*
		 * Get the descriptor entry and the buffer that we have been
		 * handed by the NA.
		 */
		popdesc = &nim->pop_desc[i];
		skb = (struct sk_buff *)popdesc->opaque;

		/*
		 * We are about to hand back the descriptor entry to the NA so
		 * we must not use popdesc from here onwards.  We set it to NULL, not
		 * because it does anything useful (and in fact for code that doesn't
		 * try to use popdesc again the compiler will eliminate this setting
		 * operation as "unused" in optimized builds) but if anyone adds a bad
		 * patch in the future that attempted to use popdesc it will trigger
		 * a segfault :-)
		 */
		popdesc = NULL;

		/*
		 * Update our host index so the NA can write any more buffers
		 * it may have.
		 */
		ni = (i + 1) & (NA_NUM_POP_DESCRIPTORS - 1);
		nim->pop_host_index = ni;

		/*
		 * Kick the NA awake (in case it was waiting for space to
		 * write more pop descriptors).
		 */
		ubi32_na_interrupt_na(nim);

		/*
		 * Finally free the SKB we were handed.
		 */
		dev_kfree_skb_any(skb);
	}
}

/*
 * ubi32_na_rx_metadata_ipv4_rule_establish()
 *	Handle the establishment of an IPv4 rule.
 */
static void ubi32_na_rx_metadata_ipv4_rule_establish(struct na_ipv4_rule_establish *nire)
{
	struct na_ipv4_statistics *nis;

	if (unlikely(nire->index >= IPV4_CONNECTION_ENTRIES)) {
		na_warning("Bad index: %d\n", nire->index);
		return;
	}

	nis = &ubi32_na.na_ipv4_statistics[nire->index];

	spin_lock(&ubi32_na.lock);
	nis->protocol = nire->protocol;
	nis->flow_interface = nire->flow_interface;
	nis->flow_ip = nire->flow_ip;
	nis->flow_ip_xlate = nire->flow_ip_xlate;
	nis->flow_ident = nire->flow_ident;
	nis->flow_ident_xlate = nire->flow_ident_xlate;
	nis->flow_accelerated_rx_packets = 0;
	nis->flow_accelerated_rx_bytes = 0;
	nis->flow_accelerated_tx_packets = 0;
	nis->flow_accelerated_tx_bytes = 0;
	nis->return_interface = nire->return_interface;
	nis->return_ip = nire->return_ip;
	nis->return_ip_xlate = nire->return_ip_xlate;
	nis->return_ident = nire->return_ident;
	nis->return_ident_xlate = nire->return_ident_xlate;
	nis->return_accelerated_rx_packets = 0;
	nis->return_accelerated_rx_bytes = 0;
	nis->return_accelerated_tx_packets = 0;
	nis->return_accelerated_tx_bytes = 0;
	nis->last_sync = ubi32_na.last_rx_jiffies;
	spin_unlock(&ubi32_na.lock);
}

/*
 * ubi32_na_rx_metadata_ipv4_rule_sync()
 *	Handle the syncing of an IPv4 rule.
 */
static void ubi32_na_rx_metadata_ipv4_rule_sync(struct na_ipv4_rule_sync *nirs)
{
	struct ubicom_na_ipv4_sync unis;
	struct net_device *flow_dev;
	struct net_device *return_dev;
	struct neighbour *neigh;
	struct ubi32_na_private *flow_priv;
	struct ubi32_na_private *return_priv;
	struct na_ipv4_statistics *nis;
	ubicom_na_ipv4_sync_callback_t ipv4_sync;

	if (unlikely(nirs->index >= IPV4_CONNECTION_ENTRIES)) {
		na_warning("Bad index: %d\n", nirs->index);
		return;
	}

	nis = &ubi32_na.na_ipv4_statistics[nirs->index];

	spin_lock(&ubi32_na.lock);
	ipv4_sync = ubi32_na.ipv4_sync;
	spin_unlock(&ubi32_na.lock);

	/*
	 * If we have a registered sync callback then we want to invoke it unless we're
	 * syncing because of a destroy request.  If we saw a destroy request it means that
	 * we already tore down the connection state and there's nothing to synchronize
	 */
	if (likely(ipv4_sync) && unlikely(nirs->reason != NA_IPV4_RULE_SYNC_REASON_DESTROY)) {
		unis.protocol = nis->protocol;
		unis.src_addr = nis->flow_ip;
		unis.src_addr_xlate = nis->flow_ip_xlate;
		unis.src_port = nis->flow_ident;
		unis.src_port_xlate = nis->flow_ident_xlate;
		unis.flow_max_window = nirs->flow_max_window;
		unis.flow_end = nirs->flow_end;
		unis.flow_max_end = nirs->flow_max_end;
		unis.flow_packet_count = nirs->flow_rx_packet_count;
		unis.flow_byte_count = nirs->flow_rx_byte_count;
		unis.dest_addr = nis->return_ip;
		unis.dest_port = nis->return_ident;
		unis.dest_addr_xlate = nis->return_ip_xlate;
		unis.dest_port_xlate = nis->return_ident_xlate;
		unis.return_max_window = nirs->return_max_window;
		unis.return_end = nirs->return_end;
		unis.return_max_end = nirs->return_max_end;
		unis.return_packet_count = nirs->return_rx_packet_count;
		unis.return_byte_count = nirs->return_rx_byte_count;
		unis.final_sync = (nirs->reason == NA_IPV4_RULE_SYNC_REASON_FLUSH) ? 1 : 0;

		/*
		 * Convert ms ticks from the NA to jiffies.  We know that inc_ticks is small
		 * and we expect HZ to be small too so we can multiply without worrying about
		 * wrap-around problems.  We add a rounding constant to ensure that the different
		 * time bases don't cause truncation errors.
		 */
		BUG_ON(HZ > 100000);
		unis.delta_jiffies = ((nirs->inc_ticks * HZ) + (MSEC_PER_SEC / 2)) / MSEC_PER_SEC;

		ipv4_sync(&unis);
	}

	spin_lock(&ubi32_na.lock);

	/*
	 * If we are being told that this entry has been finalized then mark it as not
	 * in use.
	 */
	if (nirs->reason != NA_IPV4_RULE_SYNC_REASON_STATS) {
		nis->last_sync = 0;
	} else {
		nis->flow_accelerated_rx_packets += nirs->flow_rx_packet_count;
		nis->flow_accelerated_rx_bytes += nirs->flow_rx_byte_count;
		nis->flow_accelerated_tx_packets += nirs->flow_tx_packet_count;
		nis->flow_accelerated_tx_bytes += nirs->flow_tx_byte_count;
		nis->return_accelerated_rx_packets += nirs->return_rx_packet_count;
		nis->return_accelerated_rx_bytes += nirs->return_rx_byte_count;
		nis->return_accelerated_tx_packets += nirs->return_tx_packet_count;
		nis->return_accelerated_tx_bytes += nirs->return_tx_byte_count;
		nis->last_sync = ubi32_na.last_rx_jiffies;
	}

	ubi32_na.ipv4_accelerated_rx_packets += nirs->flow_rx_packet_count + nirs->return_rx_packet_count;
	ubi32_na.ipv4_accelerated_rx_bytes += nirs->flow_rx_byte_count + nirs->return_rx_byte_count;
	ubi32_na.ipv4_accelerated_tx_packets += nirs->flow_tx_packet_count + nirs->return_tx_packet_count;
	ubi32_na.ipv4_accelerated_tx_bytes += nirs->flow_tx_byte_count + nirs->return_tx_packet_count;

	flow_dev = ubi32_na.devices[nis->flow_interface];
	flow_priv = netdev_priv(flow_dev);
	flow_priv->ipv4_accelerated_rx_packets += nirs->flow_rx_packet_count;
	flow_priv->ipv4_accelerated_rx_bytes += nirs->flow_rx_byte_count;
	flow_priv->ipv4_accelerated_tx_packets += nirs->flow_tx_packet_count;
	flow_priv->ipv4_accelerated_tx_bytes += nirs->flow_tx_byte_count;

	return_dev = ubi32_na.devices[nis->return_interface];
	return_priv = netdev_priv(return_dev);
	return_priv->ipv4_accelerated_rx_packets += nirs->return_rx_packet_count;
	return_priv->ipv4_accelerated_rx_bytes += nirs->return_rx_byte_count;
	return_priv->ipv4_accelerated_tx_packets += nirs->return_tx_packet_count;
	return_priv->ipv4_accelerated_tx_bytes += nirs->return_tx_byte_count;

	spin_unlock(&ubi32_na.lock);

	/*
	 * Update ARP table.
	 */
	if (likely(flow_dev)) {
		neigh = neigh_lookup(&arp_tbl, &nis->flow_ip, flow_dev);
		if (neigh) {
			neigh_update(neigh, NULL, neigh->nud_state, NEIGH_UPDATE_F_WEAK_OVERRIDE);
			neigh_release(neigh);
		}
	}

	if (likely(return_dev)) {
		neigh = neigh_lookup(&arp_tbl, &nis->return_ip, return_dev);
		if (neigh) {
			neigh_update(neigh, NULL, neigh->nud_state, NEIGH_UPDATE_F_WEAK_OVERRIDE);
			neigh_release(neigh);
		}
	}
}

/*
 * ubi32_na_rx_metadata_ipv6_rule_establish()
 *	Handle the establishment of an IPv6 rule.
 */
static void ubi32_na_rx_metadata_ipv6_rule_establish(struct na_ipv6_rule_establish *nire)
{
	struct na_ipv6_statistics *nis;

	if (unlikely(nire->index >= IPV6_CONNECTION_ENTRIES)) {
		na_warning("Bad index: %d\n", nire->index);
		return;
	}

	nis = &ubi32_na.na_ipv6_statistics[nire->index];

	spin_lock(&ubi32_na.lock);
	nis->protocol = nire->protocol;
	nis->flow_interface = nire->flow_interface;
	nis->flow_ip[0] = nire->flow_ip[0];
	nis->flow_ip[1] = nire->flow_ip[1];
	nis->flow_ip[2] = nire->flow_ip[2];
	nis->flow_ip[3] = nire->flow_ip[3];
	nis->flow_ident = nire->flow_ident;
	nis->flow_accelerated_rx_packets = 0;
	nis->flow_accelerated_rx_bytes = 0;
	nis->flow_accelerated_tx_packets = 0;
	nis->flow_accelerated_tx_bytes = 0;
	nis->return_interface = nire->return_interface;
	nis->return_ip[0] = nire->return_ip[0];
	nis->return_ip[1] = nire->return_ip[1];
	nis->return_ip[2] = nire->return_ip[2];
	nis->return_ip[3] = nire->return_ip[3];
	nis->return_ident = nire->return_ident;
	nis->return_accelerated_rx_packets = 0;
	nis->return_accelerated_rx_bytes = 0;
	nis->return_accelerated_tx_packets = 0;
	nis->return_accelerated_tx_bytes = 0;
	nis->last_sync = ubi32_na.last_rx_jiffies;
	spin_unlock(&ubi32_na.lock);
}

/*
 * ubi32_na_rx_metadata_ipv6_rule_sync()
 *	Handle the syncing of an IPv6 rule.
 */
static void ubi32_na_rx_metadata_ipv6_rule_sync(struct na_ipv6_rule_sync *nirs)
{
	struct ubicom_na_ipv6_sync unis;
	struct net_device *flow_dev;
	struct net_device *return_dev;
	struct neighbour *neigh;
	struct ubi32_na_private *flow_priv;
	struct ubi32_na_private *return_priv;
	struct na_ipv6_statistics *nis;
	ubicom_na_ipv6_sync_callback_t ipv6_sync;

	if (unlikely(nirs->index >= IPV6_CONNECTION_ENTRIES)) {
		na_warning("Bad index: %d\n", nirs->index);
		return;
	}

	nis = &ubi32_na.na_ipv6_statistics[nirs->index];

	spin_lock(&ubi32_na.lock);
	ipv6_sync = ubi32_na.ipv6_sync;
	spin_unlock(&ubi32_na.lock);

	/*
	 * If we have a registered sync callback then we want to invoke it unless we're
	 * syncing because of a destroy request.  If we saw a destroy request it means that
	 * we already tore down the connection state and there's nothing to synchronize
	 */
	if (likely(ipv6_sync) && unlikely(nirs->reason != NA_IPV6_RULE_SYNC_REASON_DESTROY)) {
		unis.protocol = nis->protocol;
		unis.src_addr[0] = nis->flow_ip[0];
		unis.src_addr[1] = nis->flow_ip[1];
		unis.src_addr[2] = nis->flow_ip[2];
		unis.src_addr[3] = nis->flow_ip[3];
		unis.src_port = nis->flow_ident;
		unis.flow_max_window = nirs->flow_max_window;
		unis.flow_end = nirs->flow_end;
		unis.flow_max_end = nirs->flow_max_end;
		unis.flow_packet_count = nirs->flow_rx_packet_count;
		unis.flow_byte_count = nirs->flow_rx_byte_count;
		unis.dest_addr[0] = nis->return_ip[0];
		unis.dest_addr[1] = nis->return_ip[1];
		unis.dest_addr[2] = nis->return_ip[2];
		unis.dest_addr[3] = nis->return_ip[3];
		unis.dest_port = nis->return_ident;
		unis.return_max_window = nirs->return_max_window;
		unis.return_end = nirs->return_end;
		unis.return_max_end = nirs->return_max_end;
		unis.return_packet_count = nirs->return_rx_packet_count;
		unis.return_byte_count = nirs->return_rx_byte_count;
		unis.final_sync = (nirs->reason == NA_IPV6_RULE_SYNC_REASON_FLUSH) ? 1 : 0;

		/*
		 * Convert ms ticks from the NA to jiffies.  We know that inc_ticks is small
		 * and we expect HZ to be small too so we can multiply without worrying about
		 * wrap-around problems.  We add a rounding constant to ensure that the different
		 * time bases don't cause truncation errors.
		 */
		BUG_ON(HZ > 100000);
		unis.delta_jiffies = ((nirs->inc_ticks * HZ) + (MSEC_PER_SEC / 2)) / MSEC_PER_SEC;

		ipv6_sync(&unis);
	}

	spin_lock(&ubi32_na.lock);

	/*
	 * If we are being told that this entry has been finalized then mark it as not
	 * in use.
	 */
	if (nirs->reason != NA_IPV6_RULE_SYNC_REASON_STATS) {
		nis->last_sync = 0;
	} else {
		nis->flow_accelerated_rx_packets += nirs->flow_rx_packet_count;
		nis->flow_accelerated_rx_bytes += nirs->flow_rx_byte_count;
		nis->flow_accelerated_tx_packets += nirs->flow_tx_packet_count;
		nis->flow_accelerated_tx_bytes += nirs->flow_tx_byte_count;
		nis->return_accelerated_rx_packets += nirs->return_rx_packet_count;
		nis->return_accelerated_rx_bytes += nirs->return_rx_byte_count;
		nis->return_accelerated_tx_packets += nirs->return_tx_packet_count;
		nis->return_accelerated_tx_bytes += nirs->return_tx_byte_count;
		nis->last_sync = ubi32_na.last_rx_jiffies;
	}

	ubi32_na.ipv6_accelerated_rx_packets += nirs->flow_rx_packet_count + nirs->return_rx_packet_count;
	ubi32_na.ipv6_accelerated_rx_bytes += nirs->flow_rx_byte_count + nirs->return_rx_byte_count;
	ubi32_na.ipv6_accelerated_tx_packets += nirs->flow_tx_packet_count + nirs->return_tx_packet_count;
	ubi32_na.ipv6_accelerated_tx_bytes += nirs->flow_tx_byte_count + nirs->return_tx_packet_count;

	flow_dev = ubi32_na.devices[nis->flow_interface];
	flow_priv = netdev_priv(flow_dev);
	flow_priv->ipv6_accelerated_rx_packets += nirs->flow_rx_packet_count;
	flow_priv->ipv6_accelerated_rx_bytes += nirs->flow_rx_byte_count;
	flow_priv->ipv6_accelerated_tx_packets += nirs->flow_tx_packet_count;
	flow_priv->ipv6_accelerated_tx_bytes += nirs->flow_tx_byte_count;

	return_dev = ubi32_na.devices[nis->return_interface];
	return_priv = netdev_priv(return_dev);
	return_priv->ipv6_accelerated_rx_packets += nirs->return_rx_packet_count;
	return_priv->ipv6_accelerated_rx_bytes += nirs->return_rx_byte_count;
	return_priv->ipv6_accelerated_tx_packets += nirs->return_tx_packet_count;
	return_priv->ipv6_accelerated_tx_bytes += nirs->return_tx_byte_count;

	spin_unlock(&ubi32_na.lock);

	/*
	 * Update ndisc table
	 */
#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
	if (likely(flow_dev)) {
		neigh = neigh_lookup(&nd_tbl, &nis->flow_ip, flow_dev);
		if (likely(neigh)) {
			neigh_update(neigh, NULL, neigh->nud_state, NEIGH_UPDATE_F_WEAK_OVERRIDE);
			neigh_release(neigh);
		}
	}

	if (likely(return_dev)) {
		neigh = neigh_lookup(&nd_tbl, &nis->return_ip, return_dev);
		if (likely(neigh)) {
			neigh_update(neigh, NULL, neigh->nud_state, NEIGH_UPDATE_F_WEAK_OVERRIDE);
			neigh_release(neigh);
		}
	}
#endif
}

/*
 * ubi32_na_rx_metadata_gmac_stats_sync()
 *	Handle the syncing of GMAC stats.
 */
static void ubi32_na_rx_metadata_gmac_stats_sync(struct na_gmac_stats_sync *ngss)
{
	int j;
	struct na_gmac_statistics *ngs = &ubi32_na.na_gmac_statistics[ngss->interface];

	spin_lock(&ubi32_na.lock);
	ngs->rx_bytes += ngss->rx_bytes;
	ngs->rx_packets += ngss->rx_packets;
	ngs->rx_errors += ngss->rx_errors;
	ngs->rx_receive_errors += ngss->rx_receive_errors;
	ngs->rx_overflow_errors += ngss->rx_overflow_errors;
	ngs->rx_descriptor_errors += ngss->rx_descriptor_errors;
	ngs->rx_watchdog_timeout_errors += ngss->rx_watchdog_timeout_errors;
	ngs->rx_crc_errors += ngss->rx_crc_errors;
	ngs->rx_late_collision_errors += ngss->rx_late_collision_errors;
	ngs->rx_dribble_bit_errors += ngss->rx_dribble_bit_errors;
	ngs->rx_length_errors += ngss->rx_length_errors;
	ngs->rx_ip_header_errors += ngss->rx_ip_header_errors;
	ngs->rx_ip_payload_errors += ngss->rx_ip_payload_errors;
	ngs->rx_no_buffer_errors += ngss->rx_no_buffer_errors;
	ngs->rx_transport_csum_bypassed += ngss->rx_transport_csum_bypassed;
	ngs->tx_bytes += ngss->tx_bytes;
	ngs->tx_packets += ngss->tx_packets;
	ngs->tx_collisions += ngss->tx_collisions;
	ngs->tx_errors += ngss->tx_errors;
	ngs->tx_jabber_timeout_errors += ngss->tx_jabber_timeout_errors;
	ngs->tx_frame_flushed_errors += ngss->tx_frame_flushed_errors;
	ngs->tx_loss_of_carrier_errors += ngss->tx_loss_of_carrier_errors;
	ngs->tx_no_carrier_errors += ngss->tx_no_carrier_errors;
	ngs->tx_late_collision_errors += ngss->tx_late_collision_errors;
	ngs->tx_excessive_collision_errors += ngss->tx_excessive_collision_errors;
	ngs->tx_excessive_deferral_errors += ngss->tx_excessive_deferral_errors;
	ngs->tx_underflow_errors += ngss->tx_underflow_errors;
	ngs->tx_ip_header_errors += ngss->tx_ip_header_errors;
	ngs->tx_ip_payload_errors += ngss->tx_ip_payload_errors;
	ngs->tx_dropped += ngss->tx_dropped;
	for (j = 0; j < 10; j++) {
		ngs->hw_errs[j] += ngss->hw_errs[j];
	}

	ngs->rx_missed += ngss->rx_missed;
	ngs->fifo_overflows += ngss->fifo_overflows;
	ngs->gmac_total_ticks += ngss->gmac_total_ticks;
	if (unlikely(ngs->gmac_worst_case_ticks < ngss->gmac_worst_case_ticks)) {
		ngs->gmac_worst_case_ticks = ngss->gmac_worst_case_ticks;
	}

	ngs->gmac_iterations += ngss->gmac_iterations;
	spin_unlock(&ubi32_na.lock);
}

/*
 * ubi32_na_rx_metadata_interface_stats_sync()
 *	Handle the syncing of interface statistics.
 */
static void ubi32_na_rx_metadata_interface_stats_sync(struct na_interface_stats_sync *niss)
{
	struct net_device *dev;
	struct ubi32_na_private *priv;
	int i;

	spin_lock(&ubi32_na.lock);
	dev = ubi32_na.devices[niss->interface];
	if (unlikely(!dev)) {
		spin_unlock(&ubi32_na.lock);
		return;
	}

	priv = netdev_priv(dev);
	priv->stats64.rx_packets += niss->rx_packets;
	priv->stats64.tx_packets += niss->tx_packets;
	priv->stats64.rx_bytes += niss->rx_bytes;
	priv->stats64.tx_bytes += niss->tx_bytes;
	priv->stats64.rx_errors += niss->rx_errors;
	priv->stats64.tx_errors += niss->tx_errors;
	priv->stats64.tx_dropped += niss->tx_dropped;
	priv->stats64.collisions += niss->collisions;
	priv->host_rx_packets += niss->host_rx_packets;
	priv->host_rx_bytes += niss->host_rx_bytes;
	priv->host_tx_packets += niss->host_tx_packets;
	priv->host_tx_bytes += niss->host_tx_bytes;
	priv->stats64.rx_length_errors += niss->rx_length_errors;
	priv->stats64.rx_over_errors += niss->rx_overflow_errors;
	priv->stats64.rx_crc_errors += niss->rx_crc_errors;

	for (i = 0; i < NA_EXCEPTION_EVENT_UNKNOWN_LAST; i++) {
		priv->exception_events_unknown[i] += niss->exception_events_unknown[i];
	}

	for (i = 0; i < NA_EXCEPTION_EVENT_IPV4_LAST; i++) {
		priv->exception_events_ipv4[i] += niss->exception_events_ipv4[i];
	}

	for (i = 0; i < NA_EXCEPTION_EVENT_IPV6_LAST; i++) {
		priv->exception_events_ipv6[i] += niss->exception_events_ipv6[i];
	}

	spin_unlock(&ubi32_na.lock);

	/*
	 * If interface is bridged, update brige stats
	 */
	if (br_port_exists(dev)) {
		struct net_bridge_port *p;
		rcu_read_lock();
		p = br_port_get_rcu(dev);
		if (p) {
			struct net_bridge *br = p->br;
			if (br) {
				struct br_cpu_netstats *brstats = this_cpu_ptr(br->stats);
				u64_stats_update_begin(&brstats->syncp);
				brstats->rx_packets += niss->rx_packets;
				brstats->tx_packets += niss->tx_packets;
				brstats->rx_bytes += niss->rx_bytes;
				brstats->tx_bytes += niss->tx_bytes;
				u64_stats_update_end(&brstats->syncp);
			}
		}
		rcu_read_unlock();
	}
}

/*
 * ubi32_na_rx_metadata_na_stats_sync()
 *	Handle the syncing of NA statistics.
 */
static void ubi32_na_rx_metadata_na_stats_sync(struct na_na_stats_sync *nnss)
{
	spin_lock(&ubi32_na.lock);
	ubi32_na.ipv4_connection_create_requests += nnss->ipv4_connection_create_requests;
	ubi32_na.ipv4_connection_create_collisions += nnss->ipv4_connection_create_collisions;
	ubi32_na.ipv4_connection_destroy_requests += nnss->ipv4_connection_destroy_requests;
	ubi32_na.ipv4_connection_destroy_misses += nnss->ipv4_connection_destroy_misses;
	ubi32_na.ipv4_connection_hash_hits += nnss->ipv4_connection_hash_hits;
	ubi32_na.ipv4_connection_hash_reorders += nnss->ipv4_connection_hash_reorders;
	ubi32_na.ipv4_connection_flushes += nnss->ipv4_connection_flushes;
	ubi32_na.ipv4_connection_evictions += nnss->ipv4_connection_evictions;
	ubi32_na.ipv6_connection_create_requests += nnss->ipv6_connection_create_requests;
	ubi32_na.ipv6_connection_create_collisions += nnss->ipv6_connection_create_collisions;
	ubi32_na.ipv6_connection_destroy_requests += nnss->ipv6_connection_destroy_requests;
	ubi32_na.ipv6_connection_destroy_misses += nnss->ipv6_connection_destroy_misses;
	ubi32_na.ipv6_connection_hash_hits += nnss->ipv6_connection_hash_hits;
	ubi32_na.ipv6_connection_hash_reorders += nnss->ipv6_connection_hash_reorders;
	ubi32_na.ipv6_connection_flushes += nnss->ipv6_connection_flushes;
	ubi32_na.ipv6_connection_evictions += nnss->ipv6_connection_evictions;
	ubi32_na.pe_queue_dropped += nnss->pe_queue_dropped;
	ubi32_na.pe_total_ticks += nnss->pe_total_ticks;
	if (unlikely(ubi32_na.pe_worst_case_ticks < nnss->pe_worst_case_ticks)) {
		ubi32_na.pe_worst_case_ticks = nnss->pe_worst_case_ticks;
	}

	ubi32_na.pe_iterations += nnss->pe_iterations;
	ubi32_na.except_queue_dropped += nnss->except_queue_dropped;
	ubi32_na.except_total_ticks += nnss->except_total_ticks;
	if (unlikely(ubi32_na.except_worst_case_ticks < nnss->except_worst_case_ticks)) {
		ubi32_na.except_worst_case_ticks = nnss->except_worst_case_ticks;
	}

	ubi32_na.except_iterations += nnss->except_iterations;
	ubi32_na.pbuf_alloc_fails += nnss->pbuf_alloc_fails;
	ubi32_na.pbuf_payload_alloc_fails += nnss->pbuf_payload_alloc_fails;

	spin_unlock(&ubi32_na.lock);
}

/*
 * ubi32_na_rx_metadata()
 *	Handle metadata packets received from the NA.
 */
static void ubi32_na_rx_metadata(struct sk_buff *skb)
{
	uint32_t data_addr = (uint32_t)skb->data;
	struct na_rx_metadata_object *nrmo = (struct na_rx_metadata_object *)data_addr;

	if (unlikely(skb->len < sizeof(struct na_rx_metadata_object))) {
		na_warning("payload: %d bytes, struct: %ld bytes",
			skb->len, sizeof(struct na_rx_metadata_object));
		return;
	}

	switch (nrmo->type) {
	case NA_RX_METADATA_TYPE_IPV4_RULE_ESTABLISH:
		ubi32_na_rx_metadata_ipv4_rule_establish(&nrmo->sub.ipv4_rule_establish);
		break;

	case NA_RX_METADATA_TYPE_IPV4_RULE_SYNC:
		ubi32_na_rx_metadata_ipv4_rule_sync(&nrmo->sub.ipv4_rule_sync);
		break;

	case NA_RX_METADATA_TYPE_IPV6_RULE_ESTABLISH:
		ubi32_na_rx_metadata_ipv6_rule_establish(&nrmo->sub.ipv6_rule_establish);
		break;

	case NA_RX_METADATA_TYPE_IPV6_RULE_SYNC:
		ubi32_na_rx_metadata_ipv6_rule_sync(&nrmo->sub.ipv6_rule_sync);
		break;

	case NA_RX_METADATA_TYPE_GMAC_STATS_SYNC:
		ubi32_na_rx_metadata_gmac_stats_sync(&nrmo->sub.gmac_stats_sync);
		break;

	case NA_RX_METADATA_TYPE_INTERFACE_STATS_SYNC:
		ubi32_na_rx_metadata_interface_stats_sync(&nrmo->sub.interface_stats_sync);
		break;

	case NA_RX_METADATA_TYPE_NA_STATS_SYNC:
		ubi32_na_rx_metadata_na_stats_sync(&nrmo->sub.na_stats_sync);
		break;

	default:
		na_warning("Unknown NRMO: %d", nrmo->type);
	}
}

/*
 * ubi32_na_rx()
 *	return number of frames processed
 */
static int ubi32_na_rx(struct na_if_map *nim, int quota)
{
	int count = 0;

	/*
	 * Clear down any RX interrupt flag.
	 */
	nim->int_status &= ~NA_IF_MAP_INT_STATUS_RX;

	/*
	 * While potentially slightly inaccurate we capture the current jiffies
	 * count here and use this for timestamps.  This saves a lot of unecessary
	 * fetches of the actual jiffies count.
	 */
	ubi32_na.last_rx_jiffies = get_jiffies_64();

	/*
	 * Receive one or more RX packets from the NA.
	 */
	while (count < quota) {
		uint32_t i = nim->rx_host_index;
		uint32_t ni;
		struct na_desc *rxdesc;
		struct sk_buff *skb;
		struct net_device *dev;
		uint32_t status;

		if (i == nim->rx_na_index) {
			break;
		}

		rxdesc = &nim->rx_desc[i];

		dev = ubi32_na.devices[rxdesc->interface_num];

		status = rxdesc->status;
		skb = (struct sk_buff *)rxdesc->opaque;
		skb->data = skb->head + rxdesc->payload_offs;
		skb->len = rxdesc->payload_len;
		skb->tail = skb->data + skb->len;

		/*
		 * We are about to hand back the descriptor entry to the NA so
		 * we must not use rxdesc from here onwards.  We set it to NULL, not
		 * because it does anything useful (and in fact for code that doesn't
		 * try to use rxdesc again the compiler will eliminate this setting
		 * operation as "unused" in optimized builds) but if anyone adds a bad
		 * patch in the future that attempted to use rxdesc it will trigger
		 * a segfault :-)
		 */
		rxdesc = NULL;

		/*
		 * Update our host index so the NA can write any more buffers
		 * it may have.
		 */
		ni = (i + 1) & (NA_NUM_RX_DESCRIPTORS - 1);
		nim->rx_host_index = ni;

		/*
		 * Kick the NA awake (in case it was waiting for space to
		 * write more RX descriptors).
		 */
		ubi32_na_interrupt_na(nim);

		/*
		 * Is this packet data or metadata?
		 */
		if (unlikely(!(status & NA_DESC_STATUS_PKT))) {
			ubi32_na_rx_metadata(skb);
			dev_kfree_skb_any(skb);
		} else {
			struct ubi32_na_private *priv __attribute__((unused));

			skb->dev = dev;

			/*
			 * By default we'll assume that the accelerator performed transport
			 * layer checksum verification but if it didn't we'll notify Linux
			 * that none happened and Linux can deal with this as it wants.
			 */
			skb->ip_summed = CHECKSUM_UNNECESSARY;
			if (unlikely(!(status & NA_DESC_STATUS_IP_TRANSPORT_CHECKSUM_VALID))) {
				skb->ip_summed = CHECKSUM_NONE;
			}

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
			/*
			 * If we're configured for VLAN processing then we need
			 * to strip the VLAN details and handle the packet via
			 * a VLAN receive function.
			 */
			priv = netdev_priv(dev);
			if (priv->vlgrp && skb->data[12] == 0x81 && skb->data[13] == 0x00) {
				asm volatile (
				"	move.2	%0, 14(%1)	\n\t"
				"	move.2	14(%1), 10(%1)	\n\t"
				"	move.2	12(%1), 8(%1)	\n\t"
				"	move.2	10(%1), 6(%1)	\n\t"
				"	move.2	8(%1), 4(%1)	\n\t"
				"	move.2	6(%1), 2(%1)	\n\t"
				"	move.2	4(%1), 0(%1)	\n\t"
					: "=m" (skb->vlan_tci)
					: "a" (skb->data)
					: "memory"
				);

				skb_pull(skb, VLAN_HLEN);
				skb->protocol = eth_type_trans(skb, dev);
				vlan_hwaccel_receive_skb(skb, priv->vlgrp, skb->vlan_tci);
				dev->last_rx = (long)ubi32_na.last_rx_jiffies;
				goto rx_done;
			}
#endif

			skb->protocol = eth_type_trans(skb, dev);
			netif_receive_skb(skb);
			dev->last_rx = (long)ubi32_na.last_rx_jiffies;
		}

	rx_done:
		count++;
	}

	return count;
}

/*
 * ubi32_na_napi_poll()
 *	polling routine used by NAPI
 */
static int ubi32_na_napi_poll(struct napi_struct *napi, int budget)
{
	u32_t count;

	/*
	 * First look to see if we're being asked for buffers to be pushed.
	 */
	struct na_if_map *nim = ubi32_na.regs;
	uint32_t int_status = nim->int_status;
	if (unlikely(int_status & NA_IF_MAP_INT_STATUS_PUSH)) {
		ubi32_na_push(nim);
	}

	/*
	 * Do have any buffers to pop?
	 */
	if (unlikely(int_status & NA_IF_MAP_INT_STATUS_POP)) {
		ubi32_na_pop(nim);
	}

	/*
	 * If we have TX slots available check if we're currently stopped.
	 * If we are then restart things now.
	 */
	if (likely(int_status & NA_IF_MAP_INT_STATUS_TX)) {
		if (unlikely(ubi32_na.flags & UBI32_NA_INSTANCE_FLAGS_TX_STOPPED)) {
			unsigned int i;

			na_info("cleared TX stopped state\n");
			for (i = 0; i < NA_MAX_INTERFACES; i++) {
				struct net_device *ldev = ubi32_na.devices[i];
				if (ldev) {
					netif_wake_queue(ldev);
				}
			}
			ubi32_na.flags &= ~UBI32_NA_INSTANCE_FLAGS_TX_STOPPED;
		}
	}

	count = ubi32_na_rx(nim, budget);

	if (count < budget) {
		napi_complete(napi);
		nim->int_enable = NA_IF_MAP_INT_ENABLE_RX | NA_IF_MAP_INT_ENABLE_POP |
					NA_IF_MAP_INT_ENABLE_PUSH | NA_IF_MAP_INT_ENABLE_TX;
		if (nim->rx_na_index != nim->rx_host_index || nim->pop_na_index != nim->pop_host_index) {
			nim->int_enable = 0;
			napi_schedule(napi);
		}
	}

	return count;
}

/*
 * ubi32_na_link_state_poll()
 *	polling routine to check link state
 */
static inline void ubi32_na_link_state_poll(struct net_device *dev)
{
	struct ubi32_na_private *priv = netdev_priv(dev);
	struct na_if_map_interface *nimi = priv->nimi;

	if (likely(nimi->status & NA_IF_MAP_INTERFACE_STATUS_LINK_UP)) {
		if (unlikely(!netif_carrier_ok(dev))) {
			netif_carrier_on(dev);
		}
	} else if (netif_carrier_ok(dev)) {
		netif_carrier_off(dev);
	}
}

/*
 * ubi32_na_interrupt()
 */
static irqreturn_t ubi32_na_interrupt(int irq, void *dev_id)
{
	struct ubi32_na_instance *uni = (struct ubi32_na_instance *)dev_id;
	struct na_if_map *nim = uni->regs;
	int i;

	for (i = 0; i < NA_MAX_INTERFACES; i++) {
		struct net_device *ldev = uni->devices[i];
		if (ldev) {
			ubi32_na_link_state_poll(ldev);
		}
	}

	if (unlikely(!(nim->int_status & nim->int_enable))) {
		return IRQ_NONE;
	}

	/*
	 * Disable new interrupts and start NAPI poll.
	 */
	nim->int_enable = 0;
	ubicom32_clear_interrupt(irq);
	napi_schedule(&uni->napi);

	return IRQ_HANDLED;
}

/*
 * ubi32_na_open()
 */
static int ubi32_na_open(struct net_device *dev)
{
	struct ubi32_na_private *priv = netdev_priv(dev);
	struct na_if_map_interface *nimi = priv->nimi;

	na_info("eth open %s\n", dev->name);

	/* check phy status and call netif_carrier_on */
	if (nimi->status & NA_IF_MAP_INTERFACE_STATUS_LINK_UP) {
		netif_carrier_on(dev);
	} else {
		netif_carrier_off(dev);
	}

	ubi32_na_vp_rxtx_start(dev);
	netif_start_queue(dev);
	return 0;
}

/*
 * We will use this code later.
 */
#if 0
/*
 * ubi32_na_destroy_all_rules
 *	Destroy all rules in NA.
 */
static int ubi32_na_destroy_all_rules(struct net_device *dev)
{
	struct sk_buff *skb;
	struct na_if_map *nim = ubi32_na.regs;
	uint32_t i;
	uint32_t ni;
	struct na_desc *txdesc;
	struct na_tx_metadata_object *ntmo;
	unsigned long ntmo_addr;

	skb = __dev_alloc_skb(NA_SKB_PAYLOAD_SIZE, GFP_ATOMIC | __GFP_NOWARN);
	if (!skb) {
		spin_lock_bh(&ubi32_na.lock);
		ubi32_na.skb_alloc_err++;
		spin_unlock_bh(&ubi32_na.lock);
		return 0;
	}

	spin_lock_bh(&ubi32_na.lock);
	i = nim->tx_host_index;
	ni = (i + 1) & (NA_NUM_TX_DESCRIPTORS - 1);

	if (ni == nim->tx_na_index) {
		/*
		 * We're not able to transmit right now!
		 */
		nim->int_status &= ~NA_IF_MAP_INT_STATUS_TX;
		ubi32_na.tx_q_full_cnt++;
		spin_unlock_bh(&ubi32_na.lock);
		dev_kfree_skb_any(skb);
		na_info("unable to enqueue 'destroy all' message - marked as stopped\n");
		return 0;
	}

	ntmo_addr = (unsigned long)skb_put(skb, sizeof(struct na_tx_metadata_object));
	ntmo = (struct na_tx_metadata_object *)ntmo_addr;
	ubi32_na_write_allocate_buffer(ntmo, sizeof(struct na_tx_metadata_object));
	ntmo->type = NA_TX_METADATA_TYPE_DESTROY_ALL_RULES;

	txdesc = &nim->tx_desc[i];
	txdesc->status = 0;
	txdesc->interface_num = 0;
	txdesc->opaque = (uint32_t)skb;
	txdesc->buffer = virt_to_phys(skb->head);
	txdesc->buffer_len = skb->end - skb->head;
	txdesc->payload_offs = skb->data - skb->head;
	txdesc->payload_len = skb->len;

	/*
	 * We are about to hand the new descriptor entry to the NA so
	 * we must not use txdesc from here onwards.  We set it to NULL, not
	 * because it does anything useful (and in fact for code that doesn't
	 * try to use txdesc again the compiler will eliminate this setting
	 * operation as "unused" in optimized builds) but if anyone adds a bad
	 * patch in the future that attempted to use txdesc it will trigger
	 * a segfault :-)
	 */
	txdesc = NULL;

	/*
	 * Update our host index so the NA sees we've written a new descriptor.
	 */
	nim->tx_host_index = ni;

	/*
	 * Kick the NA awake so it can process our new entry.
	 */
	ubi32_na_interrupt_na(nim);
	spin_unlock_bh(&ubi32_na.lock);

	return 0;
}

#endif

/*
 * ubi32_na_close()
 */
static int ubi32_na_close(struct net_device *dev)
{
//	volatile void *pdata;
//	struct sk_buff *skb;

	na_info("eth close %s\n", dev->name);
	netif_stop_queue(dev);	// can't transmit any more
	ubi32_na_vp_rxtx_stop(dev);

#if 0
	/*
	 * RX clean up
	 */
	while (priv->rx_tail != ubi32_na.regs->rx_in) {
		pdata = ubi32_na.regs->rx_dma_ring[priv->rx_tail];
		skb = container_of((void *)pdata, struct sk_buff, cb);
		ubi32_na.regs->rx_dma_ring[priv->rx_tail] = NULL;
		dev_kfree_skb_any(skb);
		priv->rx_tail = ((priv->rx_tail + 1) & RX_DMA_RING_MASK);
	}
	ubi32_na.regs->rx_in = 0;
	ubi32_na.regs->rx_out = ubi32_na.regs->rx_in;
	priv->rx_tail = ubi32_na.regs->rx_in;

	/*
	 * TX clean up
	 */
	BUG_ON(ubi32_na.regs->tx_out != ubi32_na.regs->tx_in);
	ubi32_na_tx_done(dev);
	BUG_ON(priv->tx_tail != ubi32_na.regs->tx_in);
	ubi32_na.regs->tx_in = 0;
	ubi32_na.regs->tx_out = ubi32_na.regs->tx_in;
	priv->tx_tail = ubi32_na.regs->tx_in;
#endif

	return 0;
}

/*
 * ubi32_na_set_config()
 */
static int ubi32_na_set_config(struct net_device *dev, struct ifmap *map)
{
	/*
	 * if must to down to config it
	 */
	na_info("set_config %x\n", dev->flags);
	if (dev->flags & IFF_UP)
		return -EBUSY;

	/*
	 * I/O and IRQ can not be changed
	 */
	if (map->base_addr != dev->base_addr) {
		na_warning("%s: Can't change I/O address\n", dev->name);
		return -EOPNOTSUPP;
	}

	if (map->irq != 0) {
		na_warning("%s: Can't change IRQ\n", dev->name);
		return -EOPNOTSUPP;
	}

	/*
	 * ignore other fields
	 */
	return 0;
}

/*
 * ubi32_na_start_xmit()
 */
static int ubi32_na_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ubi32_na_private *priv;
	struct na_if_map *nim = ubi32_na.regs;
	uint32_t i;
	uint32_t ni;
	uint32_t effective_na_index;
	uint32_t nr_frags;

	priv = netdev_priv(dev);
	spin_lock_bh(&ubi32_na.lock);

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
	if (priv->vlgrp && vlan_tx_tag_present(skb)) {
		if (!skb_cow_head(skb, VLAN_HLEN)) {
			spin_unlock_bh(&ubi32_na.lock);
			return NETDEV_TX_BUSY;
		}

		skb_push(skb, VLAN_HLEN);
		asm volatile (
		"	btst	%0, #0		\n\t"
		"	jmpeq.t	1f		\n\t"

		"	move.1	0(%0), 4(%0)	\n\t"	// perform 8-bit op.
		"	move.1	1(%0), 5(%0)	\n\t"
		"	move.1	2(%0), 6(%0)	\n\t"
		"	move.1	3(%0), 7(%0)	\n\t"
		"	move.1	4(%0), 8(%0)	\n\t"
		"	move.1	5(%0), 9(%0)	\n\t"
		"	move.1	6(%0), 10(%0)	\n\t"
		"	move.1	7(%0), 11(%0)	\n\t"
		"	move.1	8(%0), 12(%0)	\n\t"
		"	move.1	9(%0), 13(%0)	\n\t"
		"	move.1	10(%0), 14(%0)	\n\t"
		"	move.1	11(%0), 15(%0)	\n\t"
		"	move.1	12(%0), #0x81	\n\t"
		"	move.1	13(%0), #0x00	\n\t"
		"	move.1	15(%0), %1	\n\t"
		"	lsr.4	%1, %1, #8	\n\t"
		"	move.1	14(%0), %1	\n\t"
		"	jmpt.t	2f		\n\t"

		"1:	move.2	0(%0), 4(%0)	\n\t"	// perform 16-bit op.
		"	move.2	2(%0), 6(%0)	\n\t"
		"	move.2	4(%0), 8(%0)	\n\t"
		"	move.2	6(%0), 10(%0)	\n\t"
		"	move.2	8(%0), 12(%0)	\n\t"
		"	move.2	10(%0), 14(%0)	\n\t"
		"	movei	12(%0),	#0x8100	\n\t"
		"	move.2	14(%0), %1	\n\t"

		"2:				\n\t"
			:
			: "a" (skb->data), "d" (skb->vlan_tci)
			: "cc", "memory"
		);
	}
#endif

	nr_frags = skb_shinfo(skb)->nr_frags;
	BUG_ON(nr_frags > MAX_SKB_FRAGS);
	BUG_ON(skb_shinfo(skb)->gso_size != 0);

	/*
	 * We need to work out if there's sufficent space in our transmit descriptor
	 * ring to place all of the segments of the skbuff.  As the ring wraps
	 * around we have to try to unwrap it to make meaningful comparisons.  We
	 * start by working out where our "unwrapped" next index will be after a
	 * successful completion.
	 */
	i = nim->tx_host_index;
	ni = i + 1 + nr_frags;

	/*
	 * Now we work out where our NA index is if we take an "unwrapped" view of
	 * it too.
	 *
	 * In reality the NA index is always behind the host index for the transmit
	 * descriptor ring but we're worried about the case where we wrap around and
	 * catch it up again so effectively we can think of it being ahead of us.
	 */
	effective_na_index = nim->tx_na_index;
	if (effective_na_index <= i) {
		effective_na_index += NA_NUM_TX_DESCRIPTORS;
	}

	if (unlikely(ni >= effective_na_index)) {
		/*
		 * We're not able to transmit right now!
		 */
		nim->int_status &= ~NA_IF_MAP_INT_STATUS_TX;
		ubi32_na.tx_q_full_cnt++;
		ubi32_na.flags |= UBI32_NA_INSTANCE_FLAGS_TX_STOPPED;
		spin_unlock_bh(&ubi32_na.lock);
		netif_stop_queue(dev);
		na_info("unable to enqueue 'tx' - marked as stopped\n");
		return NETDEV_TX_BUSY;
	}

	/*
	 * Is this a conventional unfragmented skbuff?
	 */
	if (likely(nr_frags == 0)) {
		struct na_desc *txdesc;

		/*
		 * We have a simple linear buffer.
		 */
		txdesc = &nim->tx_desc[i];
		txdesc->status = NA_DESC_STATUS_PKT | NA_DESC_STATUS_FIRST_SEGMENT | NA_DESC_STATUS_LAST_SEGMENT;
		if (likely(skb->ip_summed == CHECKSUM_PARTIAL)) {
			txdesc->status |= NA_DESC_STATUS_GEN_IP_TRANSPORT_CHECKSUM;
		}
		txdesc->interface_num = (int8_t)priv->interface_num;
		txdesc->opaque = (uint32_t)skb;
		txdesc->buffer = virt_to_phys(skb->head);
		txdesc->buffer_len = skb->end - skb->head;
		txdesc->payload_offs = skb->data - skb->head;
		txdesc->payload_len = skb->len;
	} else {
		uint32_t j = 0;
		skb_frag_t *frag;
		struct na_desc *txdesc;

		/*
		 * We have a multi-fragment buffer.  Handle the first fragment
		 * (the header of skb).
		 */
		txdesc = &nim->tx_desc[i];
		txdesc->status = NA_DESC_STATUS_PKT | NA_DESC_STATUS_FIRST_SEGMENT | NA_DESC_STATUS_DISCARD;
		if (likely(skb->ip_summed == CHECKSUM_PARTIAL)) {
			txdesc->status |= NA_DESC_STATUS_GEN_IP_TRANSPORT_CHECKSUM;
		}
		txdesc->interface_num = (int8_t)priv->interface_num;
		txdesc->opaque = (uint32_t)NULL;
		txdesc->buffer = virt_to_phys(skb->head);
		txdesc->buffer_len = skb->end - skb->head;
		txdesc->payload_offs = skb->data - skb->head;
		txdesc->payload_len = skb->len - skb->data_len;

		/*
		 * Now handle all fragments except the last one.
		 */
		while (unlikely(j < (nr_frags - 1))) {
			i = (i + 1) & (NA_NUM_TX_DESCRIPTORS - 1);
			txdesc = &nim->tx_desc[i];

			frag = &skb_shinfo(skb)->frags[j++];
			txdesc->status = NA_DESC_STATUS_PKT | NA_DESC_STATUS_DISCARD;
			if (likely(skb->ip_summed == CHECKSUM_PARTIAL)) {
				txdesc->status |= NA_DESC_STATUS_GEN_IP_TRANSPORT_CHECKSUM;
			}
			txdesc->interface_num = (int8_t)priv->interface_num;
			txdesc->opaque = (uint32_t)NULL;
			txdesc->buffer = page_to_phys(frag->page) + frag->page_offset;
			txdesc->buffer_len = frag->size;
			txdesc->payload_offs = 0;
			txdesc->payload_len = frag->size;
		}

		/*
		 * Last fragment.
		 */
		i = (i + 1) & (NA_NUM_TX_DESCRIPTORS - 1);
		txdesc = &nim->tx_desc[i];

		frag = &skb_shinfo(skb)->frags[j];
		txdesc->status = NA_DESC_STATUS_PKT | NA_DESC_STATUS_LAST_SEGMENT;
		if (likely(skb->ip_summed == CHECKSUM_PARTIAL)) {
			txdesc->status |= NA_DESC_STATUS_GEN_IP_TRANSPORT_CHECKSUM;
		}
		txdesc->interface_num = (int8_t)priv->interface_num;
		txdesc->opaque = (uint32_t)skb;
		txdesc->buffer = page_to_phys(frag->page) + frag->page_offset;
		txdesc->buffer_len = frag->size;
		txdesc->payload_offs = 0;
		txdesc->payload_len = frag->size;
	}

	/*
	 * Update our host index so the NA sees we've written a new descriptor.
	 */
// XXX - actually make this atomic!
	nim->tx_host_index = ni & (NA_NUM_TX_DESCRIPTORS - 1);

	/*
	 * Kick the NA awake so it can process our new entry.
	 */
	ubi32_na_interrupt_na(nim);
	spin_unlock_bh(&ubi32_na.lock);

	dev->trans_start = jiffies;
	return NETDEV_TX_OK;
}

/*
 * ubi32_na_tx_timeout()
 *	Deal with a transmit timeout.
 */
static void ubi32_na_tx_timeout(struct net_device *dev)
{
	struct na_if_map *nim = ubi32_na.regs;
	struct ubi32_na_private *priv = netdev_priv(dev);

	spin_lock_bh(&ubi32_na.lock);
	priv->stats64.tx_errors++;
	spin_unlock_bh(&ubi32_na.lock);

	nim->int_enable |= NA_IF_MAP_INT_ENABLE_TX;
	ubicom32_set_interrupt(ubi32_na.irq);
	ubi32_na_interrupt_na(nim);
}

static int ubi32_na_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
na_warning("ubi32_na_ioctl");
#if 0
	struct ubi32_na_private *priv = netdev_priv(dev);
	struct mii_ioctl_data *data = if_mii(rq);

	na_info("%s: ioctl %d\n", dev->name, cmd);
	switch (cmd) {
	case SIOCGMIIPHY:
		data->phy_id = 0;
		break;

	case SIOCGMIIREG:
		if ((data->reg_num & 0x1F) == MII_BMCR) {
			/*
			 * Make up MII control register value from what we know
			 */
			data->val_out = 0x0000
					| ((ubi32_na.regs->status & UBI32_NA_VP_STATUS_DUPLEX)
							? BMCR_FULLDPLX : 0)
					| ((ubi32_na.regs->status & UBI32_NA_VP_STATUS_SPEED100)
							? BMCR_SPEED100 : 0)
					| ((ubi32_na.regs->status & UBI32_NA_VP_STATUS_SPEED1000)
							? BMCR_SPEED1000 : 0);
		} else if ((data->reg_num & 0x1F) == MII_BMSR) {
			/*
			 *  Make up MII status register value from what we know
			 */
			data->val_out =
					(BMSR_100FULL|BMSR_100HALF|BMSR_10FULL|BMSR_10HALF)
					| ((ubi32_na.regs->status & NA_IF_MAP_INTERFACE_STATUS_LINK_UP)
							? BMSR_LSTATUS : 0);
		} else {
			return -EIO;
		}
		break;

	case SIOCSMIIREG:
		return -EOPNOTSUPP;
		break;

	default:
		return -EOPNOTSUPP;
	}
#endif

	return 0;
}

/*
 * Return statistics to the caller
 */
static struct rtnl_link_stats64 *ubi32_na_get_stats64(struct net_device *dev, struct rtnl_link_stats64 *s64)
{
	struct ubi32_na_private *priv = netdev_priv(dev);

	spin_lock_bh(&ubi32_na.lock);
	s64->rx_packets = priv->stats64.rx_packets;
	s64->rx_bytes = priv->stats64.rx_bytes;
	s64->rx_errors = priv->stats64.rx_errors;
	s64->rx_dropped = priv->stats64.rx_dropped;
	s64->tx_packets = priv->stats64.tx_packets;
	s64->tx_bytes = priv->stats64.tx_bytes;
	s64->tx_errors = priv->stats64.tx_errors;
	s64->tx_dropped = priv->stats64.tx_dropped;
	s64->multicast = priv->stats64.multicast;
	s64->collisions = priv->stats64.collisions;

	s64->rx_length_errors = priv->stats64.rx_length_errors;
	s64->rx_over_errors = priv->stats64.rx_over_errors;
	s64->rx_crc_errors = priv->stats64.rx_crc_errors;
	s64->rx_frame_errors = priv->stats64.rx_frame_errors;
	s64->rx_fifo_errors = priv->stats64.rx_fifo_errors;
	s64->rx_missed_errors = priv->stats64.rx_missed_errors;

	s64->tx_aborted_errors = priv->stats64.tx_aborted_errors;
	s64->tx_carrier_errors = priv->stats64.tx_carrier_errors;
	s64->tx_fifo_errors = priv->stats64.tx_fifo_errors;
	s64->tx_heartbeat_errors = priv->stats64.tx_heartbeat_errors;
	s64->tx_window_errors = priv->stats64.tx_window_errors;

	s64->rx_compressed = priv->stats64.rx_compressed;
	s64->tx_compressed = priv->stats64.tx_compressed;
	spin_unlock_bh(&ubi32_na.lock);

	return s64;
}

static int ubi32_na_change_mtu(struct net_device *dev, int new_mtu)
{
	if ((new_mtu < 68) || (new_mtu > 1500))
		return -EINVAL;

	dev->mtu = new_mtu;
	na_info("set mtu to %d", new_mtu);

	return 0;
}

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
/*
 * ubi32_na_vlan_rx_register: register vlan for RX acceleration
 */
static void ubi32_na_vlan_rx_register(struct net_device *dev, struct vlan_group *grp)
{
	struct ubi32_na_private *priv = netdev_priv(dev);

	na_info("%s: register VLAN for HW acceleration\n", dev->name);
	priv->vlgrp = grp;
}
#endif

/*
 * ubi32_na_cleanup: unload the module
 */
void ubi32_na_cleanup(void)
{
	struct ubi32_na_private *priv;
	struct net_device *dev;
	int i;

	/*
	 * If we have NAPI active then disable it.
	 */
	if (ubi32_na.napi_active) {
		napi_disable(&ubi32_na.napi);
		ubi32_na.napi_active = false;
	}

	/*
	 * Free the IRQ allocated to the driver.
	 */
	free_irq(ubi32_na.irq, &ubi32_na);

	/*
	 * Clear down each net device that has been allocated.
	 */
	for (i = 0; i < NA_MAX_INTERFACES; i++) {
		dev = ubi32_na.devices[i];
		if (dev) {
			priv = netdev_priv(dev);
			unregister_netdev(dev);
			free_netdev(dev);
			ubi32_na.devices[i] = NULL;
		}
	}
}

/*
 * ubi32_na_set_mac_addr()
 *	Set MAC address of given net_device and also send it to NA.
 *
 * We set the given MAC address to net_device->dev_addr for Linux
 * and we also send it to NA for NA to update its local MAC address.
 * For this, we create a metadata which contains the MAC address and
 * the interface number (0 for eth0, 1 for eth1) and send it to NA.
 */
static int ubi32_na_set_mac_addr(struct net_device *dev, void *addr)
{
	struct sockaddr *mac = addr;

	struct sk_buff *skb;
	struct na_if_map *nim = ubi32_na.regs;
	uint32_t i;
	uint32_t ni;
	struct na_desc *txdesc;
	struct na_tx_metadata_object *ntmo;
	unsigned long ntmo_addr;
	struct na_mac_address_set *nmas;
	int interface_num;

	if (netif_running(dev)) {
		return -EBUSY;
	}

	/*
	 * Check that this is a valid MAC address.
	 */
	if (!is_valid_ether_addr(mac->sa_data)) {
		return -EADDRNOTAVAIL;
	}

	/*
	 * Learn which interface this MAC address is to be set for.
	 */
	interface_num = ubicom_na_interface_number_get(dev);
	if (interface_num < 0) {
		return -EADDRNOTAVAIL;
	}

	skb = __dev_alloc_skb(NA_SKB_PAYLOAD_SIZE, GFP_ATOMIC | __GFP_NOWARN);
	if (!skb) {
		spin_lock_bh(&ubi32_na.lock);
		ubi32_na.skb_alloc_err++;
		spin_unlock_bh(&ubi32_na.lock);
		return 0;
	}

	spin_lock_bh(&ubi32_na.lock);
	i = nim->tx_host_index;
	ni = (i + 1) & (NA_NUM_TX_DESCRIPTORS - 1);

	if (ni == nim->tx_na_index) {
		/*
		 * We're not able to transmit right now!
		 */
		nim->int_status &= ~NA_IF_MAP_INT_STATUS_TX;
		ubi32_na.tx_q_full_cnt++;
		spin_unlock_bh(&ubi32_na.lock);
		dev_kfree_skb_any(skb);
		na_info("unable to enqueue 'set mac addr' - marked as stopped\n");
		return 0;
	}

	/*
	 * Copy given MAC address to net_device's dev_addr
	 */
	memcpy(dev->dev_addr, mac->sa_data, ETH_ALEN);

	ntmo_addr = (unsigned long)skb_put(skb, sizeof(struct na_tx_metadata_object));
	ntmo = (struct na_tx_metadata_object *)ntmo_addr;
	ubi32_na_write_allocate_buffer(ntmo, sizeof(struct na_tx_metadata_object));
	ntmo->type = NA_TX_METADATA_TYPE_MAC_ADDR_SET;

	/*
	 * Copy MAC address to na_mac_address_set structure.
	 */
	nmas = &ntmo->sub.mac_address_set;
	nmas->interface = interface_num;
	memcpy(nmas->mac_addr, mac->sa_data, ETH_ALEN);

	na_info("MAC address to be set for interface %d: %x:%x:%x:%x:%x:%x\n", nmas->interface,
		nmas->mac_addr[0], nmas->mac_addr[1], nmas->mac_addr[2], nmas->mac_addr[3],
		nmas->mac_addr[4], nmas->mac_addr[5]);


	txdesc = &nim->tx_desc[i];
	txdesc->status = 0;
	txdesc->interface_num = 0;
	txdesc->opaque = (uint32_t)skb;
	txdesc->buffer = virt_to_phys(skb->head);
	txdesc->buffer_len = skb->end - skb->head;
	txdesc->payload_offs = skb->data - skb->head;
	txdesc->payload_len = skb->len;

	/*
	 * We are about to hand the new descriptor entry to the NA so
	 * we must not use txdesc from here onwards.  We set it to NULL, not
	 * because it does anything useful (and in fact for code that doesn't
	 * try to use txdesc again the compiler will eliminate this setting
	 * operation as "unused" in optimized builds) but if anyone adds a bad
	 * patch in the future that attempted to use txdesc it will trigger
	 * a segfault :-)
	 */
	txdesc = NULL;

	/*
	 * Update our host index so the NA sees we've written a new descriptor.
	 */
	nim->tx_host_index = ni;

	/*
	 * Kick the NA awake so it can process our new entry.
	 */
	ubi32_na_interrupt_na(nim);
	spin_unlock_bh(&ubi32_na.lock);

	return 0;
}

/*
 * ubi32_na_get_drvinfo()
 *	Return driver information.
 *
 * Note: Fields are 32 bytes in length.
 */
static void ubi32_na_get_drvinfo(struct net_device *dev,
				 struct ethtool_drvinfo *info)
{
	strcpy(info->driver, "ubi32_na");
	strcpy(info->version, "1.0");
	sprintf(info->bus_info, "Ubicom32: ultra");
}

/*
 * ubi32_na_get_msglevel()
 *	Retrieve the current debug level.
 */
static u32 ubi32_na_get_msglevel(struct net_device *dev)
{
	return 0;
}

/*
 * ubi32_na_set_msglevel()
 *	Set the debug message level.
 */
static void ubi32_na_set_msglevel(struct net_device *dev, u32 level)
{
}

/*
 * ubi32_na_set_flags()
 *	Enable support for: (ETH_FLAG_LRO | ETH_FLAG_NTUPLE | ETH_FLAG_RXHASH)
 *
 * TODO: Currently we do not support any of these.  If we never plan on doing
 * so, we are better off not having the function.  However, we need to implement
 * LRO.
 */
static int ubi32_na_set_flags(struct net_device *dev, u32 data)
{
	return ethtool_op_set_flags(dev, data, 0);
}

static const struct net_device_ops ubi32_na_netdev_ops = {
	.ndo_open = ubi32_na_open,
	.ndo_stop = ubi32_na_close,
	.ndo_start_xmit = ubi32_na_start_xmit,
	.ndo_tx_timeout = ubi32_na_tx_timeout,
	.ndo_set_config = ubi32_na_set_config,
	.ndo_do_ioctl = ubi32_na_ioctl,
	.ndo_get_stats64 = ubi32_na_get_stats64,
	.ndo_change_mtu = ubi32_na_change_mtu,
	.ndo_set_mac_address = ubi32_na_set_mac_addr,
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
	.ndo_vlan_rx_register = ubi32_na_vlan_rx_register,
#endif
};

static const struct ethtool_ops ubi32_na_ethtool_ops = {
	.get_link		= ethtool_op_get_link,
	.get_rx_csum		= ethtool_op_get_rx_csum,
	.get_tx_csum		= ethtool_op_get_tx_csum,
//	.set_tx_csum		= ethtool_op_set_tx_csum,
	.get_sg			= ethtool_op_get_sg,
//	.set_sg			= ethtool_op_set_sg,
//	.get_tso		= ethtool_op_get_tso,
//	.set_tso		= ethtool_op_set_tso,
//	.get_ufo		= ethtool_op_get_ufo,
//	.set_ufo		= ethtool_op_set_ufo,
	.get_flags		= ethtool_op_get_flags,
	.set_flags		= ubi32_na_set_flags,
	.get_drvinfo		= ubi32_na_get_drvinfo,
	.get_msglevel		= ubi32_na_get_msglevel,
	.set_msglevel		= ubi32_na_set_msglevel,
};

/*
 * ubi32_na_alloc_netdev()
 */
static struct net_device *ubi32_na_alloc_netdev(int32_t interface_num, struct na_if_map *nim)
{
	struct net_device *dev;
	struct ubi32_na_private *priv;
	int i;

	dev = alloc_etherdev(sizeof(struct ubi32_na_private));
	if (!dev) {
		na_warning("Failed to allocate net device object for interface: %d\n",
			interface_num);
		return NULL;
	}

	priv = netdev_priv(dev);
	priv->magic = UBICOM_NA_NETDEV_MAGIC;
	priv->dev = dev;
	priv->interface_num = interface_num;
	priv->nimi = &nim->per_if[interface_num];

	/*
	 * This just fill in some default Ubicom MAC address
	 */
	memcpy(dev->perm_addr, na_mac_addr[interface_num], ETH_ALEN);
	memcpy(dev->dev_addr, na_mac_addr[interface_num], ETH_ALEN);
	memset(dev->broadcast, 0xff, ETH_ALEN);

	/*
	 * Reset statistics info
	 */
	memset(&priv->stats64, 0, sizeof(struct rtnl_link_stats64));
	priv->host_rx_packets = 0;
	priv->host_rx_bytes = 0;
	priv->host_tx_packets = 0;
	priv->host_tx_bytes = 0;
	priv->ipv4_accelerated_rx_packets = 0;
	priv->ipv4_accelerated_rx_bytes = 0;
	priv->ipv4_accelerated_tx_packets = 0;
	priv->ipv4_accelerated_tx_bytes = 0;
	priv->ipv6_accelerated_rx_packets = 0;
	priv->ipv6_accelerated_rx_bytes = 0;
	priv->ipv6_accelerated_tx_packets = 0;
	priv->ipv6_accelerated_tx_bytes = 0;

	for (i = 0; i < NA_EXCEPTION_EVENT_UNKNOWN_LAST; i++) {
		priv->exception_events_unknown[i] = 0;
	}

	for (i = 0; i < NA_EXCEPTION_EVENT_IPV4_LAST; i++) {
		priv->exception_events_ipv4[i] = 0;
	}

	for (i = 0; i < NA_EXCEPTION_EVENT_IPV6_LAST; i++) {
		priv->exception_events_ipv6[i] = 0;
	}

	spin_lock_init(&ubi32_na.lock);

	dev->netdev_ops = &ubi32_na_netdev_ops;
	dev->ethtool_ops = &ubi32_na_ethtool_ops;
	dev->watchdog_timeo = UBI32_NA_VP_TX_TIMEOUT;

	/*
	 * Interfaces 0 and 1 have support for checksum offloading.
	 */
	if (interface_num < 2) {
		dev->features |= NETIF_F_SG | NETIF_F_HW_CSUM;
		dev->vlan_features |= NETIF_F_SG | NETIF_F_HW_CSUM;
	}

	/*
	 * Enable VLAN acceleration
	 */
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
	priv->vlgrp = NULL;
	dev->features |= NETIF_F_HW_VLAN_RX | NETIF_F_HW_VLAN_TX;
#endif

	return dev;
}

#define CHAR_DEV_MSG_SIZE 512

/*
 * ubi32_na_sysfs_char_device_read_ipv4_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv4_start(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "<ipv4>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv4_stats()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv4_stats(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;
	uint64_t ipv4_accelerated_rx_packets;
	uint64_t ipv4_accelerated_rx_bytes;
	uint64_t ipv4_accelerated_tx_packets;
	uint64_t ipv4_accelerated_tx_bytes;

	spin_lock_bh(&ubi32_na.lock);
	ipv4_accelerated_rx_packets = ubi32_na.ipv4_accelerated_rx_packets;
	ipv4_accelerated_rx_bytes = ubi32_na.ipv4_accelerated_rx_bytes;
	ipv4_accelerated_tx_packets = ubi32_na.ipv4_accelerated_tx_packets;
	ipv4_accelerated_tx_bytes = ubi32_na.ipv4_accelerated_tx_bytes;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<stats "
				"rx_pkts=\"%llu\" rx_bytes=\"%llu\" "
				"tx_pkts=\"%llu\" tx_bytes=\"%llu\" />\n",
				ipv4_accelerated_rx_packets,
				ipv4_accelerated_rx_bytes,
				ipv4_accelerated_tx_packets,
				ipv4_accelerated_tx_bytes);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv4_cache_stats()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv4_cache_stats(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;
	uint64_t ipv4_connection_create_requests;
	uint64_t ipv4_connection_create_collisions;
	uint64_t ipv4_connection_destroy_requests;
	uint64_t ipv4_connection_destroy_misses;
	uint64_t ipv4_connection_flushes;
	uint64_t ipv4_connection_evictions;
	uint64_t ipv4_connection_hash_hits;
	uint64_t ipv4_connection_hash_reorders;

	spin_lock_bh(&ubi32_na.lock);
	ipv4_connection_create_requests = ubi32_na.ipv4_connection_create_requests;
	ipv4_connection_create_collisions = ubi32_na.ipv4_connection_create_collisions;
	ipv4_connection_destroy_requests = ubi32_na.ipv4_connection_destroy_requests;
	ipv4_connection_destroy_misses = ubi32_na.ipv4_connection_destroy_misses;
	ipv4_connection_flushes = ubi32_na.ipv4_connection_flushes;
	ipv4_connection_evictions = ubi32_na.ipv4_connection_evictions;
	ipv4_connection_hash_hits = ubi32_na.ipv4_connection_hash_hits;
	ipv4_connection_hash_reorders = ubi32_na.ipv4_connection_hash_reorders;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<cache_stats "
				"create_requests=\"%llu\" create_collisions=\"%llu\" "
				"destroy_requests=\"%llu\" destroy_misses=\"%llu\" "
				"flushes=\"%llu\" evictions=\"%llu\" "
				"hash_hits=\"%llu\" hash_reorders=\"%llu\" />\n",
				ipv4_connection_create_requests,
				ipv4_connection_create_collisions,
				ipv4_connection_destroy_requests,
				ipv4_connection_destroy_misses,
				ipv4_connection_flushes,
				ipv4_connection_evictions,
				ipv4_connection_hash_hits,
				ipv4_connection_hash_reorders);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv4_cache_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv4_cache_start(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<cache>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv4_cache_entry()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv4_cache_entry(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	struct na_ipv4_statistics *nis;

	spin_lock_bh(&ubi32_na.lock);
	nis = &ubi32_na.na_ipv4_statistics[xws->index];
	if (!nis->last_sync) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		int bytes_read;
		uint8_t protocol;
		int32_t flow_interface;
		uint32_t flow_ip;
		uint32_t flow_ip_xlate;
		uint32_t flow_ident;
		uint32_t flow_ident_xlate;
		uint64_t flow_accelerated_rx_packets;
		uint64_t flow_accelerated_rx_bytes;
		uint64_t flow_accelerated_tx_packets;
		uint64_t flow_accelerated_tx_bytes;
		int32_t return_interface;
		uint32_t return_ip;
		uint32_t return_ip_xlate;
		uint32_t return_ident;
		uint32_t return_ident_xlate;
		uint64_t return_accelerated_rx_packets;
		uint64_t return_accelerated_rx_bytes;
		uint64_t return_accelerated_tx_packets;
		uint64_t return_accelerated_tx_bytes;
		uint64_t last_sync;

		protocol = nis->protocol;
		flow_interface = nis->flow_interface;
		flow_ip = nis->flow_ip;
		flow_ip_xlate = nis->flow_ip_xlate;
		flow_ident = nis->flow_ident;
		flow_ident_xlate = nis->flow_ident_xlate;
		flow_accelerated_rx_packets = nis->flow_accelerated_rx_packets;
		flow_accelerated_rx_bytes = nis->flow_accelerated_rx_bytes;
		flow_accelerated_tx_packets = nis->flow_accelerated_tx_packets;
		flow_accelerated_tx_bytes = nis->flow_accelerated_tx_bytes;
		return_interface = nis->return_interface;
		return_ip = nis->return_ip;
		return_ip_xlate = nis->return_ip_xlate;
		return_ident = nis->return_ident;
		return_ident_xlate = nis->return_ident_xlate;
		return_accelerated_rx_packets = nis->return_accelerated_rx_packets;
		return_accelerated_rx_bytes = nis->return_accelerated_rx_bytes;
		return_accelerated_tx_packets = nis->return_accelerated_tx_packets;
		return_accelerated_tx_bytes = nis->return_accelerated_tx_bytes;
		last_sync = nis->last_sync;
		spin_unlock_bh(&ubi32_na.lock);

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t<entry "
					"protocol=\"%u\" "
					"flow_interface=\"%d\" "
					"flow_ip=\"%pI4\" flow_ip_xlate=\"%pI4\" "
					"flow_ident=\"%u\" flow_ident_xlate=\"%u\" "
					"flow_rx_pkts=\"%llu\" flow_rx_bytes=\"%llu\" "
					"flow_tx_pkts=\"%llu\" flow_tx_bytes=\"%llu\" "
					"ret_interface=\"%d\" "
					"ret_ip=\"%pI4\" ret_ip_xlate=\"%pI4\" "
					"ret_ident=\"%u\" ret_ident_xlate=\"%u\" "
					"ret_rx_pkts=\"%llu\" ret_rx_bytes=\"%llu\" "
					"ret_tx_pkts=\"%llu\" ret_tx_bytes=\"%llu\" "
					"last_sync=\"%llu\" />\n",
					protocol,
					flow_interface,
					&flow_ip, &flow_ip_xlate,
					flow_ident, flow_ident_xlate,
					flow_accelerated_rx_packets,
					flow_accelerated_rx_bytes,
					flow_accelerated_tx_packets,
					flow_accelerated_tx_bytes,
					return_interface,
					&return_ip, &return_ip_xlate,
					return_ident, return_ident_xlate,
					return_accelerated_rx_packets,
					return_accelerated_rx_bytes,
					return_accelerated_tx_packets,
					return_accelerated_tx_bytes,
					last_sync);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->index++;
	if (xws->index >= IPV4_CONNECTION_ENTRIES) {
		xws->index = 0;
		xws->state++;
	}

	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv4_cache_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv4_cache_end(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t</cache>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv4_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv4_end(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "</ipv4>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv6_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv6_start(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "<ipv6>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv6_stats()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv6_stats(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;
	uint64_t ipv6_accelerated_rx_packets;
	uint64_t ipv6_accelerated_rx_bytes;
	uint64_t ipv6_accelerated_tx_packets;
	uint64_t ipv6_accelerated_tx_bytes;

	spin_lock_bh(&ubi32_na.lock);
	ipv6_accelerated_rx_packets = ubi32_na.ipv6_accelerated_rx_packets;
	ipv6_accelerated_rx_bytes = ubi32_na.ipv6_accelerated_rx_bytes;
	ipv6_accelerated_tx_packets = ubi32_na.ipv6_accelerated_tx_packets;
	ipv6_accelerated_tx_bytes = ubi32_na.ipv6_accelerated_tx_bytes;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<stats "
				"rx_pkts=\"%llu\" rx_bytes=\"%llu\" "
				"tx_pkts=\"%llu\" tx_bytes=\"%llu\" />\n",
				ipv6_accelerated_rx_packets,
				ipv6_accelerated_rx_bytes,
				ipv6_accelerated_tx_packets,
				ipv6_accelerated_tx_bytes);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv6_cache_stats()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv6_cache_stats(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;
	uint64_t ipv6_connection_create_requests;
	uint64_t ipv6_connection_create_collisions;
	uint64_t ipv6_connection_destroy_requests;
	uint64_t ipv6_connection_destroy_misses;
	uint64_t ipv6_connection_flushes;
	uint64_t ipv6_connection_evictions;
	uint64_t ipv6_connection_hash_hits;
	uint64_t ipv6_connection_hash_reorders;

	spin_lock_bh(&ubi32_na.lock);
	ipv6_connection_create_requests = ubi32_na.ipv6_connection_create_requests;
	ipv6_connection_create_collisions = ubi32_na.ipv6_connection_create_collisions;
	ipv6_connection_destroy_requests = ubi32_na.ipv6_connection_destroy_requests;
	ipv6_connection_destroy_misses = ubi32_na.ipv6_connection_destroy_misses;
	ipv6_connection_flushes = ubi32_na.ipv6_connection_flushes;
	ipv6_connection_evictions = ubi32_na.ipv6_connection_evictions;
	ipv6_connection_hash_hits = ubi32_na.ipv6_connection_hash_hits;
	ipv6_connection_hash_reorders = ubi32_na.ipv6_connection_hash_reorders;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<cache_stats "
				"create_requests=\"%llu\" create_collisions=\"%llu\" "
				"destroy_requests=\"%llu\" destroy_misses=\"%llu\" "
				"flushes=\"%llu\" evictions=\"%llu\" "
				"hash_hits=\"%llu\" hash_reorders=\"%llu\" />\n",
				ipv6_connection_create_requests,
				ipv6_connection_create_collisions,
				ipv6_connection_destroy_requests,
				ipv6_connection_destroy_misses,
				ipv6_connection_flushes,
				ipv6_connection_evictions,
				ipv6_connection_hash_hits,
				ipv6_connection_hash_reorders);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv6_cache_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv6_cache_start(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<cache>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv6_cache_entry()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv6_cache_entry(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	struct na_ipv6_statistics *nis;

	spin_lock_bh(&ubi32_na.lock);
	nis = &ubi32_na.na_ipv6_statistics[xws->index];
	if (!nis->last_sync) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		int bytes_read;
		uint8_t protocol;
		int32_t flow_interface;
		uint32_t flow_ip[4];
		uint32_t flow_ident;
		uint64_t flow_accelerated_rx_packets;
		uint64_t flow_accelerated_rx_bytes;
		uint64_t flow_accelerated_tx_packets;
		uint64_t flow_accelerated_tx_bytes;
		int32_t return_interface;
		uint32_t return_ip[4];
		uint32_t return_ident;
		uint64_t return_accelerated_rx_packets;
		uint64_t return_accelerated_rx_bytes;
		uint64_t return_accelerated_tx_packets;
		uint64_t return_accelerated_tx_bytes;
		uint64_t last_sync;

		protocol = nis->protocol;
		flow_interface =  nis->flow_interface;
		flow_ip[0] = nis->flow_ip[0];
		flow_ip[1] = nis->flow_ip[1];
		flow_ip[2] = nis->flow_ip[2];
		flow_ip[3] = nis->flow_ip[3];
		flow_ident = nis->flow_ident;
		flow_accelerated_rx_packets = nis->flow_accelerated_rx_packets;
		flow_accelerated_rx_bytes = nis->flow_accelerated_rx_bytes;
		flow_accelerated_tx_packets = nis->flow_accelerated_tx_packets;
		flow_accelerated_tx_bytes = nis->flow_accelerated_tx_bytes;
		return_interface = nis->return_interface;
		return_ip[0] = nis->return_ip[0];
		return_ip[1] = nis->return_ip[1];
		return_ip[2] = nis->return_ip[2];
		return_ip[3] = nis->return_ip[3];
		return_ident = nis->return_ident;
		return_accelerated_rx_packets = nis->return_accelerated_rx_packets;
		return_accelerated_rx_bytes = nis->return_accelerated_rx_bytes;
		return_accelerated_tx_packets = nis->return_accelerated_tx_packets;
		return_accelerated_tx_bytes = nis->return_accelerated_tx_bytes;
		last_sync = nis->last_sync;
		spin_unlock_bh(&ubi32_na.lock);

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t<entry "
					"protocol=\"%u\" "
					"flow_interface=\"%d\" "
					"flow_ip=\"%pI6\" flow_ident=\"%u\" "
					"flow_accel_rx_pkts=\"%llu\" flow_accel_rx_bytes=\"%llu\" "
					"flow_accel_tx_pkts=\"%llu\" flow_accel_tx_bytes=\"%llu\" "
					"ret_interface=\"%d\" "
					"ret_ip=\"%pI6\" ret_ident=\"%u\" "
					"ret_accel_rx_pkts=\"%llu\" ret_accel_rx_bytes=\"%llu\" "
					"ret_accel_tx_pkts=\"%llu\" ret_accel_tx_bytes=\"%llu\" "
					"last_sync=\"%llu\" />\n",
					protocol,
					flow_interface,
					flow_ip, flow_ident,
					flow_accelerated_rx_packets,
					flow_accelerated_rx_bytes,
					flow_accelerated_tx_packets,
					flow_accelerated_tx_bytes,
					return_interface,
					return_ip, return_ident,
					return_accelerated_rx_packets,
					return_accelerated_rx_bytes,
					return_accelerated_tx_packets,
					return_accelerated_tx_bytes,
					last_sync);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->index++;
	if (xws->index >= IPV6_CONNECTION_ENTRIES) {
		xws->index = 0;
		xws->state++;
	}

	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv6_cache_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv6_cache_end(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t</cache>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_ipv6_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_ipv6_end(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "</ipv6>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_start(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "<interfaces>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_start(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	if (!dev) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		int bytes_read;
		struct ubi32_na_private *priv = netdev_priv(dev);
		int32_t interface_num;
		char name[IFNAMSIZ];

		interface_num = priv->interface_num;
		strncpy(name, dev->name, IFNAMSIZ);
		spin_unlock_bh(&ubi32_na.lock);

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<interface num=\"%u\" name=\"%s\">\n",
					interface_num, name);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_stats()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_stats(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	if (!dev) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		int bytes_read;
		struct ubi32_na_private *priv = netdev_priv(dev);
		uint64_t rx_packets;
		uint64_t rx_bytes;
		uint64_t tx_packets;
		uint64_t tx_bytes;

		rx_packets = priv->stats64.rx_packets;
		rx_bytes = priv->stats64.rx_bytes;
		tx_packets = priv->stats64.tx_packets;
		tx_bytes = priv->stats64.tx_bytes;
		spin_unlock_bh(&ubi32_na.lock);

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
					"\t\t<stats rx_packets=\"%llu\" rx_bytes=\"%llu\" "
					"tx_packets=\"%llu\" tx_bytes=\"%llu\" />\n",
					rx_packets, rx_bytes, tx_packets, tx_bytes);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_host_stats()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_host_stats(char *buffer, char *msg, size_t *length, int *total_read,
										struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	if (!dev) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		int bytes_read;
		struct ubi32_na_private *priv = netdev_priv(dev);
		uint64_t host_rx_packets;
		uint64_t host_rx_bytes;
		uint64_t host_tx_packets;
		uint64_t host_tx_bytes;

		host_rx_packets = priv->host_rx_packets;
		host_rx_bytes = priv->host_rx_bytes;
		host_tx_packets = priv->host_tx_packets;
		host_tx_bytes = priv->host_tx_bytes;
		spin_unlock_bh(&ubi32_na.lock);

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
					"\t\t<host_stats rx_packets=\"%llu\" rx_bytes=\"%llu\" "
					"tx_packets=\"%llu\" tx_bytes=\"%llu\" />\n",
					host_rx_packets, host_rx_bytes, host_tx_packets, host_tx_bytes);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_exception()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_exception(char *buffer, char *msg, size_t *length, int *total_read,
										struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	if (!dev) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		uint64_t ct;
		struct ubi32_na_private *priv = netdev_priv(dev);

		ct = priv->exception_events_unknown[xws->index2];
		spin_unlock_bh(&ubi32_na.lock);
		if (ct) {
			int bytes_read;

			bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
						"\t\t<exception name=\"%s\" count=\"%llu\" />\n",
						exception_events_unknown_string[xws->index2],
						ct);
			if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
				return false;
			}

			*length -= bytes_read;
			*total_read += bytes_read;
		}
	}

	xws->index2++;
	if (xws->index2 >= NA_EXCEPTION_EVENT_UNKNOWN_LAST) {
		xws->index2 = 0;
		xws->state++;
	}
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_start(char *buffer, char *msg, size_t *length, int *total_read,
										struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	spin_unlock_bh(&ubi32_na.lock);
	if (dev) {
		int bytes_read;

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t<ipv4>\n");
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_stats()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_stats(char *buffer, char *msg, size_t *length, int *total_read,
										struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	if (!dev) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		int bytes_read;
		struct ubi32_na_private *priv = netdev_priv(dev);
		uint64_t ipv4_accelerated_rx_packets;
		uint64_t ipv4_accelerated_rx_bytes;
		uint64_t ipv4_accelerated_tx_packets;
		uint64_t ipv4_accelerated_tx_bytes;

		ipv4_accelerated_rx_packets = priv->ipv4_accelerated_rx_packets;
		ipv4_accelerated_rx_bytes = priv->ipv4_accelerated_rx_bytes;
		ipv4_accelerated_tx_packets = priv->ipv4_accelerated_tx_packets;
		ipv4_accelerated_tx_bytes = priv->ipv4_accelerated_tx_bytes;
		spin_unlock_bh(&ubi32_na.lock);

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
					"\t\t\t<stats rx_packets=\"%llu\" rx_bytes=\"%llu\" "
					"tx_packets=\"%llu\" tx_bytes=\"%llu\" />\n",
					ipv4_accelerated_rx_packets,
					ipv4_accelerated_rx_bytes,
					ipv4_accelerated_tx_packets,
					ipv4_accelerated_tx_bytes);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_exception()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_exception(char *buffer, char *msg, size_t *length, int *total_read,
										struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	if (!dev) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		uint64_t ct;
		struct ubi32_na_private *priv = netdev_priv(dev);

		ct = priv->exception_events_ipv4[xws->index2];
		spin_unlock_bh(&ubi32_na.lock);
		if (ct) {
			int bytes_read;

			bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
						"\t\t\t<exception name=\"%s\" count=\"%llu\" />\n",
						exception_events_ipv4_string[xws->index2],
						ct);
			if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
				return false;
			}

			*length -= bytes_read;
			*total_read += bytes_read;
		}
	}

	xws->index2++;
	if (xws->index2 >= NA_EXCEPTION_EVENT_IPV4_LAST) {
		xws->index2 = 0;
		xws->state++;
	}
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_end(char *buffer, char *msg, size_t *length, int *total_read,
										struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	spin_unlock_bh(&ubi32_na.lock);
	if (dev) {
		int bytes_read;

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t</ipv4>\n");
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_start(char *buffer, char *msg, size_t *length, int *total_read,
										struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	spin_unlock_bh(&ubi32_na.lock);
	if (dev) {
		int bytes_read;

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t<ipv6>\n");
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_stats()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_stats(char *buffer, char *msg, size_t *length, int *total_read,
										struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	if (!dev) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		int bytes_read;
		struct ubi32_na_private *priv = netdev_priv(dev);
		uint64_t ipv6_accelerated_rx_packets;
		uint64_t ipv6_accelerated_rx_bytes;
		uint64_t ipv6_accelerated_tx_packets;
		uint64_t ipv6_accelerated_tx_bytes;

		ipv6_accelerated_rx_packets = priv->ipv6_accelerated_rx_packets;
		ipv6_accelerated_rx_bytes = priv->ipv6_accelerated_rx_bytes;
		ipv6_accelerated_tx_packets = priv->ipv6_accelerated_tx_packets;
		ipv6_accelerated_tx_bytes = priv->ipv6_accelerated_tx_bytes;
		spin_unlock_bh(&ubi32_na.lock);

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
					"\t\t\t<stats rx_packets=\"%llu\" rx_bytes=\"%llu\" "
					"tx_packets=\"%llu\" tx_bytes=\"%llu\" />\n",
					ipv6_accelerated_rx_packets,
					ipv6_accelerated_rx_bytes,
					ipv6_accelerated_tx_packets,
					ipv6_accelerated_tx_bytes);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_exception()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_exception(char *buffer, char *msg, size_t *length, int *total_read,
										struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	if (!dev) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		uint64_t ct;
		struct ubi32_na_private *priv = netdev_priv(dev);

		ct = priv->exception_events_ipv6[xws->index2];
		spin_unlock_bh(&ubi32_na.lock);
		if (ct) {
			int bytes_read;

			bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
						"\t\t\t<exception name=\"%s\" count=\"%llu\" />\n",
						exception_events_ipv6_string[xws->index2],
						ct);
			if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
				return false;
			}

			*length -= bytes_read;
			*total_read += bytes_read;
		}
	}

	xws->index2++;
	if (xws->index2 >= NA_EXCEPTION_EVENT_IPV6_LAST) {
		xws->index2 = 0;
		xws->state++;
	}
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_end(char *buffer, char *msg, size_t *length, int *total_read,
										struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	spin_unlock_bh(&ubi32_na.lock);
	if (dev) {
		int bytes_read;

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t</ipv6>\n");
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_interface_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_interface_end(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct net_device *dev;

	spin_lock_bh(&ubi32_na.lock);
	dev = ubi32_na.devices[xws->index];
	spin_unlock_bh(&ubi32_na.lock);
	if (dev) {
		int bytes_read;

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t</interface>\n");
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->index++;
	if (xws->index < NA_MAX_INTERFACES) {
		xws->state = UBI32_NA_STATS_XML_STATE_INTERFACES_INTERFACE_START;
	} else {
		xws->index = 0;
		xws->state++;
	}
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_interfaces_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_interfaces_end(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "</interfaces>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_pbufs_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_pbufs_start(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "<pbufs>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_pbufs_stats()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_pbufs_stats(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;
	uint64_t pbuf_alloc_fails;
	uint64_t pbuf_payload_alloc_fails;

	spin_lock_bh(&ubi32_na.lock);
	pbuf_alloc_fails = ubi32_na.pbuf_alloc_fails;
	pbuf_payload_alloc_fails = ubi32_na.pbuf_payload_alloc_fails;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<stats "
				"pbuf_fails=\"%llu\" payload_fails=\"%llu\" />\n",
				pbuf_alloc_fails, pbuf_payload_alloc_fails);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_pbufs_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_pbufs_end(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "</pbufs>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_pe_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_pe_start(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "<pe>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_pe_queue()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_pe_queue(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;
	uint64_t pe_queue_dropped;

	spin_lock_bh(&ubi32_na.lock);
	pe_queue_dropped = ubi32_na.pe_queue_dropped;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<queue dropped=\"%llu\" />\n",
				pe_queue_dropped);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_pe_timings()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_pe_timings(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	spin_lock_bh(&ubi32_na.lock);
	if (!ubi32_na.pe_worst_case_ticks) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		uint64_t pe_iterations;
		uint64_t pe_total_ticks;
		uint32_t pe_worst_case_ticks;

		pe_iterations = ubi32_na.pe_iterations;
		pe_total_ticks = ubi32_na.pe_total_ticks;
		pe_worst_case_ticks = ubi32_na.pe_worst_case_ticks;
		spin_unlock_bh(&ubi32_na.lock);

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<timings "
					"iterations=\"%llu\" ticks=\"%llu\" "
					"avg_ticks=\"%u\" worst_ticks=\"%u\" />\n",
					pe_iterations, pe_total_ticks,
					(uint32_t)(pe_total_ticks / pe_iterations),
					pe_worst_case_ticks);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}
	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_pe_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_pe_end(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "</pe>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_except_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_except_start(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "<except>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_except_queue()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_except_queue(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;
	uint64_t except_queue_dropped;

	spin_lock_bh(&ubi32_na.lock);
	except_queue_dropped = ubi32_na.except_queue_dropped;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<queue dropped=\"%llu\" />\n",
				except_queue_dropped);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_except_timings()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_except_timings(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	spin_lock_bh(&ubi32_na.lock);
	if (!ubi32_na.except_worst_case_ticks) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		uint64_t except_iterations;
		uint64_t except_total_ticks;
		uint32_t except_worst_case_ticks;

		except_iterations = ubi32_na.except_iterations;
		except_total_ticks = ubi32_na.except_total_ticks;
		except_worst_case_ticks = ubi32_na.except_worst_case_ticks;
		spin_unlock_bh(&ubi32_na.lock);

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<timings "
					"iterations=\"%llu\" ticks=\"%llu\" "
					"avg_ticks=\"%u\" worst_ticks=\"%u\" />\n",
					except_iterations, except_total_ticks,
					(uint32_t)(except_total_ticks / except_iterations),
					except_worst_case_ticks);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}
	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_except_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_except_end(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "</except>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_start(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "<gmacs>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_start(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<gmac num=\"%u\">\n", xws->index);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_stats()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_stats(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;
	uint64_t rx_packets;
	uint64_t rx_bytes;
	uint64_t tx_packets;
	uint64_t tx_bytes;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	rx_packets = ngs->rx_packets;
	rx_bytes = ngs->rx_bytes;
	tx_packets = ngs->tx_packets;
	tx_bytes = ngs->tx_bytes;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
				"\t\t<stats rx_packets=\"%llu\" rx_bytes=\"%llu\" "
				"tx_packets=\"%llu\" tx_bytes=\"%llu\" />\n",
				rx_packets, rx_bytes, tx_packets, tx_bytes);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_rx_errors1()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_rx_errors1(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;
	uint64_t rx_errors;
	uint64_t rx_receive_errors;
	uint64_t rx_overflow_errors;
	uint64_t rx_descriptor_errors;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	rx_errors = ngs->rx_errors;
	rx_receive_errors = ngs->rx_receive_errors;
	rx_overflow_errors = ngs->rx_overflow_errors;
	rx_descriptor_errors = ngs->rx_descriptor_errors;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
				"\t\t<rx_errors1 errors=\"%llu\" receive=\"%llu\" "
				"overflow=\"%llu\" descriptor=\"%llu\" />\n",
				rx_errors, rx_receive_errors,
				rx_overflow_errors, rx_descriptor_errors);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_rx_errors2()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_rx_errors2(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;
	uint64_t rx_watchdog_timeout_errors;
	uint64_t rx_crc_errors;
	uint64_t rx_late_collision_errors;
	uint64_t rx_dribble_bit_errors;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	rx_watchdog_timeout_errors = ngs->rx_watchdog_timeout_errors;
	rx_crc_errors = ngs->rx_crc_errors;
	rx_late_collision_errors = ngs->rx_late_collision_errors;
	rx_dribble_bit_errors = ngs->rx_dribble_bit_errors;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
				"\t\t<rx_errors2 watchdog_timeout=\"%llu\" crc=\"%llu\" "
				"late_collision=\"%llu\" dribble_bit=\"%llu\" />\n",
				rx_watchdog_timeout_errors, rx_crc_errors,
				rx_late_collision_errors, rx_dribble_bit_errors);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_rx_errors3()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_rx_errors3(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;
	uint64_t rx_length_errors;
	uint64_t rx_ip_header_errors;
	uint64_t rx_ip_payload_errors;
	uint64_t rx_no_buffer_errors;
	uint64_t rx_transport_csum_bypassed;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	rx_length_errors = ngs->rx_length_errors;
	rx_ip_header_errors = ngs->rx_ip_header_errors;
	rx_ip_payload_errors = ngs->rx_ip_payload_errors;
	rx_no_buffer_errors = ngs->rx_no_buffer_errors;
	rx_transport_csum_bypassed = ngs->rx_transport_csum_bypassed;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
				"\t\t<rx_errors3 length=\"%llu\" ip_header=\"%llu\" "
				"ip_payload=\"%llu\" no_buffer=\"%llu\" "
				"transport_csum_bypassed=\"%llu\" />\n",
				rx_length_errors, rx_ip_header_errors,
				rx_ip_payload_errors, rx_no_buffer_errors,
				rx_transport_csum_bypassed);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_tx_errors1()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_tx_errors1(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;
	uint64_t tx_collisions;
	uint64_t tx_errors;
	uint64_t tx_jabber_timeout_errors;
	uint64_t tx_frame_flushed_errors;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	tx_collisions = ngs->tx_collisions;
	tx_errors = ngs->tx_errors;
	tx_jabber_timeout_errors = ngs->tx_jabber_timeout_errors;
	tx_frame_flushed_errors = ngs->tx_frame_flushed_errors;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
				"\t\t<tx_errors1 collisions=\"%llu\" errors=\"%llu\" "
				"jabber_timeout=\"%llu\" frame_flushed=\"%llu\" />\n",
				tx_collisions, tx_errors, tx_jabber_timeout_errors,
				tx_frame_flushed_errors);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_tx_errors2()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_tx_errors2(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;
	uint64_t tx_loss_of_carrier_errors;
	uint64_t tx_no_carrier_errors;
	uint64_t tx_late_collision_errors;
	uint64_t tx_excessive_collision_errors;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	tx_loss_of_carrier_errors = ngs->tx_loss_of_carrier_errors;
	tx_no_carrier_errors = ngs->tx_no_carrier_errors;
	tx_late_collision_errors = ngs->tx_late_collision_errors;
	tx_excessive_collision_errors = ngs->tx_excessive_collision_errors;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
				"\t\t<tx_errors2 loss_of_carrier=\"%llu\" no_carrier=\"%llu\" "
				"late_collision=\"%llu\" excessive_collision=\"%llu\" />\n",
				tx_loss_of_carrier_errors, tx_no_carrier_errors,
				tx_late_collision_errors, tx_excessive_collision_errors);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_tx_errors3()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_tx_errors3(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;
	uint64_t tx_excessive_deferral_errors;
	uint64_t tx_underflow_errors;
	uint64_t tx_ip_header_errors;
	uint64_t tx_ip_payload_errors;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	tx_excessive_deferral_errors = ngs->tx_excessive_deferral_errors;
	tx_underflow_errors = ngs->tx_underflow_errors;
	tx_ip_header_errors = ngs->tx_ip_header_errors;
	tx_ip_payload_errors = ngs->tx_ip_payload_errors;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
				"\t\t<tx_errors3 excessive_deferral=\"%llu\" underflow=\"%llu\" "
				"ip_header=\"%llu\" ip_payload=\"%llu\" />\n",
				tx_excessive_deferral_errors, tx_underflow_errors,
				tx_ip_header_errors, tx_ip_payload_errors);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_dma_errors1()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_dma_errors1(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;
	uint64_t rx_missed;
	uint64_t fifo_overflows;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	rx_missed = ngs->rx_missed;
	fifo_overflows = ngs->fifo_overflows;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
				"\t\t<dma_errors1 rx_missed=\"%llu\" fifo_overflows=\"%llu\" />\n",
				rx_missed, fifo_overflows);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_dma_errors2()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_dma_errors2(char *buffer, char *msg, size_t *length, int *total_read,
									struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;
	uint64_t hw_errs[10];
	int i;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	for (i = 0; i < 10; i++) {
		hw_errs[i] = ngs->hw_errs[i];
	}
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
				"\t\t<dma_errors2 err0=\"%llu\" err1=\"%llu\" "
				"err2=\"%llu\" err3=\"%llu\" "
				"err4=\"%llu\" err5=\"%llu\" "
				"err6=\"%llu\" err7=\"%llu\" "
				"err8=\"%llu\" err9=\"%llu\" />\n",
				hw_errs[0], hw_errs[1], hw_errs[2], hw_errs[3],
				hw_errs[4], hw_errs[5], hw_errs[6], hw_errs[7],
				hw_errs[8], hw_errs[9]);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_queue()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_queue(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;
	uint64_t tx_dropped;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	tx_dropped = ngs->tx_dropped;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t<queue dropped=\"%llu\" />\n", tx_dropped);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_timings()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_timings(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	struct na_gmac_statistics *ngs;
	int bytes_read;

	spin_lock_bh(&ubi32_na.lock);
	ngs = &ubi32_na.na_gmac_statistics[xws->index];
	if (!ngs->gmac_worst_case_ticks) {
		spin_unlock_bh(&ubi32_na.lock);
	} else {
		uint64_t gmac_iterations;
		uint64_t gmac_total_ticks;
		uint32_t gmac_worst_case_ticks;

		gmac_iterations = ngs->gmac_iterations;
		gmac_total_ticks = ngs->gmac_total_ticks;
		gmac_worst_case_ticks = ngs->gmac_worst_case_ticks;
		spin_unlock_bh(&ubi32_na.lock);

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t<timings "
					"iterations=\"%llu\" ticks=\"%llu\" "
					"avg_ticks=\"%u\" worst_ticks=\"%u\" />\n",
					gmac_iterations, gmac_total_ticks,
					(uint32_t)(gmac_total_ticks / gmac_iterations),
					gmac_worst_case_ticks);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_gmac_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_gmac_end(char *buffer, char *msg, size_t *length, int *total_read,
								struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t</gmac>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->index++;
	if (xws->index < NA_MAX_INTERFACES) {
		xws->state = UBI32_NA_STATS_XML_STATE_GMACS_GMAC_START;
	} else {
		xws->index = 0;
		xws->state++;
	}
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_gmacs_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_gmacs_end(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "</gmacs>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_linux_start()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_linux_start(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "<linux>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_linux_queue()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_linux_queue(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;
	uint64_t skb_alloc_err;
	uint64_t tx_q_full_cnt;

	spin_lock_bh(&ubi32_na.lock);
	skb_alloc_err = ubi32_na.skb_alloc_err;
	tx_q_full_cnt = ubi32_na.tx_q_full_cnt;
	spin_unlock_bh(&ubi32_na.lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<queue "
				"skb_alloc_errors=\"%llu\" tx_queue_full=\"%llu\" />\n",
				skb_alloc_err, tx_q_full_cnt);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * ubi32_na_sysfs_char_device_read_linux_end()
 *	Generate part of the XML output.
 */
static bool ubi32_na_sysfs_char_device_read_linux_end(char *buffer, char *msg, size_t *length, int *total_read,
							struct ubi32_na_xml_write_state *xws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "</linux>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	xws->state++;
	return true;
}

/*
 * Array of write functions that write various XML elements that correspond to
 * our XML output state machine.
 */
ubi32_na_stats_xml_write_method_t ubi32_na_stats_xml_write_methods[UBI32_NA_STATS_XML_STATE_DONE] = {
	ubi32_na_sysfs_char_device_read_ipv4_start,
	ubi32_na_sysfs_char_device_read_ipv4_stats,
	ubi32_na_sysfs_char_device_read_ipv4_cache_stats,
	ubi32_na_sysfs_char_device_read_ipv4_cache_start,
	ubi32_na_sysfs_char_device_read_ipv4_cache_entry,
	ubi32_na_sysfs_char_device_read_ipv4_cache_end,
	ubi32_na_sysfs_char_device_read_ipv4_end,
	ubi32_na_sysfs_char_device_read_ipv6_start,
	ubi32_na_sysfs_char_device_read_ipv6_stats,
	ubi32_na_sysfs_char_device_read_ipv6_cache_stats,
	ubi32_na_sysfs_char_device_read_ipv6_cache_start,
	ubi32_na_sysfs_char_device_read_ipv6_cache_entry,
	ubi32_na_sysfs_char_device_read_ipv6_cache_end,
	ubi32_na_sysfs_char_device_read_ipv6_end,
	ubi32_na_sysfs_char_device_read_interfaces_start,
	ubi32_na_sysfs_char_device_read_interfaces_interface_start,
	ubi32_na_sysfs_char_device_read_interfaces_interface_stats,
	ubi32_na_sysfs_char_device_read_interfaces_interface_host_stats,
	ubi32_na_sysfs_char_device_read_interfaces_interface_exception,
	ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_start,
	ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_stats,
	ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_exception,
	ubi32_na_sysfs_char_device_read_interfaces_interface_ipv4_end,
	ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_start,
	ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_stats,
	ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_exception,
	ubi32_na_sysfs_char_device_read_interfaces_interface_ipv6_end,
	ubi32_na_sysfs_char_device_read_interfaces_interface_end,
	ubi32_na_sysfs_char_device_read_interfaces_end,
	ubi32_na_sysfs_char_device_read_pbufs_start,
	ubi32_na_sysfs_char_device_read_pbufs_stats,
	ubi32_na_sysfs_char_device_read_pbufs_end,
	ubi32_na_sysfs_char_device_read_pe_start,
	ubi32_na_sysfs_char_device_read_pe_queue,
	ubi32_na_sysfs_char_device_read_pe_timings,
	ubi32_na_sysfs_char_device_read_pe_end,
	ubi32_na_sysfs_char_device_read_except_start,
	ubi32_na_sysfs_char_device_read_except_queue,
	ubi32_na_sysfs_char_device_read_except_timings,
	ubi32_na_sysfs_char_device_read_except_end,
	ubi32_na_sysfs_char_device_read_gmacs_start,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_start,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_stats,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_rx_errors1,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_rx_errors2,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_rx_errors3,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_tx_errors1,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_tx_errors2,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_tx_errors3,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_dma_errors1,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_dma_errors2,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_queue,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_timings,
	ubi32_na_sysfs_char_device_read_gmacs_gmac_end,
	ubi32_na_sysfs_char_device_read_gmacs_end,
	ubi32_na_sysfs_char_device_read_linux_start,
	ubi32_na_sysfs_char_device_read_linux_queue,
	ubi32_na_sysfs_char_device_read_linux_end
};

/*
 * ubi32_na_sysfs_char_device_read()
 *	Send cache entries info to userspace upon read request from user
 */
static ssize_t ubi32_na_sysfs_char_device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	char msg[CHAR_DEV_MSG_SIZE];
	int total_read = 0;
	struct ubi32_na_xml_write_state *xws;

	xws = (struct ubi32_na_xml_write_state *)filp->private_data;
	while ((xws->state != UBI32_NA_STATS_XML_STATE_DONE) && (length > CHAR_DEV_MSG_SIZE)) {
		if ((ubi32_na_stats_xml_write_methods[xws->state])(buffer, msg, &length, &total_read, xws)) {
			continue;
		}
	}

	return total_read;
}

/*
 * ubi32_na_sysfs_char_device_write()
 *	Write to char device not required/supported
 */
static ssize_t ubi32_na_sysfs_char_device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	return -EINVAL;
}

/*
 * ubi32_na_sysfs_char_device_open()
 */
static int ubi32_na_sysfs_char_device_open(struct inode *inode, struct file *file)
{
	struct ubi32_na_xml_write_state *write_state;

	write_state = file->private_data;
	if (!write_state) {
		write_state = kzalloc(sizeof(struct ubi32_na_xml_write_state), GFP_KERNEL);
		if (!write_state) {
			return -ENOMEM;
		}
		write_state->state = UBI32_NA_STATS_XML_STATE_IPV4_START;
		write_state->index = 0;
		write_state->index2 = 0;

		file->private_data = write_state;
	}

	return 0;
}

/*
 * ubi32_na_sysfs_char_device_release()
 */
static int ubi32_na_sysfs_char_device_release(struct inode *inode, struct file *file)
{
	struct ubi32_na_xml_write_state *write_state;
	write_state = (struct ubi32_na_xml_write_state *)file->private_data;
	if (write_state) {
		kfree(write_state);
	}

	return 0;
}

/*
 * File operations used in the sysfs char device
 */
static struct file_operations ubi32_na_sysfs_fops = {
	.read = ubi32_na_sysfs_char_device_read,
	.write = ubi32_na_sysfs_char_device_write,
	.open = ubi32_na_sysfs_char_device_open,
	.release = ubi32_na_sysfs_char_device_release
};

/*
 * ubi32_na_reset_counters()
 *	Reset sysfs statistic counters
 */
static void ubi32_na_reset_counters(void)
{
	int i;

	/*
	 * Zero down the global stats.
	 */
	ubi32_na.ipv4_accelerated_rx_packets = 0;
	ubi32_na.ipv4_accelerated_rx_bytes = 0;
	ubi32_na.ipv4_accelerated_tx_packets = 0;
	ubi32_na.ipv4_accelerated_tx_bytes = 0;
	ubi32_na.ipv4_connection_create_requests = 0;
	ubi32_na.ipv4_connection_create_collisions = 0;
	ubi32_na.ipv4_connection_destroy_requests = 0;
	ubi32_na.ipv4_connection_destroy_misses = 0;
	ubi32_na.ipv4_connection_hash_hits = 0;
	ubi32_na.ipv4_connection_hash_reorders = 0;
	ubi32_na.ipv4_connection_flushes = 0;
	ubi32_na.ipv4_connection_evictions = 0;
	ubi32_na.ipv6_accelerated_rx_packets = 0;
	ubi32_na.ipv6_accelerated_rx_bytes = 0;
	ubi32_na.ipv6_accelerated_tx_bytes = 0;
	ubi32_na.ipv6_accelerated_tx_packets = 0;
	ubi32_na.ipv6_connection_create_requests = 0;
	ubi32_na.ipv6_connection_create_collisions = 0;
	ubi32_na.ipv6_connection_destroy_requests = 0;
	ubi32_na.ipv6_connection_destroy_misses = 0;
	ubi32_na.ipv6_connection_hash_hits = 0;
	ubi32_na.ipv6_connection_hash_reorders = 0;
	ubi32_na.ipv6_connection_flushes = 0;
	ubi32_na.ipv6_connection_evictions = 0;
	ubi32_na.pe_queue_dropped = 0;
	ubi32_na.pe_total_ticks = 0;
	ubi32_na.pe_worst_case_ticks = 0;
	ubi32_na.pe_iterations = 0;
	ubi32_na.except_queue_dropped = 0;
	ubi32_na.except_total_ticks = 0;
	ubi32_na.except_worst_case_ticks = 0;
	ubi32_na.except_iterations = 0;
	ubi32_na.pbuf_alloc_fails = 0;
	ubi32_na.pbuf_payload_alloc_fails = 0;

	for (i = 0; i < IPV4_CONNECTION_ENTRIES; i++) {
		ubi32_na.na_ipv4_statistics[i].last_sync = 0;
	}

	for (i = 0; i < IPV6_CONNECTION_ENTRIES; i++) {
		ubi32_na.na_ipv6_statistics[i].last_sync = 0;
	}

	/*
	 * Zero down the per-interface stats.
	 */
	for (i = 0; i < NA_MAX_INTERFACES; i++) {
		struct net_device *dev;
		dev = ubi32_na.devices[i];
		if (dev) {
			int j;
			struct ubi32_na_private *priv;

			priv = netdev_priv(dev);
			memset(&priv->stats64, 0, sizeof(struct rtnl_link_stats64));
			priv->host_rx_packets = 0;
			priv->host_rx_bytes = 0;
			priv->host_tx_packets = 0;
			priv->host_tx_bytes = 0;
			priv->ipv4_accelerated_rx_packets = 0;
			priv->ipv4_accelerated_rx_bytes = 0;
			priv->ipv4_accelerated_tx_packets = 0;
			priv->ipv4_accelerated_tx_bytes = 0;
			priv->ipv6_accelerated_rx_packets = 0;
			priv->ipv6_accelerated_rx_bytes = 0;
			priv->ipv6_accelerated_tx_packets = 0;
			priv->ipv6_accelerated_tx_bytes = 0;

			for (j = 0; j < NA_EXCEPTION_EVENT_UNKNOWN_LAST; j++) {
				priv->exception_events_unknown[j] = 0;
			}

			for (j = 0; j < NA_EXCEPTION_EVENT_IPV4_LAST; j++) {
				priv->exception_events_ipv4[j] = 0;
			}

			for (j = 0; j < NA_EXCEPTION_EVENT_IPV6_LAST; j++) {
				priv->exception_events_ipv6[j] = 0;
			}
		}
	}

	/*
	 * Zero down the GMAC stats.
	 */
	for (i = 0; i < NA_MAX_INTERFACES; i++) {
		int j;

		struct na_gmac_statistics *ngs = &ubi32_na.na_gmac_statistics[i];
		ngs->rx_bytes = 0;
		ngs->rx_packets = 0;
		ngs->rx_errors = 0;
		ngs->rx_receive_errors = 0;
		ngs->rx_overflow_errors = 0;
		ngs->rx_descriptor_errors = 0;
		ngs->rx_watchdog_timeout_errors = 0;
		ngs->rx_crc_errors = 0;
		ngs->rx_late_collision_errors = 0;
		ngs->rx_dribble_bit_errors = 0;
		ngs->rx_length_errors = 0;
		ngs->rx_ip_header_errors = 0;
		ngs->rx_ip_payload_errors = 0;
		ngs->rx_no_buffer_errors = 0;
		ngs->rx_transport_csum_bypassed = 0;
		ngs->tx_bytes = 0;
		ngs->tx_packets = 0;
		ngs->tx_collisions = 0;
		ngs->tx_errors = 0;
		ngs->tx_jabber_timeout_errors = 0;
		ngs->tx_frame_flushed_errors = 0;
		ngs->tx_loss_of_carrier_errors = 0;
		ngs->tx_no_carrier_errors = 0;
		ngs->tx_late_collision_errors = 0;
		ngs->tx_excessive_collision_errors = 0;
		ngs->tx_excessive_deferral_errors = 0;
		ngs->tx_underflow_errors = 0;
		ngs->tx_ip_header_errors = 0;
		ngs->tx_ip_payload_errors = 0;
		ngs->tx_dropped = 0;
		for (j = 0; j < 10; j++) {
			ngs->hw_errs[j] = 0;
		}

		ngs->rx_missed = 0;
		ngs->fifo_overflows = 0;
		ngs->gmac_total_ticks = 0;
		ngs->gmac_worst_case_ticks = 0;
		ngs->gmac_iterations = 0;
	}
}

/*
 * ubi32_na_get_reset_counters()
 */
static ssize_t ubi32_na_get_reset_counters(struct sys_device *dev, struct sysdev_attribute *attr, char *buf)
{
	return sprintf(buf, "0\n");
}

/*
 * ubi32_na_set_reset_counters()
 *	Get reset_counters value, for any value, resets sysfs statistics counters
 */
static ssize_t ubi32_na_set_reset_counters(struct sys_device *dev, struct sysdev_attribute *attr, const char *buf, size_t count)
{
	spin_lock_bh(&ubi32_na.lock);
	ubi32_na_reset_counters();
	spin_unlock_bh(&ubi32_na.lock);
	return count;
}

/*
 * ubi32_na_get_cache_dev_major()
 *	Send major number of char device that hold cache entries data.
 */
static ssize_t ubi32_na_get_cache_dev_major(struct sys_device *dev, struct sysdev_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", ubi32_na.cache_dev_major);
}

/*
 * Create sysfs attributes for sysfs entries
 */
static SYSDEV_ATTR(reset_counters, 0644, ubi32_na_get_reset_counters, ubi32_na_set_reset_counters);
static SYSDEV_ATTR(cache_dev_major, 0444, ubi32_na_get_cache_dev_major, NULL);

/*
 * ubi32_na_add_sysfs()
 *	Add sysfs support, register sysfs class and create sysdev files. Register char device.
 */
int ubi32_na_add_sysfs(void)
{
	int result;

	/*
	 * Register the sysfs class
	 */
	result = sysdev_class_register(&ubi32_na_sysclass);
	if (result) {
		na_warning("Failed to register SysFS class %d\n", result);
		goto sysfs_cleanup_1;
	}

	/*
	 * Register SYSFS device control
	 */
	memset(&ubi32_na_sys_dev, 0, sizeof(ubi32_na_sys_dev));
	ubi32_na_sys_dev.id = 0;
	ubi32_na_sys_dev.cls = &ubi32_na_sysclass;
	result = sysdev_register(&ubi32_na_sys_dev);
	if (result) {
		na_warning("Failed to register SysFS device %d\n", result);
		goto sysfs_cleanup_2;
	}

	result = sysdev_create_file(&ubi32_na_sys_dev, &attr_reset_counters);
	if (result) {
		na_warning("Failed to register reset_counters file %d\n", result);
		goto sysfs_cleanup_8;
	}

	result = sysdev_create_file(&ubi32_na_sys_dev, &attr_cache_dev_major);
	if (result) {
		na_warning("Failed to register cache_dev_major file %d\n", result);
		goto sysfs_cleanup_11;
	}

	/*
	 * Register a char device
	 */
	result = register_chrdev(0, ubi32_na_sysclass.name, &ubi32_na_sysfs_fops);
	if (result < 0) {
		na_warning("Failed to register chrdev %d\n", result);
		goto sysfs_cleanup_12;
	}

	ubi32_na.cache_dev_major = result;
	return 0;

sysfs_cleanup_12:
	sysdev_remove_file(&ubi32_na_sys_dev, &attr_cache_dev_major);
sysfs_cleanup_11:
	sysdev_remove_file(&ubi32_na_sys_dev, &attr_reset_counters);
sysfs_cleanup_8:
	sysdev_unregister(&ubi32_na_sys_dev);
sysfs_cleanup_2:
	sysdev_class_unregister(&ubi32_na_sysclass);
sysfs_cleanup_1:
	return result;
}

/*
 * ubi32_na_init_module()
 */
int ubi32_na_init_module(void)
{
	struct na_devtree_node *na_dn;
	int i;
	int err;
	struct na_if_map *nim;

	/*
	 * Look for the device tree node.
	 */
	na_dn = (struct na_devtree_node *)devtree_find_node("NA");
	if (!na_dn) {
		na_info("NA does not exist\n");
		return -ENODEV;
	}

	na_info("Found NA - interfaces: %d\n", na_dn->num_interfaces);
	if (!na_dn->num_interfaces) {
		return -ENODEV;
	}

	nim = na_dn->map;
	ubi32_na.regs = nim;

	na_info("NA: nim: 0x%p, na_int: %d host_int: %d\n",
		nim, nim->na_int, nim->host_int);

	/*
	 * Request the IRQ that we require.
	 */
	ubi32_na.irq = nim->host_int;
	err = request_irq(ubi32_na.irq, ubi32_na_interrupt, IRQF_DISABLED, "ubi32_na", &ubi32_na);
	if (err) {
		na_warning("fail to request_irq %d\n", err);
		return -ENODEV;
	}

	/*
	 * Device allocation.
	 */
	err = 0;
	for (i = 0; i < na_dn->num_interfaces; i++) {
		struct net_device *dev;

		dev = ubi32_na_alloc_netdev(i, nim);
		if (!dev) {
			err = -ENOMEM;
			break;
		}

		err = register_netdev(dev);
		if (err) {
			na_warning("Failed to register NA interface %d\n", i);
			free_netdev(dev);
			break;
		}

		ubi32_na.devices[i] = dev;
	}

	/*
	 * If we encountered any errors during initialization then tear down whatever
	 * we did complete and return an error.
	 */
	if (err) {
		ubi32_na_cleanup();
		return err;
	}

	/*
	 * Set up NAPI polling.
	 */
	netif_napi_add(ubi32_na.devices[0], &ubi32_na.napi, ubi32_na_napi_poll, 64);
	napi_enable(&ubi32_na.napi);
	ubi32_na.napi_active = true;

	ubi32_na_add_sysfs();

	return 0;
}

/*
 * ubicom_na_net_dev_register_ipv4_mgr()
 */
void ubicom_na_net_dev_register_ipv4_mgr(ubicom_na_ipv4_sync_callback_t event_callback)
{
	spin_lock_bh(&ubi32_na.lock);
	ubi32_na.ipv4_sync = event_callback;
	spin_unlock_bh(&ubi32_na.lock);
}

/*
 * ubicom_na_net_dev_unregister_ipv4_mgr()
 */
void ubicom_na_net_dev_unregister_ipv4_mgr(void)
{
	spin_lock_bh(&ubi32_na.lock);
	ubi32_na.ipv4_sync = NULL;
	spin_unlock_bh(&ubi32_na.lock);
}

/*
 * ubicom_na_create_ipv4_rule()
 *	Create a NA entry to accelerate the given connection
 */
void ubicom_na_create_ipv4_rule(struct ubicom_na_ipv4_create *unic)
{
	struct sk_buff *skb;
	struct na_if_map *nim = ubi32_na.regs;
	uint32_t i;
	uint32_t ni;
	struct na_desc *txdesc;
	struct na_tx_metadata_object *ntmo;
	unsigned long ntmo_addr;
	struct na_ipv4_rule_create *nirc;

	na_debug("Create IPv4: %pI4:%d (%pI4:%d), %pI4:%d (%pI4:%d), p: %d\n",
		&unic->src_ip, unic->src_port, &unic->src_ip_xlate, unic->src_port_xlate,
		&unic->dest_ip, unic->dest_port, &unic->dest_ip_xlate, unic->dest_port_xlate, unic->protocol);
	skb = __dev_alloc_skb(NA_SKB_PAYLOAD_SIZE, GFP_ATOMIC | __GFP_NOWARN);
	if (unlikely(!skb)) {
		spin_lock_bh(&ubi32_na.lock);
		ubi32_na.skb_alloc_err++;
		spin_unlock_bh(&ubi32_na.lock);
		return;
	}

	spin_lock_bh(&ubi32_na.lock);
	i = nim->tx_host_index;
	ni = (i + 1) & (NA_NUM_TX_DESCRIPTORS - 1);

	if (unlikely(ni == nim->tx_na_index)) {
		/*
		 * We're not able to transmit right now!
		 */
		nim->int_status &= ~NA_IF_MAP_INT_STATUS_TX;
		ubi32_na.tx_q_full_cnt++;
		spin_unlock_bh(&ubi32_na.lock);
		dev_kfree_skb_any(skb);
		na_info("unable to enqueue 'IPv4 rule create' - marked as stopped\n");
		return;
	}

	ntmo_addr = (unsigned long)skb_put(skb, sizeof(struct na_tx_metadata_object));
	ntmo = (struct na_tx_metadata_object *)ntmo_addr;
	ubi32_na_write_allocate_buffer(ntmo, sizeof(struct na_tx_metadata_object));
	ntmo->type = NA_TX_METADATA_TYPE_IPV4_RULE_CREATE;

	nirc = &ntmo->sub.ipv4_rule_create;
	nirc->protocol = (uint8_t)unic->protocol;

	nirc->flow_interface_num = unic->src_interface_num;
	nirc->flow_ip = unic->src_ip;
	nirc->flow_ip_xlate = unic->src_ip_xlate;
	nirc->flow_ident = (uint32_t)unic->src_port;
	nirc->flow_ident_xlate = (uint32_t)unic->src_port_xlate;
	nirc->flow_window_scale = unic->flow_window_scale;
	nirc->flow_max_window = unic->flow_max_window;
	nirc->flow_end = unic->flow_end;
	nirc->flow_max_end = unic->flow_max_end;
	memcpy(nirc->flow_mac, unic->src_mac, 6);

	nirc->return_interface_num = unic->dest_interface_num;
	nirc->return_ip = unic->dest_ip;
	nirc->return_ip_xlate = unic->dest_ip_xlate;
	nirc->return_ident = (uint32_t)unic->dest_port;
	nirc->return_ident_xlate = (uint32_t)unic->dest_port_xlate;
	nirc->return_window_scale = unic->return_window_scale;
	nirc->return_max_window = unic->return_max_window;
	nirc->return_end = unic->return_end;
	nirc->return_max_end = unic->return_max_end;
	if (nirc->return_ip != nirc->return_ip_xlate || nirc->return_ident != nirc->return_ident_xlate) {
		memcpy(nirc->return_mac, unic->dest_mac_xlate, 6);
	} else {
		memcpy(nirc->return_mac, unic->dest_mac, 6);
	}

	nirc->flags = 0;
	if (unic->flags & UBICOM_NA_IPV4_CREATE_FLAG_NO_SEQ_CHECK) {
		nirc->flags |= NA_IPV4_RULE_CREATE_FLAG_NO_SEQ_CHECK;
	}

	txdesc = &nim->tx_desc[i];
	txdesc->status = 0;
	txdesc->interface_num = 0;
	txdesc->opaque = (uint32_t)skb;
	txdesc->buffer = virt_to_phys(skb->head);
	txdesc->buffer_len = skb->end - skb->head;
	txdesc->payload_offs = skb->data - skb->head;
	txdesc->payload_len = skb->len;

	/*
	 * We are about to hand the new descriptor entry to the NA so
	 * we must not use txdesc from here onwards.  We set it to NULL, not
	 * because it does anything useful (and in fact for code that doesn't
	 * try to use txdesc again the compiler will eliminate this setting
	 * operation as "unused" in optimized builds) but if anyone adds a bad
	 * patch in the future that attempted to use txdesc it will trigger
	 * a segfault :-)
	 */
	txdesc = NULL;

	/*
	 * Update our host index so the NA sees we've written a new descriptor.
	 */
	nim->tx_host_index = ni;

	/*
	 * Kick the NA awake so it can process our new entry.
	 */
	ubi32_na_interrupt_na(nim);
	spin_unlock_bh(&ubi32_na.lock);
}

/*
 * ubicom_na_destroy_ipv4_rule()
 *	Destroy the given connection in the NA
 */
void ubicom_na_destroy_ipv4_rule(struct ubicom_na_ipv4_destroy *unid)
{
	struct sk_buff *skb;
	struct na_if_map *nim = ubi32_na.regs;
	uint32_t i;
	uint32_t ni;
	struct na_desc *txdesc;
	struct na_tx_metadata_object *ntmo;
	unsigned long ntmo_addr;
	struct na_ipv4_rule_destroy *nird;

	na_debug("Destroy IPv4: %pI4:%d, %pI4:%d, p: %d\n",
		&unid->src_ip, unid->src_port, &unid->dest_ip, unid->dest_port, unid->protocol);
	skb = __dev_alloc_skb(NA_SKB_PAYLOAD_SIZE, GFP_ATOMIC | __GFP_NOWARN);
	if (unlikely(!skb)) {
		spin_lock_bh(&ubi32_na.lock);
		ubi32_na.skb_alloc_err++;
		spin_unlock_bh(&ubi32_na.lock);
// XXX - use a return code?
		return;
	}

	spin_lock_bh(&ubi32_na.lock);
	i = nim->tx_host_index;
	ni = (i + 1) & (NA_NUM_TX_DESCRIPTORS - 1);

	if (unlikely(ni == nim->tx_na_index)) {
		/*
		 * We're not able to transmit right now!
		 */
		nim->int_status &= ~NA_IF_MAP_INT_STATUS_TX;
		ubi32_na.tx_q_full_cnt++;
		spin_unlock_bh(&ubi32_na.lock);
		dev_kfree_skb_any(skb);
		na_info("unable to enqueue 'IPv4 destroy rule' - marked as stopped\n");
		return;
	}

	ntmo_addr = (unsigned long)skb_put(skb, sizeof(struct na_tx_metadata_object));
	ntmo = (struct na_tx_metadata_object *)ntmo_addr;
	ubi32_na_write_allocate_buffer(ntmo, sizeof(struct na_tx_metadata_object));
	ntmo->type = NA_TX_METADATA_TYPE_IPV4_RULE_DESTROY;

	nird = &ntmo->sub.ipv4_rule_destroy;
	nird->protocol = (uint8_t)unid->protocol;
	nird->flow_ip = unid->src_ip;
	nird->flow_ident = (uint32_t)unid->src_port;
	nird->return_ip = unid->dest_ip;
	nird->return_ident = (uint32_t)unid->dest_port;

	txdesc = &nim->tx_desc[i];
	txdesc->status = 0;
	txdesc->interface_num = 0;
	txdesc->opaque = (uint32_t)skb;
	txdesc->buffer = virt_to_phys(skb->head);
	txdesc->buffer_len = skb->end - skb->head;
	txdesc->payload_offs = skb->data - skb->head;
	txdesc->payload_len = skb->len;

	/*
	 * We are about to hand the new descriptor entry to the NA so
	 * we must not use txdesc from here onwards.  We set it to NULL, not
	 * because it does anything useful (and in fact for code that doesn't
	 * try to use txdesc again the compiler will eliminate this setting
	 * operation as "unused" in optimized builds) but if anyone adds a bad
	 * patch in the future that attempted to use txdesc it will trigger
	 * a segfault :-)
	 */
	txdesc = NULL;

	/*
	 * Update our host index so the NA sees we've written a new descriptor.
	 */
	nim->tx_host_index = ni;

	/*
	 * Kick the NA awake so it can process our new entry.
	 */
	ubi32_na_interrupt_na(nim);
	spin_unlock_bh(&ubi32_na.lock);
}

/*
 * ubicom_na_net_dev_register_ipv6_mgr()
 *	Called to register an IPv6 connection manager with this driver
 */
void ubicom_na_net_dev_register_ipv6_mgr(ubicom_na_ipv6_sync_callback_t event_callback)
{
	spin_lock_bh(&ubi32_na.lock);
	ubi32_na.ipv6_sync = event_callback;
	spin_unlock_bh(&ubi32_na.lock);
}

/*
 * ubicom_na_net_dev_unregister_ipv6_mgr()
 *	Called to unregister an IPv6 connection manager
 */
void ubicom_na_net_dev_unregister_ipv6_mgr(void)
{
	spin_lock_bh(&ubi32_na.lock);
	ubi32_na.ipv6_sync = NULL;
	spin_unlock_bh(&ubi32_na.lock);
}

/*
 * ubicom_na_create_ipv6_rule()
 *	Create a NA entry to accelerate the given connection
 */
void ubicom_na_create_ipv6_rule(struct ubicom_na_ipv6_create *unic)
{
	struct sk_buff *skb;
	struct na_if_map *nim = ubi32_na.regs;
	uint32_t i;
	uint32_t ni;
	struct na_desc *txdesc;
	struct na_tx_metadata_object *ntmo;
	unsigned long ntmo_addr;
	struct na_ipv6_rule_create *nirc;

	na_debug("Create IPv6: %pI6:%d, %pI6:%d, p: %d\n",
		unic->src_ip, unic->src_port, unic->dest_ip, unic->dest_port, unic->protocol);
	skb = __dev_alloc_skb(NA_SKB_PAYLOAD_SIZE, GFP_ATOMIC | __GFP_NOWARN);
	if (unlikely(!skb)) {
		spin_lock_bh(&ubi32_na.lock);
		ubi32_na.skb_alloc_err++;
		spin_unlock_bh(&ubi32_na.lock);
		return;
	}

	spin_lock_bh(&ubi32_na.lock);
	i = nim->tx_host_index;
	ni = (i + 1) & (NA_NUM_TX_DESCRIPTORS - 1);

	if (unlikely(ni == nim->tx_na_index)) {
		/*
		 * We're not able to transmit right now!
		 */
		nim->int_status &= ~NA_IF_MAP_INT_STATUS_TX;
		ubi32_na.tx_q_full_cnt++;
		spin_unlock_bh(&ubi32_na.lock);
		dev_kfree_skb_any(skb);
		na_info("unable to enqueue 'IPv6 create rule' - marked as stopped\n");
		return;
	}

	ntmo_addr = (unsigned long)skb_put(skb, sizeof(struct na_tx_metadata_object));
	ntmo = (struct na_tx_metadata_object *)ntmo_addr;
	ubi32_na_write_allocate_buffer(ntmo, sizeof(struct na_tx_metadata_object));
	ntmo->type = NA_TX_METADATA_TYPE_IPV6_RULE_CREATE;

	nirc = &ntmo->sub.ipv6_rule_create;
	nirc->protocol = (uint8_t)unic->protocol;

	nirc->flow_interface_num = unic->src_interface_num;
	nirc->flow_ip[0] = unic->src_ip[0];
	nirc->flow_ip[1] = unic->src_ip[1];
	nirc->flow_ip[2] = unic->src_ip[2];
	nirc->flow_ip[3] = unic->src_ip[3];
	nirc->flow_ident = (uint32_t)unic->src_port;
	nirc->flow_window_scale = unic->flow_window_scale;
	nirc->flow_max_window = unic->flow_max_window;
	nirc->flow_end = unic->flow_end;
	nirc->flow_max_end = unic->flow_max_end;
	memcpy(nirc->flow_mac, unic->src_mac, 6);

	nirc->return_interface_num = unic->dest_interface_num;
	nirc->return_ip[0] = unic->dest_ip[0];
	nirc->return_ip[1] = unic->dest_ip[1];
	nirc->return_ip[2] = unic->dest_ip[2];
	nirc->return_ip[3] = unic->dest_ip[3];
	nirc->return_ident = (uint32_t)unic->dest_port;
	nirc->return_window_scale = unic->return_window_scale;
	nirc->return_max_window = unic->return_max_window;
	nirc->return_end = unic->return_end;
	nirc->return_max_end = unic->return_max_end;
	memcpy(nirc->return_mac, unic->dest_mac, 6);

	nirc->flags = 0;
	if (unic->flags & UBICOM_NA_IPV6_CREATE_FLAG_NO_SEQ_CHECK) {
		nirc->flags |= NA_IPV6_RULE_CREATE_FLAG_NO_SEQ_CHECK;
	}

	txdesc = &nim->tx_desc[i];
	txdesc->status = 0;
	txdesc->interface_num = 0;
	txdesc->opaque = (uint32_t)skb;
	txdesc->buffer = virt_to_phys(skb->head);
	txdesc->buffer_len = skb->end - skb->head;
	txdesc->payload_offs = skb->data - skb->head;
	txdesc->payload_len = skb->len;

	/*
	 * We are about to hand the new descriptor entry to the NA so
	 * we must not use txdesc from here onwards.  We set it to NULL, not
	 * because it does anything useful (and in fact for code that doesn't
	 * try to use txdesc again the compiler will eliminate this setting
	 * operation as "unused" in optimized builds) but if anyone adds a bad
	 * patch in the future that attempted to use txdesc it will trigger
	 * a segfault :-)
	 */
	txdesc = NULL;

	/*
	 * Update our host index so the NA sees we've written a new descriptor.
	 */
	nim->tx_host_index = ni;

	/*
	 * Kick the NA awake so it can process our new entry.
	 */
	ubi32_na_interrupt_na(nim);
	spin_unlock_bh(&ubi32_na.lock);
}

/*
 * ubicom_na_destroy_ipv6_rule()
 *	Destroy the given connection in the NA
 */
void ubicom_na_destroy_ipv6_rule(struct ubicom_na_ipv6_destroy *unid)
{
	struct sk_buff *skb;
	struct na_if_map *nim = ubi32_na.regs;
	uint32_t i;
	uint32_t ni;
	struct na_desc *txdesc;
	struct na_tx_metadata_object *ntmo;
	unsigned long ntmo_addr;
	struct na_ipv6_rule_destroy *nird;

	na_debug("Destroy IPv6: %pI6:%d, %pI6:%d, p: %d\n",
		unid->src_ip, unid->src_port, unid->dest_ip, unid->dest_port, unid->protocol);
	skb = __dev_alloc_skb(NA_SKB_PAYLOAD_SIZE, GFP_ATOMIC | __GFP_NOWARN);
	if (unlikely(!skb)) {
		spin_lock_bh(&ubi32_na.lock);
		ubi32_na.skb_alloc_err++;
		spin_unlock_bh(&ubi32_na.lock);
// XXX - use a return code?
		return;
	}

	spin_lock_bh(&ubi32_na.lock);
	i = nim->tx_host_index;
	ni = (i + 1) & (NA_NUM_TX_DESCRIPTORS - 1);

	if (unlikely(ni == nim->tx_na_index)) {
		/*
		 * We're not able to transmit right now!
		 */
		nim->int_status &= ~NA_IF_MAP_INT_STATUS_TX;
		ubi32_na.tx_q_full_cnt++;
		spin_unlock_bh(&ubi32_na.lock);
		dev_kfree_skb_any(skb);
		na_info("unable to enqueue 'IPv6 destroy rule' - marked as stopped\n");
		return;
	}

	ntmo_addr = (unsigned long)skb_put(skb, sizeof(struct na_tx_metadata_object));
	ntmo = (struct na_tx_metadata_object *)ntmo_addr;
	ubi32_na_write_allocate_buffer(ntmo, sizeof(struct na_tx_metadata_object));
	ntmo->type = NA_TX_METADATA_TYPE_IPV6_RULE_DESTROY;

	nird = &ntmo->sub.ipv6_rule_destroy;
	nird->protocol = (uint8_t)unid->protocol;
	nird->flow_ip[0] = unid->src_ip[0];
	nird->flow_ip[1] = unid->src_ip[1];
	nird->flow_ip[2] = unid->src_ip[2];
	nird->flow_ip[3] = unid->src_ip[3];
	nird->flow_ident = (uint32_t)unid->src_port;
	nird->return_ip[0] = unid->dest_ip[0];
	nird->return_ip[1] = unid->dest_ip[1];
	nird->return_ip[2] = unid->dest_ip[2];
	nird->return_ip[3] = unid->dest_ip[3];
	nird->return_ident = (uint32_t)unid->dest_port;

	txdesc = &nim->tx_desc[i];
	txdesc->status = 0;
	txdesc->interface_num = 0;
	txdesc->opaque = (uint32_t)skb;
	txdesc->buffer = virt_to_phys(skb->head);
	txdesc->buffer_len = skb->end - skb->head;
	txdesc->payload_offs = skb->data - skb->head;
	txdesc->payload_len = skb->len;

	/*
	 * We are about to hand the new descriptor entry to the NA so
	 * we must not use txdesc from here onwards.  We set it to NULL, not
	 * because it does anything useful (and in fact for code that doesn't
	 * try to use txdesc again the compiler will eliminate this setting
	 * operation as "unused" in optimized builds) but if anyone adds a bad
	 * patch in the future that attempted to use txdesc it will trigger
	 * a segfault :-)
	 */
	txdesc = NULL;

	/*
	 * Update our host index so the NA sees we've written a new descriptor.
	 */
	nim->tx_host_index = ni;

	/*
	 * Kick the NA awake so it can process our new entry.
	 */
	ubi32_na_interrupt_na(nim);
	spin_unlock_bh(&ubi32_na.lock);
}

/*
 * ubicom_na_interface_number_get()
 *	Return the interface number of the NA net_device.
 *
 * Returns -1 on failure or the interface number of dev is an NA net_device.
 */
int32_t ubicom_na_interface_number_get(struct net_device *dev)
{
	struct ubi32_na_private *priv;
	priv = netdev_priv(dev);
	if (priv->magic != UBICOM_NA_NETDEV_MAGIC) {
		return -1;
	}
	return priv->interface_num;
}

module_init(ubi32_na_init_module);
module_exit(ubi32_na_cleanup);

EXPORT_SYMBOL(ubicom_na_net_dev_register_ipv4_mgr);
EXPORT_SYMBOL(ubicom_na_net_dev_unregister_ipv4_mgr);
EXPORT_SYMBOL(ubicom_na_net_dev_register_ipv6_mgr);
EXPORT_SYMBOL(ubicom_na_net_dev_unregister_ipv6_mgr);
EXPORT_SYMBOL(ubicom_na_create_ipv4_rule);
EXPORT_SYMBOL(ubicom_na_destroy_ipv4_rule);
EXPORT_SYMBOL(ubicom_na_create_ipv6_rule);
EXPORT_SYMBOL(ubicom_na_destroy_ipv6_rule);
EXPORT_SYMBOL(ubicom_na_interface_number_get);

MODULE_AUTHOR("Ubicom, Inc.");
MODULE_LICENSE("GPL");

