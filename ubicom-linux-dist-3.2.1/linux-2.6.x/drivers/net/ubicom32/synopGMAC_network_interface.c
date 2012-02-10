/* \file
 * This is the network dependent layer to handle network related functionality.
 * This file is tightly coupled to neworking frame work of linux 2.6.xx kernel.
 * The functionality carried out in this file should be treated as an example only
 * if the underlying operating system is not Linux.
 *
 * \note Many of the functions other than the device specific functions
 *  changes for operating system other than Linux 2.6.xx
 * \internal
 *-----------------------------REVISION HISTORY-----------------------------------
 * Ubicom		01/Mar/2010			Modified for Ubicom32
 * Synopsys		01/Aug/2007			Created
 */

#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#include "synopGMAC_plat.h"
#include "synopGMAC_Dev.h"
#include "synopGMAC_network_interface.h"

#define IOCTL_READ_REGISTER  SIOCDEVPRIVATE+1
#define IOCTL_WRITE_REGISTER SIOCDEVPRIVATE+2
#define IOCTL_READ_IPSTRUCT  SIOCDEVPRIVATE+3
#define IOCTL_READ_RXDESC    SIOCDEVPRIVATE+4
#define IOCTL_READ_TXDESC    SIOCDEVPRIVATE+5
#define IOCTL_POWER_DOWN     SIOCDEVPRIVATE+6

/*
 * These are the global pointers for their respecive structures
 */
static synopGMACdevice *synopGMACDev[2] __attribute__((section(".ocm_data"))) = {NULL, NULL};

//#define GMAC_UBICOM_PROFILER
#ifdef GMAC_UBICOM_PROFILER
#include <asm/profile.h>
static volatile int int_count __attribute__((section(".ocm_data"))) = 0;
static volatile int rx_desc_check __attribute__((section(".ocm_data"))) = 0;
static volatile int rx_frame __attribute__((section(".ocm_data"))) = 0;
static volatile int rx_poll __attribute__((section(".ocm_data"))) = 0;
static volatile int tx_desc_check __attribute__((section(".ocm_data"))) = 0;
static volatile int tx_frame __attribute__((section(".ocm_data"))) = 0;
static volatile int tx_poll __attribute__((section(".ocm_data"))) = 0;
static volatile int tx_frame_fragmented __attribute__((section(".ocm_data"))) = 0;
#define GMAC_UBICOM_PROFILER_VCOUNT_INC(x) ((x)++)
#else
#define GMAC_UBICOM_PROFILER_VCOUNT_INC(x)
#endif

/*
 * Sample Wake-up frame filter configurations
 */
u32 synopGMAC_wakeup_filter_config0[] = {
	0x00000000,	// For Filter0 CRC is not computed may be it is 0x0000
	0x00000000,	// For Filter1 CRC is not computed may be it is 0x0000
	0x00000000,	// For Filter2 CRC is not computed may be it is 0x0000
	0x5F5F5F5F,     // For Filter3 CRC is based on 0,1,2,3,4,6,8,9,10,11,12,14,16,17,18,19,20,22,24,25,26,27,28,30 bytes from offset
	0x09000000,     // Filter 0,1,2 are disabled, Filter3 is enabled and filtering applies to only multicast packets
	0x1C000000,     // Filter 0,1,2 (no significance), filter 3 offset is 28 bytes from start of Destination MAC address
	0x00000000,     // No significance of CRC for Filter0 and Filter1
	0xBDCC0000      // No significance of CRC for Filter2, Filter3 CRC is 0xBDCC
};
u32 synopGMAC_wakeup_filter_config1[] = {
	0x00000000,	// For Filter0 CRC is not computed may be it is 0x0000
	0x00000000,	// For Filter1 CRC is not computed may be it is 0x0000
	0x7A7A7A7A,	// For Filter2 CRC is based on 1,3,4,5,6,9,11,12,13,14,17,19,20,21,25,27,28,29,30 bytes from offset
	0x00000000,     // For Filter3 CRC is not computed may be it is 0x0000
	0x00010000,     // Filter 0,1,3 are disabled, Filter2 is enabled and filtering applies to only unicast packets
	0x00100000,     // Filter 0,1,3 (no significance), filter 2 offset is 16 bytes from start of Destination MAC address
	0x00000000,     // No significance of CRC for Filter0 and Filter1
	0x0000A0FE      // No significance of CRC for Filter3, Filter2 CRC is 0xA0FE
};
u32 synopGMAC_wakeup_filter_config2[] = {
	0x00000000,	// For Filter0 CRC is not computed may be it is 0x0000
	0x000000FF,	// For Filter1 CRC is computed on 0,1,2,3,4,5,6,7 bytes from offset
	0x00000000,	// For Filter2 CRC is not computed may be it is 0x0000
	0x00000000,     // For Filter3 CRC is not computed may be it is 0x0000
	0x00000100,     // Filter 0,2,3 are disabled, Filter 1 is enabled and filtering applies to only unicast packets
	0x0000DF00,     // Filter 0,2,3 (no significance), filter 1 offset is 223 bytes from start of Destination MAC address
	0xDB9E0000,     // No significance of CRC for Filter0, Filter1 CRC is 0xDB9E
	0x00000000      // No significance of CRC for Filter2 and Filter3
};

/*
 * The synopGMAC_wakeup_filter_config3[] is a sample configuration for wake up filter.
 * Filter1 is used here
 * Filter1 offset is programmed to 50 (0x32)
 * Filter1 mask is set to 0x000000FF, indicating First 8 bytes are used by the filter
 * Filter1 CRC= 0x7EED this is the CRC computed on data 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55
 *
 * Refer accompanied software DWC_gmac_crc_example.c for CRC16 generation and how to use the same.
 */
u32 synopGMAC_wakeup_filter_config3[] = {
	0x00000000,	// For Filter0 CRC is not computed may be it is 0x0000
	0x000000FF,	// For Filter1 CRC is computed on 0,1,2,3,4,5,6,7 bytes from offset
	0x00000000,	// For Filter2 CRC is not computed may be it is 0x0000
	0x00000000,     // For Filter3 CRC is not computed may be it is 0x0000
	0x00000100,     // Filter 0,2,3 are disabled, Filter 1 is enabled and filtering applies to only unicast packets
	0x00003200,     // Filter 0,2,3 (no significance), filter 1 offset is 50 bytes from start of Destination MAC address
	0x7eED0000,     // No significance of CRC for Filter0, Filter1 CRC is 0x7EED,
	0x00000000      // No significance of CRC for Filter2 and Filter3
};

/*
 * dma_map_single_buf()
 *	Data cache flush/invalidate op for frame data buffer.
 *
 *	Only data flush is performed. The sync is not needed
 *	because it is always followed by a descriptor flush
 *	with sync.
 */
static void dma_map_single_buf(void * addr, u32 len)
{
	u32 begin_addr, end_addr, op_addr;
	u32 byte_count;
	u32 *a_tmp, d_tmp;

	/*
	 * Calculate the cache lines we need to operate from begin_addr to end_addr.
	 * The size is also re-calculated.
	 */
	begin_addr = (u32)addr & ~(CACHE_LINE_SIZE - 1);
	end_addr = ((u32)addr + len + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
	op_addr = end_addr;
	byte_count = end_addr - begin_addr;

#if (CACHE_LINE_SIZE != 32)
#error "Let me know when cache line size changed"
#endif
	asm volatile (
	"	lsr.4		%[n], %[n], #5			\n\t"	// n = number of cache lines to be flushed
	"	jmpeq.w.f	10f				\n\t"	// zero check

	"	sub.4		%[call_off], #0, %[n]		\n\t"
	"	and.4		%[call_off], #63, %[call_off]	\n\t"	// D15 = (2^N - 1) & -(n) where N = 6 here
	"	call		%[call_addr], .+4		\n\t"
	"	lea.4		%[call_addr], (%[call_addr], %[call_off])	\n\t"
	"	calli		%[call_addr], 8(%[call_addr])	\n\t"

	"1:	.rept		64				\n\t"	// repetition number: 2^N = 64 for N = 6
	"	flush		-32(%[addr])++			\n\t"
	"	.endr						\n\t"
	"	add.4		%[n], #-64, %[n]		\n\t"
	"	jmpgt.w.f	1b				\n\t"

	"10:							\n\t"
		: [addr]"+a" (op_addr), [n]"+d" (byte_count), [call_addr]"=&a" (a_tmp), [call_off]"=&d" (d_tmp)
		:
		: "cc", "memory"
	);
}

#if !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
/*
 * dma_unmap_single_buf()
 *	Data cache flush/invalidate op for frame data buffer.
 *
 *	Only data flush is performed. The sync is not needed
 *	because it is always followed by a descriptor flush
 *	with sync.
 */
void dma_unmap_single_buf(void * addr, u32 len)
{
	u32 begin_addr, end_addr, op_addr;
	u32 byte_count;
	u32 *a_tmp, d_tmp;

	/*
	 * Calculate the cache lines we need to operate from begin_addr to end_addr.
	 * The size is also re-calculated.
	 */
	begin_addr = (u32)addr & ~(CACHE_LINE_SIZE - 1);
	end_addr = ((u32)addr + len + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
	op_addr = end_addr;
	byte_count = end_addr - begin_addr;

#if (CACHE_LINE_SIZE != 32)
#error "Let me know when cache line size changed"
#endif
	asm volatile (
	"	lsr.4		%[n], %[n], #5			\n\t"	// n = number of cache lines to be flushed
	"	jmpeq.w.f	10f				\n\t"	// zero check

	"	sub.4		%[call_off], #0, %[n]		\n\t"
	"	and.4		%[call_off], #63, %[call_off]	\n\t"	// D15 = (2^N - 1) & -(n) where N = 6 here
	"	call		%[call_addr], .+4		\n\t"
	"	lea.4		%[call_addr], (%[call_addr], %[call_off])	\n\t"
	"	calli		%[call_addr], 8(%[call_addr])	\n\t"

	"1:	.rept		64				\n\t"	// repetition number: 2^N = 64 for N = 6
	"	inval		-32(%[addr])++			\n\t"
	"	.endr						\n\t"
	"	add.4		%[n], #-64, %[n]		\n\t"
	"	jmpgt.w.f	1b				\n\t"

	"10:							\n\t"
		: [addr]"+a" (op_addr), [n]"+d" (byte_count), [call_addr]"=&a" (a_tmp), [call_off]"=&d" (d_tmp)
		:
		: "cc", "memory"
	);
}
#endif

/*
 * Function used to detect the cable plugging and unplugging.
 * This function gets scheduled once in every second and polls
 * the PHY register for network cable plug/unplug. Once the
 * connection is back the GMAC device is configured as per
 * new Duplex mode and Speed of the connection.
 * @param[in] u32 type but is not used currently.
 * \return returns void.
 * \note This function is tightly coupled with Linux 2.6.xx.
 * \callgraph
 */
static void synopGMAC_linux_cable_unplug_function(u32 arg)
{
	synopGMACdevice *gmacdev = (synopGMACdevice *)arg;
	struct net_device *netdev = gmacdev->synopGMACnetdev;
	struct ethtionode *ethNode = gmacdev->ethdn;

	u16 data = ethNode->status;
	if ((data & UBI32_ETH_VP_STATUS_LINK) == 0) {
		if (gmacdev->LinkState == LINKUP) {
			TR("Link down\n");
			gmacdev->LinkState = LINKDOWN;
			gmacdev->DuplexMode = 0;
			gmacdev->Speed = 0;
			gmacdev->LoopBackMode = NOLOOPBACK;
			netif_carrier_off(netdev);
		}
	} else {
		if (gmacdev->LinkState != LINKUP) {
			TR("Link UP: %08x\n",ethNode->status);
			gmacdev->LinkState = LINKUP;
			//status = synopGMAC_check_phy_init(gmacdev);
			gmacdev->DuplexMode = (data & UBI32_ETH_VP_STATUS_DUPLEX)  ? FULLDUPLEX: HALFDUPLEX ;
			if (data & UBI32_ETH_VP_STATUS_SPEED1000)
				gmacdev->Speed      =   SPEED1000;
			else if (data & UBI32_ETH_VP_STATUS_SPEED100)
				gmacdev->Speed      =   SPEED100;
			else
				gmacdev->Speed      =   SPEED10;
			gmacdev->LoopBackMode = NOLOOPBACK;
			synopGMAC_mac_init(gmacdev);
			netif_carrier_on(netdev);
		}
	}

	gmacdev->synopGMAC_cable_unplug_timer.expires = CHECK_TIME + jiffies;
	add_timer(&gmacdev->synopGMAC_cable_unplug_timer);
}

static void synopGMAC_linux_powerdown_mac(synopGMACdevice *gmacdev)
{
	TR0("Put the GMAC to power down mode..\n");
	// Disable the Dma engines in tx path
	gmacdev->GMAC_Power_down = 1;	// Let ISR know that Mac is going to be in the power down mode
	synopGMAC_disable_dma_tx(gmacdev);
	plat_delay(10000);		//allow any pending transmission to complete
	// Disable the Mac tx
	synopGMAC_tx_disable(gmacdev);

	// Disable the Mac rx
	synopGMAC_rx_disable(gmacdev);
	plat_delay(10000); 		//Allow any pending buffer to be read by host
	//Disable the Dma in rx path
	synopGMAC_disable_dma_rx(gmacdev);

	//enable the power down mode
	//synopGMAC_pmt_unicast_enable(gmacdev);

	//prepare the gmac for magic packet reception and wake up frame reception
	synopGMAC_magic_packet_enable(gmacdev);
	synopGMAC_write_wakeup_frame_register(gmacdev, synopGMAC_wakeup_filter_config3);

	synopGMAC_wakeup_frame_enable(gmacdev);

	//gate the application and transmit clock inputs to the code. This is not done in this driver :).

	//enable the Mac for reception
	synopGMAC_rx_enable(gmacdev);

	//Enable the assertion of PMT interrupt
	synopGMAC_pmt_int_enable(gmacdev);
	//enter the power down mode
	synopGMAC_power_down_enable(gmacdev);
}

static void synopGMAC_linux_powerup_mac(synopGMACdevice *gmacdev)
{
	gmacdev->GMAC_Power_down = 0;	// Let ISR know that MAC is out of power down now
	if (synopGMAC_is_magic_packet_received(gmacdev))
		TR("GMAC wokeup due to Magic Pkt Received\n");
	if (synopGMAC_is_wakeup_frame_received(gmacdev))
		TR("GMAC wokeup due to Wakeup Frame Received\n");
	//Disable the assertion of PMT interrupt
	synopGMAC_pmt_int_disable(gmacdev);
	//Enable the mac and Dma rx and tx paths
	synopGMAC_rx_enable(gmacdev);
       	synopGMAC_enable_dma_rx(gmacdev);

	synopGMAC_tx_enable(gmacdev);
	synopGMAC_enable_dma_tx(gmacdev);
}

/*
 * This sets up the transmit Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Device is interested only after the descriptors are setup. Therefore this function
 * is not included in the device driver API. This function should be treated as an
 * example code to design the descriptor structures for ring mode or chain mode.
 * This function depends on the device structure for allocation consistent dma-able memory in case of linux.
 *	- Allocates the memory for the descriptors.
 *	- Initialize the Busy and Next descriptors indices to 0(Indicating first descriptor).
 *	- Initialize the Busy and Next descriptors to first descriptor address.
 * 	- Initialize the last descriptor with the endof ring in case of ring mode.
 *	- Initialize the descriptors in chain mode.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to device structure.
 * @param[in] number of descriptor expected in tx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note This function fails if allocation fails for required number of descriptors in Ring mode, but in chain mode
 *  function returns -ESYNOPGMACNOMEM in the process of descriptor chain creation. once returned from this function
 *  user should for gmacdev->TxDescCount to see how many descriptors are there in the chain. Should continue further
 *  only if the number of descriptors in the chain meets the requirements
 */
static s32 synopGMAC_setup_tx_desc_queue(synopGMACdevice * gmacdev,struct device * dev,u32 no_of_desc, u32 desc_mode)
{
	s32 i;
	DmaDesc *first_desc = NULL;
	dma_addr_t dma_addr;
	gmacdev->TxDescCount = 0;

	BUG_ON(desc_mode != RINGMODE);
	BUG_ON((no_of_desc & (no_of_desc - 1)) != 0);	// Must be power-of-2

	TR("Total size of memory required for Tx Descriptors in Ring Mode = 0x%08x\n",
		(u32)((sizeof(DmaDesc) * no_of_desc)));
	first_desc = plat_alloc_consistent_dmaable_memory(dev, sizeof(DmaDesc) * no_of_desc,&dma_addr);
	if (first_desc == NULL) {
		TR("Error in Tx Descriptors memory allocation\n");
		return -ESYNOPGMACNOMEM;
	}
	BUG_ON((int)first_desc & (CACHE_LINE_SIZE - 1));
	gmacdev->TxDescCount = no_of_desc;
	gmacdev->TxDesc      = first_desc;
	gmacdev->TxDescDma   = dma_addr;
	TR("Tx Descriptors in Ring Mode: No. of descriptors = %d base = 0x%08x dma = 0x%08x\n",
		no_of_desc, (u32)first_desc, dma_addr);

	for (i =0; i < gmacdev -> TxDescCount; i++) {
		synopGMAC_tx_desc_init_ring(gmacdev->TxDesc + i, i == (gmacdev->TxDescCount - 1));
		TR("%02d %08x \n",i, (unsigned int)(gmacdev->TxDesc + i) );
	}

	gmacdev->TxNext = 0;
	gmacdev->TxBusy = 0;
	gmacdev->TxNextDesc = gmacdev->TxDesc;
	gmacdev->TxBusyDesc = gmacdev->TxDesc;
	gmacdev->BusyTxDesc  = 0;

	return -ESYNOPGMACNOERR;
}

/*
 * This sets up the receive Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Device is interested only after the descriptors are setup. Therefore this function
 * is not included in the device driver API. This function should be treated as an
 * example code to design the descriptor structures in ring mode or chain mode.
 * This function depends on the device structure for allocation of consistent dma-able memory in case of linux.
 *	- Allocates the memory for the descriptors.
 *	- Initialize the Busy and Next descriptors indices to 0(Indicating first descriptor).
 *	- Initialize the Busy and Next descriptors to first descriptor address.
 * 	- Initialize the last descriptor with the endof ring in case of ring mode.
 *	- Initialize the descriptors in chain mode.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to device structure.
 * @param[in] number of descriptor expected in rx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note This function fails if allocation fails for required number of descriptors in Ring mode, but in chain mode
 *  function returns -ESYNOPGMACNOMEM in the process of descriptor chain creation. once returned from this function
 *  user should for gmacdev->RxDescCount to see how many descriptors are there in the chain. Should continue further
 *  only if the number of descriptors in the chain meets the requirements
 */
static s32 synopGMAC_setup_rx_desc_queue(synopGMACdevice * gmacdev,struct device * dev,u32 no_of_desc, u32 desc_mode)
{
	s32 i;
	DmaDesc *first_desc = NULL;
	dma_addr_t dma_addr;
	gmacdev->RxDescCount = 0;

	BUG_ON(desc_mode != RINGMODE);
	BUG_ON((no_of_desc & (no_of_desc - 1)) != 0);	// Must be power-of-2

	TR("total size of memory required for Rx Descriptors in Ring Mode = 0x%08x\n",
		(u32)((sizeof(DmaDesc) * no_of_desc)));
	first_desc = plat_alloc_consistent_dmaable_memory (dev, sizeof(DmaDesc) * no_of_desc, &dma_addr);
	if (first_desc == NULL) {
		TR("Error in Rx Descriptor Memory allocation in Ring mode\n");
		return -ESYNOPGMACNOMEM;
	}
	BUG_ON((int)first_desc & (CACHE_LINE_SIZE - 1));
	gmacdev->RxDescCount = no_of_desc;
	gmacdev->RxDesc      = first_desc;
	gmacdev->RxDescDma   = dma_addr;
	TR("Rx Descriptors in Ring Mode: No. of descriptors = %d base = 0x%08x dma = 0x%08x\n",
		no_of_desc, (u32)first_desc, dma_addr);

	for (i =0; i < gmacdev -> RxDescCount; i++) {
		synopGMAC_rx_desc_init_ring(gmacdev->RxDesc + i, i == (gmacdev->RxDescCount - 1));
		TR("%02d %08x \n",i, (unsigned int)(gmacdev->RxDesc + i));
	}

	gmacdev->RxNext = 0;
	gmacdev->RxBusy = 0;
	gmacdev->RxNextDesc = gmacdev->RxDesc;
	gmacdev->RxBusyDesc = gmacdev->RxDesc;
	gmacdev->BusyRxDesc   = 0;

	return -ESYNOPGMACNOERR;
}

/*
 * This gives up the receive Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Once device's Dma is stopped the memory descriptor memory and the buffer memory deallocation,
 * is completely handled by the operating system, this call is kept outside the device driver Api.
 * This function should be treated as an example code to de-allocate the descriptor structures in ring mode or chain mode
 * and network buffer deallocation.
 * This function depends on the device structure for dma-able memory deallocation for both descriptor memory and the
 * network buffer memory under linux.
 * The responsibility of this function is to
 *     - Free the network buffer memory if any.
 *	- Fee the memory allocated for the descriptors.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to device structure.
 * @param[in] number of descriptor expected in rx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note No referece should be made to descriptors once this function is called. This function is invoked when the device is closed.
 */
static void synopGMAC_giveup_rx_desc_queue(synopGMACdevice * gmacdev, struct device *dev, u32 desc_mode)
{
	s32 i;
	u32 status;
	dma_addr_t dma_addr1;
	u32 length1;
	u32 data1;

	for (i = 0; i < gmacdev->RxDescCount; i++) {
		synopGMAC_get_desc_data(gmacdev->RxDesc + i, &status, &dma_addr1, &length1, &data1);
		if ((length1 != 0) && (data1 != 0)) {
			//dma_unmap_single_buf(dma_addr1, 0);		// not needed
			dev_kfree_skb_any((struct sk_buff *) data1);	// free buffer1
			TR("(Ring mode) rx buffer1 %08x of size %d from %d rx descriptor is given back\n", data1, length1, i);
		}
	}
	plat_free_consistent_dmaable_memory(dev, (sizeof(DmaDesc) * gmacdev->RxDescCount), gmacdev->RxDesc, gmacdev->RxDescDma);
	TR("Memory allocated %08x  for Rx Desriptors (ring) is given back\n", (u32)gmacdev->RxDesc);
	TR("rx-------------------------------------------------------------------rx\n");

	gmacdev->RxDesc    = NULL;
	gmacdev->RxDescDma = 0;
}

/*
 * This gives up the transmit Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Once device's Dma is stopped the memory descriptor memory and the buffer memory deallocation,
 * is completely handled by the operating system, this call is kept outside the device driver Api.
 * This function should be treated as an example code to de-allocate the descriptor structures in ring mode or chain mode
 * and network buffer deallocation.
 * This function depends on the device structure for dma-able memory deallocation for both descriptor memory and the
 * network buffer memory under linux.
 * The responsibility of this function is to
 *     - Free the network buffer memory if any.
 *	- Fee the memory allocated for the descriptors.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to device structure.
 * @param[in] number of descriptor expected in tx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note No reference should be made to descriptors once this function is called. This function is invoked when the device is closed.
 */
static void synopGMAC_giveup_tx_desc_queue(synopGMACdevice * gmacdev,struct device * dev, u32 desc_mode)
{
	s32 i;
	u32 status;
	dma_addr_t dma_addr1;
	u32 length1;
	u32 data1;

	for (i = 0; i < gmacdev->TxDescCount; i++) {
		synopGMAC_get_desc_data(gmacdev->TxDesc + i, &status, &dma_addr1, &length1, &data1);
		if((length1 != 0) && (data1 != 0)){
			//dma_unmap_single_buf(dma_addr1, 0);		// not needed
			dev_kfree_skb_any((struct sk_buff *) data1);	// free buffer1
			TR("(Ring mode) tx buffer1 %08x of size %d from %d rx descriptor is given back\n", data1, length1, i);
		}
	}
	plat_free_consistent_dmaable_memory(dev, (sizeof(DmaDesc) * gmacdev->TxDescCount), gmacdev->TxDesc, gmacdev->TxDescDma);
	TR("Memory allocated %08x for Tx Desriptors (ring) is given back\n", (u32)gmacdev->TxDesc);
	TR("tx-------------------------------------------------------------------tx\n");

	gmacdev->TxDesc    = NULL;
	gmacdev->TxDescDma = 0;
}

/*
 * Function to handle housekeeping after a packet is transmitted over the wire.
 * After the transmission of a packet DMA generates corresponding interrupt
 * (if it is enabled). It takes care of returning the sk_buff to the linux
 * kernel, updating the networking statistics and tracking the descriptors.
 * @param[in] pointer to net_device structure.
 * \return void.
 * \note This function runs in interrupt context
 */
static void synopGMAC_handle_transmit_over(struct net_device *netdev)
{
	synopGMACdevice *gmacdev = netdev_priv(netdev);
	DmaDesc *txdesc;
	u32 status;
	u32 data1;
	u32 length1;
	u32 dma_addr1;
#ifdef ENH_DESC_8W
	u32 ext_status;
#ifdef ENH_DESC_TIMESTAMP
	u32 time_stamp_high;
	u32 time_stamp_low;
	u16 time_stamp_higher;
#endif
#endif

	/*
	 * Handle the transmit Descriptors
	 */
	GMAC_UBICOM_PROFILER_VCOUNT_INC(tx_poll);
	while (1) {
		GMAC_UBICOM_PROFILER_VCOUNT_INC(tx_desc_check);
		txdesc = synopGMAC_get_tx_qptr(gmacdev);
		if (unlikely(txdesc == NULL)) {
			break;
		}

		status = txdesc->status;
#ifdef ENH_DESC_8W
		ext_status = txdesc->extstatus;
#ifdef ENH_DESC_TIMESTAMP
		synopGMAC_TS_read_timestamp_higher_val(gmacdev, &time_stamp_higher);
		time_stamp_high = txdesc->timestamphigh;
		time_stamp_low = txdesc->timestamplow;
#endif
#endif

		dma_addr1 = txdesc->buffer1;
		length1 = (txdesc->length & DescSize1Mask) >> DescSize1Shift;
		data1 = txdesc->data1;

		TR("%02d %08x %08x %08x %08x %08x\n",gmacdev->TxBusy,(u32)txdesc,status,txdesc->length,dma_addr1,data1);
		synopGMAC_reset_tx_qptr(gmacdev);
		netdev->stats.tx_bytes += length1;

		/*
		 * Only the descriptor of the last segment contains skb in data1
		 */
		if (!data1) {
			continue;
		}

#ifdef	IPC_OFFLOAD
		if (unlikely(synopGMAC_is_tx_ipv4header_checksum_error(gmacdev, status))) {
			TR0("Harware Failed to Insert IPV4 Header Checksum\n");
		}
		if (unlikely(synopGMAC_is_tx_payload_checksum_error(gmacdev, status))) {
			TR0("Harware Failed to Insert Payload Checksum\n");
		}
#endif

		GMAC_UBICOM_PROFILER_VCOUNT_INC(tx_frame);
		asm volatile ("prefetch (%0)" : : "a"(&((struct sk_buff *)data1)->data));
		//dma_unmap_single_buf(dma_addr1, length1);	// Not needed.
		dev_kfree_skb_any((struct sk_buff *)data1);

		if (likely(synopGMAC_is_desc_valid(status))) {
			netdev->stats.tx_packets++;
		}
		else {
			TR0("Error in Status %08x\n",status);
			netdev->stats.tx_errors++;
			netdev->stats.tx_aborted_errors += synopGMAC_is_tx_aborted(status);
			netdev->stats.tx_carrier_errors += synopGMAC_is_tx_carrier_error(status);
		}
		netdev->stats.collisions += synopGMAC_get_tx_collision_count(status);
	}
	netif_wake_queue(netdev);
}

//#define GMAC_CRC_DOUBLE_CHECK 1
#ifdef GMAC_CRC_DOUBLE_CHECK
/*
 * synocGMAC_check_crc32()
 *	Calculate the Ethernet CRC32 for the entire skbuff.
 */
u32 synocGMAC_check_crc32(void *data, int len)
{
	u32_t crc32 = 0xffffffff;

	asm volatile (
	"	sub.2		d15, #0, %2		\n\t" /* Zero length check */
	"	jmpeq.s.f	10f			\n\t"
	"	and.4		d15, #(64-1), d15	\n\t" /* d15 = (-len) & (2^N - 1) */

	"	move.4		-4(SP)++, MAC_LO	\n\t"
	"	move.4		-4(SP)++, MAC_HI	\n\t"
	"	swapb.4		MAC_LO, %0		\n\t" /* MAC_LO = Reverse the initial CRC */

	"	call		a3, .+4			\n\t"
	"	lea.4		a3, (a3,d15)		\n\t"
	"	calli		a3, 8(a3)		\n\t"

	"1:	.rept		64			\n\t" /* repeat for 2^N bytes */
	"	crcgen		(%1)1++, %3		\n\t"
	"	.endr					\n\t"
	"	add.4		%2, #-64, %2		\n\t" /* len -= 2^N */
	"	jmpgt.w.t	1b			\n\t"

	"	swapb.4		%0, MAC_LO		\n\t" /* Reverse the result CRC */
	"	move.4		MAC_HI, (SP)4++		\n\t"
	"	move.4		MAC_LO, (SP)4++		\n\t"
	"10:						\n\t"

		: "+&d" (crc32), "+a" (data), "+d" (len)
		: "d" (0xEDB88320)
		: "d15", "a3"//, "MAC_HI", "MAC_LO"
	);

	BUG_ON (~crc32 != 0x1cdf4421);
	return ~crc32;
}
#endif

/*
 * Function to Receive a packet from the interface.
 * After Receiving a packet, DMA transfers the received packet to the system memory
 * and generates corresponding interrupt (if it is enabled). This function prepares
 * the sk_buff for received packet after removing the ethernet CRC, and hands it over
 * to linux networking stack.
 * 	- Updataes the networking interface statistics
 *	- Keeps track of the rx descriptors
 * @param[in] pointer to net_device structure.
 * \return void.
 * \note This function runs in interrupt context.
 */
static u32 synopGMAC_handle_received_data(struct net_device *netdev, u32 quota)
{
	synopGMACdevice *gmacdev = netdev_priv(netdev);
	DmaDesc *rxdesc;
	u32 status;
	u32 len;
	u32 data1;
	u32 dma_addr1;
#ifdef ENH_DESC_8W
	u32 ext_status;
#ifdef ENH_DESC_TIMESTAMP
	u32 time_stamp_high;
	u32 time_stamp_low;
	u16 time_stamp_higher;
#endif
#endif
	struct sk_buff *skb;
	u32 count = 0, refill_count = 0, reserve_len;

	/*
	 * Handle the Receive Descriptors
	 */
	GMAC_UBICOM_PROFILER_VCOUNT_INC(rx_poll);
	while (count < quota) {
		GMAC_UBICOM_PROFILER_VCOUNT_INC(rx_desc_check);
		rxdesc = synopGMAC_get_rx_qptr(gmacdev);
		if (unlikely(rxdesc == NULL)) {
			break;
		}

		status = rxdesc->status;
#ifdef ENH_DESC_8W
		ext_status = rxdesc->extstatus;
#endif
		dma_addr1 = rxdesc->buffer1;
		data1 = rxdesc->data1;

#ifdef ENH_DESC_8W
#ifdef ENH_DESC_TIMESTAMP
		synopGMAC_TS_read_timestamp_higher_val(gmacdev, &time_stamp_higher);
		time_stamp_high = rxdesc->timestamphigh;
		time_stamp_low = rxdesc->timestamplow;
		TR("S:%08x ES:%08x DA1:%08x d1:%08x TSH:%08x TSL:%08x TSHW:%08x \n",
			status, ext_status, dma_addr1, data1, time_stamp_high, time_stamp_low, time_stamp_higher);
#endif
#endif
		TR("%02d %08x %08x %08x %08x %08x\n", gmacdev->RxBusy, (u32)rxdesc,status, rxdesc->length, dma_addr1, data1);
		synopGMAC_reset_rx_qptr(gmacdev);

		GMAC_UBICOM_PROFILER_VCOUNT_INC(rx_frame);
		BUG_ON(data1 == 0);

		/*
		 * Perform prefetch on skbuff structure data
		 */
		skb = (struct sk_buff *)data1;
		asm volatile ("prefetch (%0)" : : "a"(&skb->len));
		asm volatile ("prefetch (%0)" : : "a"(&skb->data));

		/*
		 * Validate RX frame:
		 *	The max frame size check is implied by buffer's limited length
		 *	and the presence of both first and last descritpor flags.
		 */
		len = synopGMAC_get_rx_desc_frame_length(status);
		if (unlikely(!synopGMAC_is_rx_desc_valid(status)) ||
		    unlikely(len < (ETH_ZLEN + ETH_FCS_LEN))) {
			/*
			 * At first step unmapped the dma address
			 */
			//dma_unmap_single_buf(dma_addr1, 0);

			/*
			 * Now the present skb should be set free
			 */
			dev_kfree_skb_any(skb);
			TR0("%s: Invalid RX status = %08x\n", __FUNCTION__, status);

			count++;
			netdev->stats.rx_errors++;
			netdev->stats.collisions       += synopGMAC_is_rx_frame_collision(status);
			netdev->stats.rx_crc_errors    += synopGMAC_is_rx_crc(status);
			netdev->stats.rx_frame_errors  += synopGMAC_is_frame_dribbling_errors(status);
			netdev->stats.rx_length_errors += synopGMAC_is_rx_frame_length_errors(status);

			continue;
		}
#ifdef GMAC_CRC_DOUBLE_CHECK
#if !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
		dma_unmap_single_buf(skb->data, len);	// invalidate receive buffer
#endif
		synocGMAC_check_crc32(skb->data, len);
#endif

		/*
		 * At first step unmapped the dma address
		 */
		len -= ETH_FCS_LEN; //Not interested in Ethernet CRC bytes
		//dma_unmap_single_buf(dma_addr1, len);	// Not needed
		__skb_put(skb, len);

		/*
		 * Perform prefetch on RX frame header
		 */
		asm volatile ("prefetch (%0)" : : "a"(skb->data));
		//asm volatile ("prefetch 32(%0)" : : "a"(skb->data));

#ifdef IPC_OFFLOAD
		/*
		 * Now lets check for the IPC offloading
		 * Since we have enabled the checksum offloading in hardware, lets inform the kernel
		 * not to perform the checksum computation on the incoming packet. Note that ip header
		 * checksum will be computed by the kernel immaterial of what we inform. Similary TCP/UDP/ICMP
		 * pseudo header checksum will be computed by the stack. What we can inform is not to perform
		 * payload checksum.
		 * When CHECKSUM_UNNECESSARY is set kernel bypasses the checksum computation.
		 */
		skb->ip_summed = CHECKSUM_UNNECESSARY;
		skb->dev = netdev;

#ifdef ENH_DESC_8W
		if (likely(synopGMAC_is_ext_status(gmacdev, status))) { // extended status present indicates that the RDES4 need to be probed
			if (unlikely(synopGMAC_ES_is_IP_header_error(gmacdev,ext_status) ||	// IP header (IPV4) checksum error
				     synopGMAC_ES_is_rx_checksum_bypassed(gmacdev,ext_status) ||// Hardware engine bypassed the checksum computation/checking
				     synopGMAC_ES_is_IP_payload_error(gmacdev,ext_status))) {	// IP payload checksum is in error (UDP/TCP/ICMP checksum error)
				TR0("(EXTSTS)Abnormal checksum status = 0x%8x\n", ext_status);
				skb->ip_summed = CHECKSUM_NONE;     //Let Kernel compute the checkssum
			}
		} else // No extended status. So relevant information is available in the status itself
#endif //ENH_DESC_8W
		{
			/*
			 * Rx Descriptor COE type2 encoding
			 * enum RxDescCOEEncode
			 *
			 * RxLenLT600                   = 0,    Bit(5:7:0)=>0 IEEE 802.3 type frame Length field is Lessthan 0x0600
			 * RxIpHdrPayLoadChkBypass      = 1,    Bit(5:7:0)=>1 Payload & Ip header checksum bypassed (unsuppported payload)
			 * RxIpHdrPayLoadRes            = 2,    Bit(5:7:0)=>2 Reserved
			 * RxChkBypass                  = 3,    Bit(5:7:0)=>3 Neither IPv4 nor IPV6. So checksum bypassed
			 * RxNoChkError                 = 4,    Bit(5:7:0)=>4 No IPv4/IPv6 Checksum error detected
			 * RxPayLoadChkError            = 5,    Bit(5:7:0)=>5 Payload checksum error detected for Ipv4/Ipv6 frames
			 * RxIpHdrChkError              = 6,    Bit(5:7:0)=>6 Ip header checksum error detected for Ipv4 frames
			 * RxIpHdrPayLoadChkError       = 7,    Bit(5:7:0)=>7 Payload & Ip header checksum error detected for Ipv4/Ipv6 frames
			 */
			u32 err_stratus = synopGMAC_is_rx_checksum_error(gmacdev, status);
			if (likely(err_stratus == DescRxChkBit5)) {
				// (DescRxChkBit5 && !DescRxChkBit7 && !DescRxChkBit0)
				TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 4>\n");
			} else if (err_stratus == 0) {
				// (!DescRxChkBit5 && !DescRxChkBit7 && !DescRxChkBit0)
				TR("IEEE 802.3 type frame with Length field Lesss than 0x0600 <Chk Status = 0>\n");
				skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
			} else {
				if (err_stratus == DescRxChkBit0) {
					TR0("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 1>\n");
					skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
				}
				else if (err_stratus == (DescRxChkBit7 | DescRxChkBit0)) {
					TR0("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 3>\n");
					skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
				}
				else if (err_stratus == (DescRxChkBit5 | DescRxChkBit0)) {
					TR0(" TCP/UDP payload checksum Error <Chk Status = 5>\n");
					skb->ip_summed = CHECKSUM_NONE;	//Let Kernel compute the Checksum
				}
				else if (err_stratus == (DescRxChkBit5 | DescRxChkBit7)) {
					//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
					TR0(" Error in 16bit IPV4 Header Checksum <Chk Status = 6>\n");
					skb->ip_summed = CHECKSUM_UNNECESSARY;	//Let Kernel bypass the TCP/UDP checksum computation
				}
				else if (err_stratus == (DescRxChkBit5 | DescRxChkBit7 | DescRxChkBit0)) {
					//Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
					TR0(" Both IP header and Payload Checksum Error <Chk Status = 7>\n");
					skb->ip_summed = CHECKSUM_NONE;	        //Let Kernel compute the Checksum
				} else {
					TR0(" Unknown IP header and Payload Checksum Error <Chk Status = 0x%08x> \n", err_stratus);
					skb->ip_summed = CHECKSUM_NONE;	        //Let Kernel compute the Checksum
				}
			}
		}
#endif //IPC_OFFLOAD

		count++;
		netdev->last_rx = jiffies;
		netdev->stats.rx_packets++;
		netdev->stats.rx_bytes += len;

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
		if (gmacdev->vlgrp) {
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
			__skb_pull(skb, VLAN_HLEN);

			skb->protocol = eth_type_trans(skb, netdev);
			vlan_hwaccel_receive_skb(skb,
				gmacdev->vlgrp, skb->vlan_tci);

			continue;
		}
#endif
		skb->protocol = eth_type_trans(skb, netdev);
		netif_receive_skb(skb);
	}

	/*
	 * Now allocate more RX buffer and let GMAC DMA engine know about them.
	 */
	reserve_len = ETHERNET_EXTRA;
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
	if (gmacdev->vlgrp) reserve_len -= 4;
#endif
	while (gmacdev->BusyRxDesc < gmacdev->RxDescCount) {
		skb = __dev_alloc_skb(ETHERNET_EXTRA + ETH_MAX_FRAME_LEN + CACHE_LINE_SIZE,
			GFP_ATOMIC | __GFP_NOWARN | __GFP_COLD);
		if(skb == NULL){
			TR("SKB memory allocation failed \n");
			break;
		}
		skb_reserve(skb, reserve_len);

		/*
		 * Flush and invaidate from skb->data to skb->data + ETH_MAX_FRAME_LEN - 1.
		 */
		dma_map_single_buf(skb->data, ETH_MAX_FRAME_LEN);
		dma_addr1 = virt_to_phys(skb->data);

		synopGMAC_set_rx_qptr(gmacdev, dma_addr1, ETH_MAX_FRAME_LEN, (u32)skb);
		refill_count++;
	}

	if (refill_count > 0) {
		synopGMAC_resume_dma_rx(gmacdev);
	}

	return count;
}

static inline void synopGMAC_ubicom32_enable_interrupt(synopGMACdevice *gmacdev)
{
	asm volatile(
	"	move.4	"D(IO_INT_MASK)"(%0), #-1	\n\t"
		:
		: "a" (gmacdev->NbrBase)
	);
}

static inline void synopGMAC_ubicom32_disable_interrupt(synopGMACdevice *gmacdev)
{
	asm volatile(
	"	move.4	"D(IO_INT_MASK)"(%0), #0	\n\t"
		:
		: "a" (gmacdev->NbrBase)
	);
}

static inline u32 synopGMAC_ubicom32_check_interrupt(synopGMACdevice *gmacdev)
{
	u32 int_status;
	asm volatile(
	"	move.4	%0, "D(IO_INT_STATUS)"(%1)	\n\t"
		: "=d" (int_status)
		: "a" (gmacdev->NbrBase)
	);
	return int_status;
}

/*
 * Interrupt service routing.
 * This is the function registered as ISR for device interrupts.
 * @param[in] interrupt number.
 * @param[in] void pointer to device unique structure (Required for shared interrupts in Linux).
 * @param[in] pointer to pt_regs (not used).
 * \return Returns IRQ_NONE if not device interrupts IRQ_HANDLED for device interrupts.
 * \note This function runs in interrupt context
 *
 */
static irqreturn_t synopGMAC_intr_handler(s32 intr_num, void * dev_id)
{
	/*Kernels passes the netdev structure in the dev_id. So grab it*/
	struct net_device *netdev;
	synopGMACdevice *gmacdev;
	u32 dma_status_reg;
	s32 status;

	netdev  = (struct net_device *) dev_id;
	BUG_ON(netdev == NULL);
        BUG_ON(intr_num != netdev->irq);

        gmacdev = netdev_priv(netdev);
        BUG_ON(gmacdev == NULL);
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	GMAC_UBICOM_PROFILER_VCOUNT_INC(int_count);
	TR("HW INT: status = %08x mask = %08x\n",
		*(u32 *)(gmacdev->NbrBase + IO_INT_STATUS),
		*(u32 *)(gmacdev->NbrBase + IO_INT_MASK));

	/*
	 * Read the Dma interrupt status
	 */
	dma_status_reg = synopGMACReadReg((u32 *)gmacdev->DmaBase, DmaStatus);
       	TR("%s:Dma Status Reg: 0x%08x\n", __FUNCTION__, dma_status_reg);

	if (unlikely(dma_status_reg == 0))
		return IRQ_NONE;

	if (unlikely(dma_status_reg & GmacPmtIntr)) {
		TR0("%s:: Interrupt due to PMT module\n", __FUNCTION__);
		synopGMAC_linux_powerup_mac(gmacdev);
	}

	if (unlikely(dma_status_reg & GmacMmcIntr)) {
		TR0("%s:: Interrupt due to MMC module\n", __FUNCTION__);
		TR("%s:: synopGMAC_rx_int_status = %08x\n", __FUNCTION__,
			synopGMAC_read_mmc_rx_int_status(gmacdev));
		TR("%s:: synopGMAC_tx_int_status = %08x\n", __FUNCTION__,
			synopGMAC_read_mmc_tx_int_status(gmacdev));
	}

	if (unlikely(dma_status_reg & GmacLineIntfIntr)) {
		TR("%s:: Interrupt due to GMAC LINE module\n",__FUNCTION__);
		TR("GMAC status reg is %08x mask is %08x\n",
			synopGMACReadReg((u32 *)gmacdev->MacBase, GmacInterruptStatus),
			synopGMACReadReg((u32 *)gmacdev->MacBase, GmacInterruptMask));
		if (synopGMACReadReg((u32 *)gmacdev->MacBase, GmacInterruptStatus) & GmacRgmiiIntSts) {
			status = synopGMACReadReg((u32 *)gmacdev->MacBase, 0x00D8);
			TR("GMAC RGMII status is %08x\n", status);
		}
	}

#ifdef UBICOM32_USE_NAPI
	if (napi_schedule_prep(&gmacdev->napi)) {
		//synopGMAC_disable_interrupt_all(gmacdev);
		synopGMAC_ubicom32_disable_interrupt(gmacdev);
		__napi_schedule(&gmacdev->napi);
	}
#else
	//synopGMAC_disable_interrupt_all(gmacdev);
	synopGMAC_ubicom32_disable_interrupt(gmacdev);
	tasklet_schedule(&gmacdev->tsk);
#endif
        return IRQ_HANDLED;
}

#ifdef UBICOM32_USE_NAPI
/*
 * NAPI poll callback
 * This is the NAPI callback function
 * @param[in] napi structure.
 * @param[in] napi poll budget.
 * \return Returns number of RX frames processed.
 */
static int synopGMAC_napi_poll(struct napi_struct *napi, int budget)
{
	struct net_device *netdev;
	synopGMACdevice *gmacdev;
	u32 interrupt;
        u32 count = 0;

	gmacdev  = container_of(napi, synopGMACdevice, napi);
	BUG_ON(gmacdev == NULL);

#else
/*
 * Tasklet poll callback
 * This is the Tasklet callback function
 * @param[in] tasklet structure.
 *
 */
static void synopGMAC_task_poll(unsigned long arg)
{
	struct net_device *netdev;
	synopGMACdevice *gmacdev;
	u32 interrupt;
        u32 count = 0, budget = UBICOM32_POLL_BUDGET;

	gmacdev  = (synopGMACdevice *)arg;
	BUG_ON(gmacdev == NULL);

#endif

	netdev  = gmacdev->synopGMACnetdev;
	BUG_ON(netdev == NULL);

	/*
	 * Now lets handle the DMA interrupts
	 */
	interrupt = synopGMAC_get_interrupt_type(gmacdev);
	TR("%s:Interrupts to be handled: 0x%08x\n", __FUNCTION__, interrupt);

	count = synopGMAC_handle_received_data(netdev, budget);

	if (gmacdev->BusyTxDesc > 0) {
		synopGMAC_handle_transmit_over(netdev);
	}

	if (unlikely(interrupt & (DmaIntErrorMask
			| DmaIntRxAbnMask | DmaIntRxStoppedMask
			| DmaIntTxAbnMask | DmaIntTxStoppedMask))) {

		if (interrupt & DmaIntErrorMask) {
			TR0("%s::Fatal Bus Error Inetrrupt Seen (DMA status = 0x%08x)\n",__FUNCTION__,interrupt);
			BUG();
#if 0	// Comented out thes untested and seemingly never used code.
			synopGMAC_disable_dma_tx(gmacdev);
			synopGMAC_disable_dma_rx(gmacdev);

			synopGMAC_take_desc_ownership_tx(gmacdev);
			synopGMAC_take_desc_ownership_rx(gmacdev);

			synopGMAC_init_tx_rx_desc_queue(gmacdev);

			synopGMAC_reset(gmacdev);

			/*
			 * After soft reset, configure the MAC address to default value
			 */
			synopGMAC_set_mac_addr(gmacdev,GmacAddr0High, GmacAddr0Low, netdev->dev_addr);

			/*
			 * The next two register must beprogrammed the same way as synopGMAC_linux_open()
			 */
#ifdef ENH_DESC_8W
			synopGMAC_dma_bus_mode_init(gmacdev, DmaFixedBurstEnable | DmaBurstLength16 | DmaDescriptorSkip0 | DmaDescriptor8Words | DmaArbitPr);
#else
			synopGMAC_dma_bus_mode_init(gmacdev, DmaFixedBurstEnable | DmaBurstLength16 | DmaDescriptorSkip0 | DmaArbitPr);
#endif
			synopGMAC_dma_control_init(gmacdev,DmaStoreAndForward | DmaRxThreshCtrl128 | DmaTxSecondFrame);

			synopGMAC_init_rx_desc_base(gmacdev);
			synopGMAC_init_tx_desc_base(gmacdev);
			synopGMAC_mac_init(gmacdev);
			synopGMAC_enable_dma_rx(gmacdev);
			synopGMAC_enable_dma_tx(gmacdev);
#endif
		}

		if (interrupt & DmaIntRxAbnMask) {
			TR("%s::Abnormal Rx Interrupt Seen (DMA status = 0x%08x)\n", __FUNCTION__, interrupt);
			#if 1
			if (gmacdev->GMAC_Power_down == 0) {	// If Mac is not in powerdown
				netdev->stats.rx_over_errors++;

				/*
				 * Just issue a poll demand to resume DMA operation
				 * (May happen when too few RX desciptors are in use)
				 */
				synopGMAC_handle_received_data(netdev, 0);
				synopGMAC_resume_dma_rx(gmacdev);
			}
			#endif
		}

		if (interrupt & DmaIntRxStoppedMask) {
			#if 1
			if (gmacdev->GMAC_Power_down == 0) {	// If Mac is not in powerdown
				TR0("%s::Receiver stopped (DMA status = 0x%08x)\n", __FUNCTION__, interrupt);
				BUG();

				netdev->stats.rx_over_errors++;

				synopGMAC_handle_received_data(netdev, 0);
				synopGMAC_enable_dma_rx(gmacdev);
			}
			#endif
		}

		if (interrupt & DmaIntTxAbnMask) {
			TR0("%s::Abnormal Tx Interrupt Seen (DMA status = 0x%08x)\n", __FUNCTION__, interrupt);
			#if 1
			if (gmacdev->GMAC_Power_down == 0) {    // If Mac is not in powerdown
			       synopGMAC_handle_transmit_over(netdev);
			}
			#endif
		}

		if (interrupt & DmaIntTxStoppedMask) {
			#if 1
			if (gmacdev->GMAC_Power_down == 0) {	// If Mac is not in powerdown
				TR0("%s::Transmitter stopped (DMA status = 0x%08x)\n", __FUNCTION__, interrupt);
				BUG();

				synopGMAC_disable_dma_tx(gmacdev);
				synopGMAC_take_desc_ownership_tx(gmacdev);

				synopGMAC_enable_dma_tx(gmacdev);
				netif_wake_queue(netdev);
			}
			#endif
		}
	}

#ifdef UBICOM32_USE_NAPI
	if (count < budget) {
		napi_complete(napi);
		if (synopGMAC_ubicom32_check_interrupt(gmacdev) != 0) {
			if (napi_reschedule(napi)) {
				return count;
			}
		}
		synopGMAC_ubicom32_enable_interrupt(gmacdev);
	}
        return count;
#else
	if ((count == budget) || (synopGMAC_ubicom32_check_interrupt(gmacdev) != 0)) {
                tasklet_schedule(&gmacdev->tsk);
        } else {
		synopGMAC_ubicom32_enable_interrupt(gmacdev);
	}
#endif
}

/*
 * Function used when the interface is opened for use.
 * We register synopGMAC_linux_open function to linux open(). Basically this
 * function prepares the the device for operation . This function is called whenever ifconfig (in Linux)
 * activates the device (for example "ifconfig eth0 up"). This function registers
 * system resources needed
 * 	- Attaches device to device specific structure
 * 	- Programs the MDC clock for PHY configuration
 * 	- Check and initialize the PHY interface
 *	- ISR registration
 * 	- Setup and initialize Tx and Rx descriptors
 *	- Initialize MAC and DMA
 *	- Allocate Memory for RX descriptors (The should be DMAable)
 * 	- Initialize one second timer to detect cable plug/unplug
 *	- Configure and Enable Interrupts
 *	- Enable Tx and Rx
 *	- start the Linux network queue interface
 * @param[in] pointer to net_device structure.
 * \return Returns 0 on success and error status upon failure.
 * \callgraph
 */
static s32 synopGMAC_linux_open(struct net_device *netdev)
{
	s32 status = 0;
	struct device *dev = &netdev->dev;
	synopGMACdevice *gmacdev = (synopGMACdevice *) netdev_priv(netdev);

	TR("%s called \n",__FUNCTION__);
	TR("gmacdev = %08x netdev = %08x dev= %08x\n", (u32)gmacdev, (u32)netdev, (u32)dev);
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	/*
	 * Now platform dependent initialization.
	 */
	synopGMAC_disable_interrupt_all(gmacdev);

	/*
	 * Lets reset the IP
	 */
	synopGMAC_reset(gmacdev);

	/*
	 * Start Ethernet I/O
	 */
	gmacdev->ethdn->command = UBI32_ETH_VP_CMD_RX_ENABLE | UBI32_ETH_VP_CMD_TX_ENABLE;
	//while ((gmacdev->ethdn->status & UBI32_ETH_VP_STATUS_RX_STATE) == 0);

	/*
	 * Lets read the version of ip in to device structure
	 */
	synopGMAC_read_version(gmacdev);
	synopGMAC_set_mac_addr(gmacdev, GmacAddr0High, GmacAddr0Low, netdev->dev_addr);

#if 0	// Unsupported by Ubicom port
	/*
	 * Check for Phy initialization
	 */
	synopGMAC_set_mdc_clk_div(gmacdev,GmiiCsrClk2);
	gmacdev->ClockDivMdc = synopGMAC_get_mdc_clk_div(gmacdev);
	status = synopGMAC_check_phy_init(gmacdev);
#endif

	/*
	 * Request for the interrupt
	 */
	status = request_irq (netdev->irq, synopGMAC_intr_handler, IRQF_DISABLED, netdev->name,netdev);
	if (status) {
 		TR0("Error in request_irq: errno = %d\n", status);
		return -ESYNOPGMACPHYERR;
	}
 	TR("%s owns an interrupt on line %d\n",netdev->name, netdev->irq);

	/*
	 * Set up the tx and rx descriptor queue/ring
	 */
	if (synopGMAC_setup_tx_desc_queue(gmacdev, dev, TRANSMIT_DESC_SIZE, RINGMODE) < 0) {
 		TR0("Error in setup TX descriptor\n");
		free_irq(netdev->irq, netdev);
		return -ESYNOPGMACNOMEM;
	}
	synopGMAC_init_tx_desc_base(gmacdev);	//Program the transmit descriptor base address in to DmaTxBase addr

	if (synopGMAC_setup_rx_desc_queue(gmacdev, dev, RECEIVE_DESC_SIZE, RINGMODE) < 0) {
 		TR0("Error in setup RX descriptor\n");
		free_irq(netdev->irq, netdev);
		synopGMAC_giveup_tx_desc_queue(gmacdev, dev, RINGMODE);
		return -ESYNOPGMACNOMEM;
	}
	synopGMAC_init_rx_desc_base(gmacdev);	//Program the receive descriptor base address in to DmaTxBase addr

#ifdef ENH_DESC_8W
	synopGMAC_dma_bus_mode_init(gmacdev, DmaFixedBurstEnable | DmaBurstLength16 | DmaDescriptorSkip0 | DmaDescriptor8Words | DmaArbitPr); //pbl32 incr with rxthreshold 128 and Desc is 8 Words
#else
	synopGMAC_dma_bus_mode_init(gmacdev, DmaFixedBurstEnable | DmaBurstLength16 | DmaDescriptorSkip0 | DmaArbitPr);                       //pbl32 incr with rxthreshold 128
#endif
	synopGMAC_dma_control_init(gmacdev, DmaStoreAndForward | DmaRxThreshCtrl128 | DmaTxSecondFrame);

	/*
	 * Initialize the mac interface
	 */
	synopGMAC_mac_init(gmacdev);
	synopGMAC_pause_control(gmacdev); // This enables the pause control in Full duplex mode of operation
	#ifdef IPC_OFFLOAD
	/*
	 * IPC Checksum offloading is enabled for this driver. Should only be used if Full Ip checksumm offload engine is configured in the hardware
	 */
	synopGMAC_enable_rx_chksum_offload(gmacdev);  	//Enable the offload engine in the receive path
	synopGMAC_rx_tcpip_chksum_drop_enable(gmacdev); // This is default configuration, DMA drops the packets if error in encapsulated ethernet payload
							// The FEF bit in DMA control register is configured to 0 indicating DMA to drop the errored frames.
	/*
	 * Inform the Linux Networking stack about the hardware capability of checksum offloading
	 */
	netdev->features |= NETIF_F_SG | NETIF_F_HW_CSUM;
	netdev->vlan_features |= NETIF_F_SG | NETIF_F_HW_CSUM;
	#endif

	synopGMAC_handle_received_data(netdev, 0);

	TR("Setting up the cable unplug timer\n");
	init_timer(&gmacdev->synopGMAC_cable_unplug_timer);
	gmacdev->synopGMAC_cable_unplug_timer.function = (void *)synopGMAC_linux_cable_unplug_function;
	gmacdev->synopGMAC_cable_unplug_timer.data = (u32) gmacdev;
	gmacdev->synopGMAC_cable_unplug_timer.expires = CHECK_TIME + jiffies;
	add_timer(&gmacdev->synopGMAC_cable_unplug_timer);

#ifdef  UBICOM32_USE_NAPI
	napi_enable(&gmacdev->napi);
#else
	tasklet_init(&gmacdev->tsk, synopGMAC_task_poll, (unsigned long)gmacdev);
#endif

	synopGMAC_clear_interrupt(gmacdev);

	/*
	 * Disable the interrupts generated by MMC and IPC counters.
	 * If these are not disabled ISR should be modified accordingly to handle these interrupts.
	 */
	synopGMAC_disable_mmc_tx_interrupt(gmacdev, 0xFFFFFFFF);
	synopGMAC_disable_mmc_rx_interrupt(gmacdev, 0xFFFFFFFF);
	synopGMAC_disable_mmc_ipc_rx_interrupt(gmacdev, 0xFFFFFFFF);

	synopGMAC_enable_dma_rx(gmacdev);
	synopGMAC_enable_dma_tx(gmacdev);

	netif_start_queue(netdev);
	synopGMAC_enable_interrupt(gmacdev, DmaIntEnable);

	return 0;
}

/*
 * Function used when the interface is closed.
 *
 * This function is registered to linux stop() function. This function is
 * called whenever ifconfig (in Linux) closes the device (for example "ifconfig eth0 down").
 * This releases all the system resources allocated during open call.
 * system resources int needs
 * 	- Disable the device interrupts
 * 	- Stop the receiver and get back all the rx descriptors from the DMA
 * 	- Stop the transmitter and get back all the tx descriptors from the DMA
 * 	- Stop the Linux network queue interface
 *	- Free the irq (ISR registered is removed from the kernel)
 * 	- Release the TX and RX descripor memory
 *	- De-initialize one second timer rgistered for cable plug/unplug tracking
 * @param[in] pointer to net_device structure.
 * \return Returns 0 on success and error status upon failure.
 * \callgraph
 */
static s32 synopGMAC_linux_close(struct net_device *netdev)
{
	synopGMACdevice *gmacdev = (synopGMACdevice *) netdev_priv(netdev);
	struct device *dev = &netdev->dev;

	TR("%s\n",__FUNCTION__);
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	/*
	 * Stop Ethernet I/O
	 */
	gmacdev->ethdn->command = 0;

	/*
	 * Disable all the interrupts
	 */
	synopGMAC_disable_interrupt_all(gmacdev);
	TR("the synopGMAC interrupt has been disabled\n");

	/*
	 * Disable the reception
	 */
	synopGMAC_rx_disable(gmacdev);
	plat_delay(10000); 		//Allow any pending buffer to be read by host
	synopGMAC_disable_dma_rx(gmacdev);
	synopGMAC_take_desc_ownership_rx(gmacdev);
	TR("the synopGMAC Reception has been disabled\n");

	/*
	 * Disable the transmission
	 */
	synopGMAC_disable_dma_tx(gmacdev);
	plat_delay(10000);		//allow any pending transmission to complete
	synopGMAC_tx_disable(gmacdev);
	synopGMAC_take_desc_ownership_tx(gmacdev);
	TR("the synopGMAC Transmission has been disabled\n");
	netif_stop_queue(netdev);

	/*
	 * Now free the irq: This will detach the interrupt handler registered
	 */
	free_irq(netdev->irq, netdev);
	TR("the synopGMAC interrupt handler has been removed\n");

#ifdef UBICOM32_USE_NAPI
	napi_disable(&gmacdev->napi);
#else
	tasklet_kill(&gmacdev->tsk);
#endif

	/*
	 * Free the Rx Descriptor contents
	 */
	TR("Now calling synopGMAC_giveup_rx_desc_queue \n");
	synopGMAC_giveup_rx_desc_queue(gmacdev, dev, RINGMODE);
	TR("Now calling synopGMAC_giveup_tx_desc_queue \n");
	synopGMAC_giveup_tx_desc_queue(gmacdev, dev, RINGMODE);

	TR("Freeing the cable unplug timer\n");
	del_timer(&gmacdev->synopGMAC_cable_unplug_timer);

	return -ESYNOPGMACNOERR;
}

/*
 * Function to transmit a given packet on the wire.
 * Whenever Linux Kernel has a packet ready to be transmitted, this function is called.
 * The function prepares a packet and prepares the descriptor and
 * enables/resumes the transmission.
 * @param[in] pointer to sk_buff structure.
 * @param[in] pointer to net_device structure.
 * \return Returns 0 on success and Error code on failure.
 * \note structure sk_buff is used to hold packet in Linux networking stacks.
 */
static s32 synopGMAC_linux_xmit_frames(struct sk_buff *skb, struct net_device *netdev)
{
	u32 offload_needed = 0, nr_frags;
	u32 dma_addr;
	synopGMACdevice *gmacdev;

	TR("%s called \n",__FUNCTION__);
	BUG_ON(skb == NULL);
	BUG_ON((skb->len < ETH_HLEN) || (skb->len > ETH_MAX_FRAME_LEN));
	if ((skb->len < ETH_HLEN) || (skb->len > ETH_MAX_FRAME_LEN))
		goto drop;

	gmacdev = (synopGMACdevice *) netdev_priv(netdev);
	if (gmacdev == NULL)
		goto drop;
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	nr_frags = skb_shinfo(skb)->nr_frags;
	BUG_ON(nr_frags > MAX_SKB_FRAGS);
	BUG_ON(skb_shinfo(skb)->gso_size != 0);

	if ((gmacdev->BusyTxDesc + nr_frags) >= gmacdev->TxDescCount) {
		/*
		 * Return busy here. The TX will be re-tried
		 */
		TR("%s No More Free Tx Descriptors\n",__FUNCTION__);
		netif_stop_queue(netdev);
		return NETDEV_TX_BUSY;
	}

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
	if (gmacdev->vlgrp) {
		if (unlikely(skb_headroom(skb) < VLAN_HLEN)) {
			if (!skb_cow_head(skb, VLAN_HLEN)) {
				return NETDEV_TX_BUSY;
			}
		}

		/*
		 * make skb->data private
		 */
		if (unlikely(skb_cloned(skb))) {
			struct sk_buff *skb_dup;
			skb_dup = skb_copy(skb, GFP_ATOMIC | __GFP_NOWARN);
			if (unlikely(!skb_dup)) {
				return NETDEV_TX_BUSY;
			}

			/*
			 * Free the old skb (reduce its reference count)
			 */
			dev_kfree_skb_any(skb);
			skb = skb_dup;
		}

		__skb_push(skb, VLAN_HLEN);
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
		: "memory", "cc"
		);
	}
#endif

	/*
	 * Now we have skb ready and OS invoked this function. Lets make our DMA know about this
	 */
	asm volatile ("prefetch (%0)" : : "a"(gmacdev->TxNextDesc));

	if (skb->ip_summed == CHECKSUM_PARTIAL) {
		offload_needed = 0x00000001;
	}

	if (likely(nr_frags == 0)) {
		dma_map_single_buf(skb->data, skb->len);
		dma_addr = virt_to_phys(skb->data);
		synopGMAC_set_tx_qptr(gmacdev, dma_addr, skb->len, (u32)skb,
			offload_needed, (DescTxFirst | DescTxLast | DescTxIntEnable), DescOwnByDma);
		asm volatile ("add.4 %0, %0, %1" : : "U4"(gmacdev->BusyTxDesc), "d"(1) : "cc", "memory");
	} else {
		u32 i = 0, first_ds, len;
		DmaDesc *txdesc;
		struct skb_frag_struct *frag;
		GMAC_UBICOM_PROFILER_VCOUNT_INC(tx_frame_fragmented);

		/*
		 * Now process multi-fragment TX
		 * First fragment (the header part of skb)
		 */
		len = skb->len - skb->data_len;
		dma_map_single_buf(skb->data, len);
		dma_addr = virt_to_phys(skb->data);
		first_ds = synopGMAC_set_tx_qptr(gmacdev, dma_addr, len, 0,
				offload_needed, DescTxFirst, 0);

		/*
		 * All fragments except the last one (unlikely)
		 */
		while (unlikely(i < (nr_frags - 1))) {
			frag = &skb_shinfo(skb)->frags[i++];
			len = frag->size;
			dma_addr = (unsigned long) page_to_phys(frag->page) + frag->page_offset;
			dma_map_single_buf((void *)dma_addr, len);
			synopGMAC_set_tx_qptr(gmacdev, dma_addr, len, 0, 0, 0, DescOwnByDma);
		}

		/*
		 * Last fragment
		 */
		frag = &skb_shinfo(skb)->frags[i];
		len = frag->size;
		dma_addr = (unsigned long) page_to_phys(frag->page) + frag->page_offset;
		dma_map_single_buf((void *)dma_addr, len);
		synopGMAC_set_tx_qptr(gmacdev, dma_addr, len, (u32)skb,
				0, (DescTxLast | DescTxIntEnable), DescOwnByDma);

		/*
		 * Set DMA flag for the first segment as the last step
		 */
		txdesc = gmacdev->TxDesc + first_ds;
		txdesc->status |= DescOwnByDma;
		dma_map_single_desc(txdesc);

		asm volatile ("add.4 %0, %0, %1" : : "U4"(gmacdev->BusyTxDesc), "d"(nr_frags + 1) : "cc", "memory");
	}

	/*
	 * Now force the DMA to start transmission
	 */
	synopGMAC_resume_dma_tx(gmacdev);
	netdev->trans_start = jiffies;

	return NETDEV_TX_OK;

drop:
	/*
	 * Now drop it
	 */
	dev_kfree_skb_any(skb);
	netdev->stats.tx_dropped++;
	return NETDEV_TX_OK;
}

/*
 * Function provides the network interface statistics.
 * Function is registered to linux get_stats() function. This function is
 * called whenever ifconfig (in Linux) asks for networkig statistics
 * (for example "ifconfig eth0").
 * @param[in] pointer to net_device structure.
 * \return Returns pointer to net_device_stats structure.
 * \callgraph
 */
static struct net_device_stats *  synopGMAC_linux_get_stats(struct net_device *netdev)
{
	TR("%s called \n", __FUNCTION__);
	return( &(netdev->stats) );
}

/*
 * Function to set multicast and promiscous mode.
 * @param[in] pointer to net_device structure.
 * \return returns void.
 */
static void synopGMAC_linux_set_multicast_list(struct net_device *netdev)
{
	TR("%s called \n", __FUNCTION__);
	//todo Function not yet implemented.
}

/*
 * Function to set ethernet address of the NIC.
 * @param[in] pointer to net_device structure.
 * @param[in] pointer to an address structure.
 * \return Returns 0 on success Errorcode on failure.
 */
static s32 synopGMAC_linux_set_mac_address(struct net_device *netdev, void * macaddr)
{
	synopGMACdevice *gmacdev;
	struct sockaddr *addr = macaddr;

	TR("%s called \n", __FUNCTION__);
	gmacdev = (synopGMACdevice *) netdev_priv(netdev);
	if (gmacdev == NULL)
		return -1;
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	synopGMAC_set_mac_addr(gmacdev,GmacAddr0High,GmacAddr0Low, addr->sa_data);
	synopGMAC_get_mac_addr(gmacdev,GmacAddr0High,GmacAddr0Low, netdev->dev_addr);

	return 0;
}

/*
 * Function to change the Maximum Transfer Unit.
 * @param[in] pointer to net_device structure.
 * @param[in] New value for maximum frame size.
 * \return Returns 0 on success Errorcode on failure.
 */
static s32 synopGMAC_linux_change_mtu(struct net_device *netdev, s32 newmtu)
{
	TR("%s called \n", __FUNCTION__);
	//todo Function not yet implemented.
	return 0;
}

/*
 * IOCTL interface.
 * This function is mainly for debugging purpose.
 * This provides hooks for Register read write, Retrieve descriptor status
 * and Retreiving Device structure information.
 * @param[in] pointer to net_device structure.
 * @param[in] pointer to ifreq structure.
 * @param[in] ioctl command.
 * \return Returns 0 on success Error code on failure.
 */
static s32 synopGMAC_linux_do_ioctl(struct net_device *netdev, struct ifreq *ifr, s32 cmd)
{
	s32 retval = 0;
	u16 temp_data = 0;
	synopGMACdevice *gmacdev = NULL;
	struct mii_ioctl_data *mii_req = if_mii(ifr);
	struct ifr_data_struct
	{
		u32 unit;
		u32 addr;
		u32 data;
	} *req;


	if (netdev == NULL)
		return -1;
	if (ifr == NULL)
		return -1;

	req = (struct ifr_data_struct *)ifr->ifr_data;

	gmacdev = (synopGMACdevice *) netdev_priv(netdev);
	if (gmacdev == NULL)
		return -1;
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	//TR("%s :: on device %s req->unit = %08x req->addr = %08x req->data = %08x cmd = %08x \n",__FUNCTION__,netdev->name,req->unit,req->addr,req->data,cmd);

	switch (cmd)
	{
		case SIOCGMIIREG:
			if ((mii_req->reg_num & 0x1F) == MII_BMCR) {
				/*
				 * Make up MII control register value from what we know
				 */
				mii_req->val_out = 0x0000
				| ((gmacdev->ethdn->status & UBI32_ETH_VP_STATUS_DUPLEX)
						? BMCR_FULLDPLX : 0)
				| ((gmacdev->ethdn->status & UBI32_ETH_VP_STATUS_SPEED100)
						? BMCR_SPEED100 : 0)
				| ((gmacdev->ethdn->status & UBI32_ETH_VP_STATUS_SPEED1000)
						? BMCR_SPEED1000 : 0);
			} else if ((mii_req->reg_num & 0x1F) == MII_BMSR) {
				/*
				 * Make up MII status register value from what we know
				 */
				mii_req->val_out =
				(BMSR_100FULL|BMSR_100HALF|BMSR_10FULL|BMSR_10HALF)
				| ((gmacdev->ethdn->status & UBI32_ETH_VP_STATUS_LINK)
						? BMSR_LSTATUS : 0);
			} else {
				return -EIO;
			}
			break;

		case IOCTL_READ_REGISTER:		//IOCTL for reading IP registers : Read Registers
			if (req->unit == 0)		// Read Mac Register
				req->data = synopGMACReadReg((u32 *)gmacdev->MacBase, req->addr);
			else if (req->unit == 1)	// Read DMA Register
				req->data = synopGMACReadReg((u32 *)gmacdev->DmaBase, req->addr);
			else if (req->unit == 2){	// Read Phy Register
				retval = synopGMAC_read_phy_reg((u32 *)gmacdev->MacBase,
						gmacdev->PhyBase, req->addr, &temp_data);
				req->data = (u32)temp_data;
				if(retval != -ESYNOPGMACNOERR)
					TR("ERROR in Phy read\n");
			}
			break;

		case IOCTL_WRITE_REGISTER:		//IOCTL for reading IP registers : Read Registers
			if (req->unit == 0)		// Write Mac Register
				synopGMACWriteReg((u32 *)gmacdev->MacBase, req->addr, req->data);
			else if (req->unit == 1)	// Write DMA Register
				synopGMACWriteReg((u32 *)gmacdev->DmaBase, req->addr, req->data);
			else if (req->unit == 2){	// Write Phy Register
				retval = synopGMAC_write_phy_reg((u32 *)gmacdev->MacBase,
						gmacdev->PhyBase, req->addr, req->data);
				if (retval != -ESYNOPGMACNOERR)
					TR("ERROR in Phy read\n");
			}
			break;

		case IOCTL_READ_IPSTRUCT:		//IOCTL for reading GMAC DEVICE IP private structure
			memcpy(ifr->ifr_data, gmacdev, sizeof(synopGMACdevice));
			break;

		case IOCTL_READ_RXDESC:			//IOCTL for Reading Rx DMA DESCRIPTOR
			memcpy(ifr->ifr_data, gmacdev->RxDesc + ((DmaDesc *) (ifr->ifr_data))->data1, sizeof(DmaDesc) );
			break;

		case IOCTL_READ_TXDESC:			//IOCTL for Reading Tx DMA DESCRIPTOR
			memcpy(ifr->ifr_data, gmacdev->TxDesc + ((DmaDesc *) (ifr->ifr_data))->data1, sizeof(DmaDesc) );
			break;

		case IOCTL_POWER_DOWN:
			if (req->unit == 1){		//power down the mac
				TR("============I will Power down the MAC now =============\n");
				// If it is already in power down don't power down again
				retval = 0;
				if (((synopGMACReadReg((u32 *)gmacdev->MacBase,GmacPmtCtrlStatus)) & GmacPmtPowerDown) != GmacPmtPowerDown) {
					synopGMAC_linux_powerdown_mac(gmacdev);
					retval = 0;
				}
			}
			if (req->unit == 2){		//Disable the power down  and wake up the Mac locally
				TR("============I will Power up the MAC now =============\n");
				//If already powered down then only try to wake up
				retval = -1;
				if (((synopGMACReadReg((u32 *)gmacdev->MacBase,GmacPmtCtrlStatus)) & GmacPmtPowerDown) == GmacPmtPowerDown) {
					synopGMAC_power_down_disable(gmacdev);
					synopGMAC_linux_powerup_mac(gmacdev);
					retval = 0;
				}
			}
			break;
		default:
			retval = -1;
	}

	return retval;
}

/*
 * Function to handle a Tx Hang.
 * This is a software hook (Linux) to handle transmitter hang if any.
 * We get transmitter hang in the device interrupt status, and is handled
 * in ISR. This function is here as a place holder.
 * @param[in] pointer to net_device structure
 * \return void.
 */
static void synopGMAC_linux_tx_timeout(struct net_device *netdev)
{
	synopGMACdevice *gmacdev;
	u32 interrupt;
	TR0("%s called \n",__FUNCTION__);

	gmacdev = (synopGMACdevice *) netdev_priv(netdev);
	if (gmacdev == NULL)
		return;
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	if (gmacdev->GMAC_Power_down != 0) {	// If Mac is not in powerdown
		TR0("%s TX time out during power downis ignored\n", netdev->name);
		return;
	}

	interrupt = synopGMAC_get_interrupt_type(gmacdev);
	TR0("%s TX time out (DMA status = 0x%08x)\n", netdev->name, interrupt);

	BUG_ON(gmacdev->BusyTxDesc > 0);
	synopGMAC_resume_dma_tx(gmacdev);
}

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
/*
 * ubi32_eth_vlan_rx_register: regsiter vlan for RX acceleration
 */
static void ubi32_eth_vlan_rx_register(struct net_device *netdev, struct vlan_group *grp)
{
	synopGMACdevice *gmacdev = (synopGMACdevice *)netdev_priv(netdev);
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	TR0("%s: register VLAN for HW acceleration\n", netdev->name);
	gmacdev->vlgrp = grp;
}
#endif

static const struct net_device_ops ubicom32_synopGMAC_netdev_ops = {
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
	.ndo_vlan_rx_register 	= ubi32_eth_vlan_rx_register,
#endif
	.ndo_open 		= &synopGMAC_linux_open,
	.ndo_stop 		= &synopGMAC_linux_close,
	.ndo_start_xmit 	= &synopGMAC_linux_xmit_frames,
	.ndo_get_stats 		= &synopGMAC_linux_get_stats,
	.ndo_set_multicast_list = &synopGMAC_linux_set_multicast_list,
	.ndo_set_mac_address 	= &synopGMAC_linux_set_mac_address,
	.ndo_change_mtu 	= &synopGMAC_linux_change_mtu,
	.ndo_do_ioctl 		= &synopGMAC_linux_do_ioctl,
	.ndo_tx_timeout 	= &synopGMAC_linux_tx_timeout,
};

/*
 * Function to initialize the Linux network interface.
 *
 * Linux dependent Network interface is setup here. This provides
 * an example to handle the network dependent functionality.
 *
 * \return Returns 0 on success and Error code on failure.
 */
s32 __init synopGMAC_init_network_interface(void)
{
	u32 i;
	s32 err = 0;
	struct net_device *netdev;
	synopGMACdevice *gmacdev;
	struct ethtionode *eth_node;
	const u8 def_mac[ETH_ALEN] = DEFAULT_MAC_ADDRESS;
	const char *ultra_eth_name[2] = {"eth_lan", "eth_wan"};;

	for (i = 0; i < 2; i++) {
		/*
		 * See if the eth_vp is in the device tree.
		 * (Break here is no Ethernet instance found)
		 */
		eth_node = (struct ethtionode *)devtree_find_node(ultra_eth_name[i]);
		if (!eth_node) {
			err = -ENODEV;
			break;
		}

		if ((eth_node->dn.recvirq != RH_GMAC_INT) && (eth_node->dn.recvirq != RI_GMAC_INT)) {
			TR0("%s: invalid GMAC I/O block\n", ultra_eth_name[i]);
			BUG();
			err = -EIO;
			break;
		}

		/*
		 * Lets allocate and set up an ethernet device, it takes the sizeof the private structure.
		 *  This is mandatory as a 32 byte allignment is required for the private data structure.
		 */
		netdev = alloc_etherdev(sizeof(synopGMACdevice));
		if(!netdev){
			TR0("Problem in alloc_etherdev()..Take Necessary action\n");
			err = -ESYNOPGMACNOMEM;
			break;
		}

		if (eth_node->dn.recvirq == RH_GMAC_INT) {
			netdev->base_addr = IO_PORT_RH;
		} else {
			netdev->base_addr = IO_PORT_RI;
		}
		netdev->irq = eth_node->dn.recvirq;
		TR0("Initializing synopsys GMAC interfaces %s: (port = 0x%lx, irq = %d)\n",
			eth_node->dn.name, netdev->base_addr, netdev->irq) ;

		/*
		 * Attach the device to MAC struct This will configure all the required base addresses
		 * such as Mac base, configuration base, phy base address (out of 32 possible phys)
		 */
		gmacdev = netdev_priv(netdev);
		synopGMAC_attach(gmacdev, netdev->base_addr);

		gmacdev->synopGMACnetdev = netdev;
		gmacdev->ethdn = eth_node;

		/*
		 * Toggle interface enable/disable to allow proper HW initialization to be done.
		 * (This is to work-around for problem of mising RX clock when using external PHY)
		 */
		gmacdev->ethdn->command = UBI32_ETH_VP_CMD_RX_ENABLE | UBI32_ETH_VP_CMD_TX_ENABLE;
		while ((gmacdev->ethdn->status & UBI32_ETH_VP_STATUS_RX_STATE) == 0);
		gmacdev->ethdn->command = 0;

#ifdef UBICOM32_USE_NAPI
		netif_napi_add(netdev, &gmacdev->napi, synopGMAC_napi_poll, UBICOM32_POLL_BUDGET);
#endif

		synopGMAC_reset(gmacdev);
		//netdev->features |= NETIF_F_HIGHDMA; -- TBD by Ubicom

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
		/*
		 * Enabel VLAN acceleration
		 */
		gmacdev->vlgrp = NULL;
		netdev->features |= NETIF_F_HW_VLAN_RX | NETIF_F_HW_VLAN_TX;
#endif

		/*
		 * This just fill in some default Ubicom MAC address
		 */
		memcpy(netdev->perm_addr, def_mac, ETH_ALEN);
		memcpy(netdev->dev_addr, def_mac, ETH_ALEN);
		netdev->dev_addr[ETH_ALEN - 1] += i;
		memset(netdev->broadcast, 0xff, ETH_ALEN);

		netdev->watchdog_timeo = 5 * HZ;
		netdev->netdev_ops = &ubicom32_synopGMAC_netdev_ops;

		/*Now start the network interface*/
		TR("Now Registering the netdevice\n");
		if (register_netdev(netdev)) {
			TR0("Error in Registering netdevice\n");
			free_netdev(netdev);
			break;
		}

		synopGMACDev[i] = gmacdev;
	}

	if ((synopGMACDev[0] == NULL) && (synopGMACDev[1] == NULL)) {
		TR0("no native Ethernet interface found\n");
		return err;
	}

#ifdef GMAC_UBICOM_PROFILER
	profile_register_performance_counter(&int_count, "GMAC int");
	profile_register_performance_counter(&rx_desc_check, "GMAC RX desc");
	profile_register_performance_counter(&rx_frame, "GMAC RX frame");
	profile_register_performance_counter(&rx_poll, "GMAC RX poll");
	profile_register_performance_counter(&tx_desc_check, "GMAC TX desc");
	profile_register_performance_counter(&tx_frame, "GMAC TX frame");
	profile_register_performance_counter(&tx_poll, "GMAC TX poll");
	profile_register_performance_counter(&tx_frame_fragmented, "GMAC TX fragmented frame");
#endif

	return 0;
}

/*
 * Function to initialize the Linux network interface.
 * Linux dependent Network interface is setup here. This provides
 * an example to handle the network dependent functionality.
 * \return Returns 0 on success and Error code on failure.
 */
void __exit synopGMAC_exit_network_interface(void)
{
	u32 i;
	synopGMACdevice *gmacdev;

	TR0("Now Calling network_unregister\n");
	for (i = 0; i < 2; i++) {
		gmacdev = synopGMACDev[i];
		if (gmacdev) {
			unregister_netdev(gmacdev->synopGMACnetdev);
			free_netdev(gmacdev->synopGMACnetdev);
			synopGMACDev[i] = NULL;
		}
	}
}
