/*
 * arch/ubicom32/mach-common/usb_dwc.c
 *   Ubicom32 architecture usb support for Synopsys DWC OTG IP.
 *
 * (C) Copyright 2010, Ubicom, Inc.
 * Copyright (C) 2007 MontaVista Software, Inc. <source@mvista.com>
 * Author: Kevin Hilman
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 2 of the License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <asm/cacheflush.h>
#include <asm/ubicom32-io.h>
#include <asm/lm.h>
#include <asm/devtree.h>

/*
 * Bits to disable/enable usb2 clock
 */
#define DWC_CLK0_DIS_BIT		(1 << 12)
#define DWC_CLK1_DIS_BIT		(1 << 13)

/*
 * Bits to move usb2 clock from reference freq to working freq
 */
#define DWC_CLK0_SEL_BIT		(1 << 12)
#define DWC_CLK1_SEL_BIT		(1 << 13)

/*
 * Port (FUNC) base addresses
 */
#define DWC_PORT0_BASE			IO_PORT_RF
#define DWC_PORT1_BASE			IO_PORT_RG
#define DWC_FUNC_BASE(inst)		((inst) ? DWC_PORT1_BASE : DWC_PORT0_BASE)

/*
 * Func register bitmap
 */
#define DWC_FN_RESET_BIT		(1 << 4)

/*
 * Function ctl0 register bitmap of HSOTG Control Register
 */
#define DWC_CTL0_UPDATE_TOGGLE_BIT      (1 << 31)
#define DWC_CTL0_SOF_TOGGLE_MASK        (0x3FFF)
#define DWC_CTL0_SOF_TOGGLE(val)        ((val & DWC_CTL0_SOF_TOGGLE_MASK) << 16)
#define DWC_CTL0_LOWLEAK_MEM_DIS_BIT	(1 << 15)
#define DWC_CTL0_LOWLEAK_MODE_EN_BIT	(1 << 14)
#define DWC_CTL0_SLV_AHB_BLK_ADDR_MASK  (0x3F)
#define DWC_CTL0_SLV_AHB_BLK_ADDR(val)  ((val & DWC_CTL0_SLV_AHB_BLK_ADDR_MASK) << 8)
#define DWC_CTL0_SLV_AHB_BIGENDIAN      (1 << 3)
#define DWC_CTL0_DMA_AHB_BIGENDIAN      (1 << 2)
#define DWC_CTL0_WSTALL_BIT		(1 << 0)



/*
 * Function ctl1 register bitmap of Phy Configuration Register
 */
#define DWC_PHYENABLE_BIT		(1 << 31)
#define DWC_PHYCOMMMONONN_BIT		(1 << 30)
#define DWC_PHYPORTRESET_BIT		(1 << 25)
#define DWC_PHYOTG_DISABLE_BIT		(1 << 24)
#define DWC_PHYOTG_VBUSVLDEXT           (1 << 23)
#define DWC_ENDIAN_SELECT           	(1 << 14)
#define DWC_PHYOTG_SUSPENDM_OVR_BIT     (1 << 13)
#define DWC_PHYOTG_SESSEND_OVRD_BIT	(1 << 12)
#define DWC_PHYOTG_VBUSVALID_OVRD_BIT	(1 << 11)
#define DWC_PHYOTG_IDDIG_OVRD_BIT	(1 << 10)
#define DWC_PHYOTG_BVALID_OVRD_BIT	(1 << 9)
#define DWC_PHYOTG_AVALID_OVRD_BIT	(1 << 8)
#define DWC_PHYOTG_SUSPENDM_BIT		(1 << 5)
#define DWC_PHYOTG_SESSEND_BIT		(1 << 4)
#define DWC_PHYOTG_VBUSVALID_BIT	(1 << 3)
#define DWC_PHYOTG_IDDIG_BIT		(1 << 2)
#define DWC_PHYOTG_BVALID_BIT		(1 << 1)
#define DWC_PHYOTG_AVALID_BIT		(1 << 0)

/*
 * Function ctl2 register bitmap of Phy Test Control Register
 */
#define DWC_PHYTESTCTRL_BISTEN          (1 << 23)
#define DWC_PHYTESTCTRL_LSBIST          (1 << 22)
#define DWC_PHYTESTCTRL_FSBIST          (1 << 21)
#define DWC_PHYTESTCTRL_HSBIST          (1 << 20)
#define DWC_PHYTESTCTRL_VATESTENB       (1 << 17)
#define DWC_PHYTESTCTRL_SIDDQ           (1 << 16)
#define DWC_PHYTESTCTRL_TESTDATAIN_MASK (0xFF)
#define DWC_PHYTESTCTRL_TETDATAIN(val)  ((val & DWC_PHYTESTCTRL_TESTDATAIN_MASK) << 8)
#define DWC_PHYTESTCTRL_TESTADDR_MASK   (0x0F)
#define DWC_PHYTESTCTRL_TESTADDR(val)   ((val & DWC_PHYTESTCTRL_TESTADDR_MASK) << 4)
#define DWC_PHYTESTCTRL_TESTDATAOUTSEL  (1 << 3)
#define DWC_PHYTESTCTRL_TESTCLK0        (1 << 2)
#define DWC_PHYTESTCTRL_LOOPBACKENB     (1 << 1)


/*
 * Function ctl3 register bitmap of Phy Parameter Override Register
 */

/* 
 * VBUS Valid Threshold Adjustment
 * Function: This bus adjusts the voltage level for the VBUS Valid threshold.
 * 111: + 9%
 * 110: + 6%
 * 101: + 3%
 * 100: Design default
 * 011: - 3%
 * 010: - 6%
 * 001: - 9%
 * 000: - 12%
 */

#define DWC_PHYPARAMOVR_OTGTUNE_MASK    (0x07)
#define DWC_PHYPARAMOVR_OTGTUNE(val)    ((val & DWC_PHYPARAMOVR_OTGTUNE_MASK) << 20)
#define DWC_PHYPARAMOVR_OTGTUNE_P9      (0x07)
#define DWC_PHYPARAMOVR_OTGTUNE_P6      (0x06)
#define DWC_PHYPARAMOVR_OTGTUNE_P3      (0x05
#define DWC_PHYPARAMOVR_OTGTUNE_DEFAULT (0x04)
#define DWC_PHYPARAMOVR_OTGTUNE_M3      (0x03)
#define DWC_PHYPARAMOVR_OTGTUNE_M6      (0x02)
#define DWC_PHYPARAMOVR_OTGTUNE_M9      (0x01)
#define DWC_PHYPARAMOVR_OTGTUNE_M12     (0x00)




/*
 * Disconnect Threshold Adjustment
 * Function: This bus adjusts the voltage level for the threshold used to detect
 * a disconnect event at the host.
 * 111: + 4.5%
 * 110: + 3%
 * 101: + 1.5%
 * 100: Design default
 * 011: - 1.5%
 * 010: - 3%
 * 001: - 4.5%
 * 000: - 6%
 */

#define DWC_PHYPARAMOVR_COMPDISTUNE_MASK    (0x07)
#define DWC_PHYPARAMOVR_COMPDISTUNE(val)    ((val & DWC_PHYPARAMOVR_COMPDISTUNE_MASK) << 15)
#define DWC_PHYPARAMOVR_COMPDISTUNE_P4_5    (0x07)
#define DWC_PHYPARAMOVR_COMPDISTUNE_P3      (0x06)
#define DWC_PHYPARAMOVR_COMPDISTUNE_P1_5    (0x05)
#define DWC_PHYPARAMOVR_COMPDISTUNE_DEFAULT (0x04)
#define DWC_PHYPARAMOVR_COMPDISTUNE_M1_5    (0x03)
#define DWC_PHYPARAMOVR_COMPDISTUNE_M3      (0x02)
#define DWC_PHYPARAMOVR_COMPDISTUNE_M4_5    (0x01)
#define DWC_PHYPARAMOVR_COMPDISTUNE_M6      (0x00)

/*
 * Squelch Threshold Adjustment
 * Function: This bus adjusts the voltage level for the threshold used to detect
 * valid high-speed data.
 * 111: - 20%
 * 110: - 15%
 * 101: - 10%
 * 100: - 5%
 * 011: Design default
 * 010: + 5%
 * 001: + 10%
 * 000: + 15%
 */

#define DWC_PHYPARAMOVR_SQRXTUNE_MASK    (0x07)
#define DWC_PHYPARAMOVR_SQRXTUNE(val)    ((val & DWC_PHYPARAMOVR_SQRXTUNE_MASK) << 12)
#define DWC_PHYPARAMOVR_SQRXTUNE_M20     (0x07)
#define DWC_PHYPARAMOVR_SQRXTUNE_M15     (0x06)
#define DWC_PHYPARAMOVR_SQRXTUNE_M10     (0x05)
#define DWC_PHYPARAMOVR_SQRXTUNE_M5      (0x04)
#define DWC_PHYPARAMOVR_SQRXTUNE_DEFAULT (0x03)
#define DWC_PHYPARAMOVR_SQRXTUNE_P5      (0x02)
#define DWC_PHYPARAMOVR_SQRXTUNE_P10     (0x01)
#define DWC_PHYPARAMOVR_SQRXTUNE_P15     (0x00)


/*
 * FS/LS Source Impedance Adjustment
 * Function: This bus adjusts the low- and full-speed single-ended source
 * impedance while driving high. The following adjustment values are based on
 * nominal process, voltage, and temperature.
 * 1111: - 5%
 * 0111: - 2.5%
 * 0011: Design default
 * 0001: + 2.5%
 * 0000: + 5%
 * All other bit settings are reserved.
 */

#define DWC_PHYPARAMOVR_TXFSLSTUNE_MASK    (0x0F)
#define DWC_PHYPARAMOVR_TXFSLSTUNE(val)    ((val & DWC_PHYPARAMOVR_TXFSLSTUNE_MASK) << 8)
#define DWC_PHYPARAMOVR_TXFSLSTUNE_M5      (0x0F)
#define DWC_PHYPARAMOVR_TXFSLSTUNE_M2_5    (0x07)
#define DWC_PHYPARAMOVR_TXFSLSTUNE_DEFAULT (0x03)
#define DWC_PHYPARAMOVR_TXFSLSTUNE_P2_5    (0x01)
#define DWC_PHYPARAMOVR_TXFSLSTUNE_P5      (0x00)

/*
 * HS DC Voltage Level Adjustment
 * Function: This bus adjusts the high-speed DC level voltage.
 * 1111: + 8.75%
 * 1110: + 7.5%
 * 1101: + 6.25%
 * 1100: + 5%
 * 1011: + 3.75%
 * 1010: + 2.5%
 * 1001: + 1.25%
 * 1000: Design default
 * 0111: - 1.25%
 * 0110: - 2.5%
 * 0101: - 3.75%
 * 0100: - 5%
 * 0011: - 6.25%
 * 0010: - 7.5%
 * 0001: - 8.75%
 * 0000: - 10%
 */

#define DWC_PHYPARAMOVR_TXVREFTUNE_MASK     (0x0F)
#define DWC_PHYPARAMOVR_TXVREFSTUNE(val)    ((val & DWC_PHYPARAMOVR_TXVREFTUNE_MASK) << 4)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_P8_75   (0x0F)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_P7_5    (0x0E)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_P6_25   (0x0D)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_P5      (0x0C)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_P3_75   (0x0B)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_P2_5    (0x0A)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_P1_25   (0x09)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_DEFAULT (0x08)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_M1_25   (0x07)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_M2_5    (0x06)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_M3_75   (0x05)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_M5      (0x04)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_M6_25   (0x03)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_M7_5    (0x02)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_M8_75   (0x01)
#define DWC_PHYPARAMOVR_TXVREFSTUNE_M10     (0x00)

/*
 * HS Transmitter Pre-Emphasis Enable
 * Function: This signal controls the pre-emphasis for a J-K or K-J state
 * transition in HS mode.
 * 1:                  The HS Transmitter pre-emphasis is enabled.
 * 0 (Design default): The HS Transmitter pre-emphasis is disabled.
 */
#define DWC_PHYPARAMOVR_PREEMPHTUNE     (1 << 3)


/*
 * HS Transmitter Rise/Fall Time Adjustment
 * Function: This bus adjusts the rise/fall times of the high-speed waveform.
 *   1: -8%
 *   0: Design Default
 */
#define DWC_PHYPARAMOVR_RISETUNE        (1 << 2)



/*
 * Transmitter High-Speed Crossover Adjustment
 * Function: This bus adjusts the voltage at which the DP and DM signals
 * cross while transmitting in HS mode.
 */

#define DWC_PHYPARAMOVR_TXHSXVTUNE_MASK    (0x03)
#define DWC_PHYPARAMOVR_TXHSXVTUNE(val)    ((val &DWC_PHYPARAMOVR_TXHSXVTUNE_MASK) << 0 ) 
#define DWC_PHYPARAMOVR_TXHSXVTUNE_DEFAULT (0x03)
#define DWC_PHYPARAMOVR_TXHSXVTUNE_P15MV   (0x02)
#define DWC_PHYPARAMOVR_TXHSXVTUNE_M15MV   (0x01)



/*
 * Function 4-7 unused
 */

/*
 * Function status0 register bitmap
 */

/*
 * Function status1 register bitmap
 */

#define DWC_STATUS1_WCMDQ_EMPTY_BIT	(1 << 0)

/*
 * More readable alias
 */
#define IS_DEVICE			0	
#define IS_HOST				1

/*
 * Helper macros handling registers
 */
#define DWC_REG_TST(reg, val) ( *((volatile unsigned int *)(reg)) & val )
#define DWC_REG_SET(reg, val) { volatile unsigned int *r = (unsigned int *)(reg); *r = (*r | (val)); }
#define DWC_REG_CLR(reg, val) { volatile unsigned int *r = (unsigned int *)(reg); *r = (*r & ~(val)); }
#define DWC_REG_WRITE(reg, val) { volatile unsigned int *r = (unsigned int *)(reg); *r = (val); }
#define DWC_REG_READ(reg, val) { volatile unsigned int *r = (unsigned int *)(reg); val = *r; }

/*
 * Use otg overide signal to force a port in either host or device mode.
 */
static void __init dwc_otg_overide(u32 fbase, bool is_host)
{
        printk(KERN_INFO "%s: 0x%08X  %d \n", __func__, fbase, is_host);
	/*
	 * Enable overide signals
	 */
	DWC_REG_SET(fbase + IO_CTL1, DWC_PHYOTG_SESSEND_OVRD_BIT | DWC_PHYOTG_VBUSVALID_OVRD_BIT | DWC_PHYOTG_IDDIG_OVRD_BIT
			| DWC_PHYOTG_BVALID_OVRD_BIT | DWC_PHYOTG_AVALID_OVRD_BIT);

	if (is_host) {
		/*
		 * Overide to be host mode 
		 */
		DWC_REG_SET(fbase + IO_CTL1, DWC_PHYOTG_AVALID_BIT | DWC_PHYOTG_VBUSVALID_BIT);
		DWC_REG_CLR(fbase + IO_CTL1, DWC_PHYOTG_BVALID_BIT | DWC_PHYOTG_IDDIG_BIT | DWC_PHYOTG_SESSEND_BIT);
	} else {
		/*
		 * Overide to be device mode
		 */
		DWC_REG_SET(fbase + IO_CTL1, DWC_PHYOTG_BVALID_BIT | DWC_PHYOTG_IDDIG_BIT | DWC_PHYOTG_VBUSVALID_BIT);
		DWC_REG_CLR(fbase + IO_CTL1, DWC_PHYOTG_AVALID_BIT | DWC_PHYOTG_SESSEND_BIT);
	}
}

/*
 * Set the port into its proper mode.
 */
static void __init dwc_otg_cfg_mode(u32 fbase, int inst)
{
	/*
	 * Set the correct mode of port 1 
	 */
	if (inst == 0) {
#ifdef CONFIG_DWC_PORT0_OTG
		printk(KERN_INFO "Set usb port 0 to OTG\n");
		DWC_REG_CLR(fbase + IO_CTL1, DWC_PHYOTG_DISABLE_BIT);
#endif

#ifdef CONFIG_DWC_PORT0_HOST
		printk(KERN_INFO "Set usb port 0 to host only\n");
		dwc_otg_overide(fbase, IS_HOST);
#endif

#ifdef CONFIG_DWC_PORT0_DEVICE
		printk(KERN_INFO "Set usb port 0 to device only\n");
		dwc_otg_overide(fbase, IS_DEVICE);
#endif
	} else {
#ifdef CONFIG_DWC_PORT1_HOST
		printk(KERN_INFO "Set usb port 1 to host only\n");
		dwc_otg_overide(fbase, IS_HOST);
#endif

#ifdef CONFIG_DWC_PORT1_DEVICE
		printk(KERN_INFO "Set usb port 1 to device only\n");
		dwc_otg_overide(fbase, IS_DEVICE);
#endif
	}
}

/*
 * Bring a USB2.0 port out of reset.
 */
static void __init dwc_bringup_port(int inst)
{
	u32 fbase = DWC_FUNC_BASE(inst);
	u32 reg;

	printk(KERN_DEBUG "%s: %d\n", __func__, inst);

	/*
	 * Enable the clock and get the function base address 
	 */
	if (inst == 0) {
		DWC_REG_CLR(GENERAL_CFG_BASE + GEN_CLK_IO_DISABLE, DWC_CLK0_DIS_BIT);
		DWC_REG_SET(GENERAL_CFG_BASE + GEN_CLK_IO_SELECT, DWC_CLK0_SEL_BIT);
	} else {
		DWC_REG_CLR(GENERAL_CFG_BASE + GEN_CLK_IO_DISABLE, DWC_CLK1_DIS_BIT);
		DWC_REG_SET(GENERAL_CFG_BASE + GEN_CLK_IO_SELECT, DWC_CLK1_SEL_BIT);
	}


	/*
	 * signal quality settings as per latest tests (07/15/2010) 
	 */
	DWC_REG_WRITE(fbase + IO_CTL3,  
		      DWC_PHYPARAMOVR_TXHSXVTUNE(DWC_PHYPARAMOVR_TXHSXVTUNE_DEFAULT) |
		      DWC_PHYPARAMOVR_TXVREFSTUNE(DWC_PHYPARAMOVR_TXVREFSTUNE_P8_75) |
		      DWC_PHYPARAMOVR_TXFSLSTUNE(DWC_PHYPARAMOVR_TXFSLSTUNE_M5) |
		      DWC_PHYPARAMOVR_SQRXTUNE(DWC_PHYPARAMOVR_SQRXTUNE_M20) |
		      DWC_PHYPARAMOVR_COMPDISTUNE(DWC_PHYPARAMOVR_COMPDISTUNE_P4_5) |
		      DWC_PHYPARAMOVR_OTGTUNE(DWC_PHYPARAMOVR_OTGTUNE_P9)|
		      DWC_PHYPARAMOVR_PREEMPHTUNE |
		      DWC_PHYPARAMOVR_RISETUNE
          );

	DWC_REG_READ(fbase + IO_CTL3, reg);
	printk(KERN_INFO "Parameter Override Signals:  0x%08X\n", reg );

	/*
	 * Select the function but keep it in reset state
	 */
	DWC_REG_WRITE(fbase + IO_FUNC, IO_PORTX_FUNC_USB2_0 | DWC_FN_RESET_BIT);

	/*
	 * Take usb2 memory out of low leakage mode  and set master and slave AHB endianess
	 */
	DWC_REG_CLR(fbase + IO_CTL0,
		    DWC_CTL0_LOWLEAK_MODE_EN_BIT |
		    DWC_CTL0_LOWLEAK_MEM_DIS_BIT |
		    DWC_CTL0_SLV_AHB_BIGENDIAN   |

		    0
		    );

	printk(KERN_DEBUG "func_ctl0=0x%08X\n", fbase + IO_CTL0);

	/*
	 * Set the port to work in either otg, host only or device only mode
	 */
	dwc_otg_cfg_mode(fbase, inst);

	/*
	 * Take the phy port out of reset
	 */
	DWC_REG_CLR(fbase + IO_CTL1, DWC_PHYPORTRESET_BIT);
	/*
	 * clear OTG Disable
	 */
	DWC_REG_CLR(fbase + IO_CTL1, DWC_PHYOTG_DISABLE_BIT);
	/*
	 * set PhySuspendM
	 */
	DWC_REG_SET(fbase + IO_CTL1, DWC_PHYOTG_SUSPENDM_BIT);
	/*
	 * PHY enable
	*/
	DWC_REG_SET(fbase + IO_CTL1, DWC_PHYENABLE_BIT);

#ifdef CONFIG_DWC_DDMA_DESC
	/*
	 * Set endian to make non word aligned bytes to be correct value.
	 * Otherwise these bytes would be corrupted.
	 */
	DWC_REG_SET(fbase + IO_CTL1, DWC_ENDIAN_SELECT);
	printk(KERN_DEBUG "IO_CTL1 0x%08x\n", *((unsigned int *)(fbase + IO_CTL1)));
#endif

	/*
	 * Unmask function interrupt
	 */
	DWC_REG_WRITE(fbase + IO_INT_MASK, 1);

#ifdef DEBUG
	DWC_REG_READ(fbase + IO_INT_MASK, reg);
	printk(KERN_DEBUG ">>>>>>>>>> IO_INT_MASK= 0x%08X\n", reg );

	DWC_REG_READ(fbase + IO_STATUS1, reg);
	printk(KERN_DEBUG ">>>>>>>>>> IO_STATUS1= 0x%08X\n", reg );
#endif

	/*
	 * Bring the function out of reset
	 */
	DWC_REG_WRITE(fbase + IO_FUNC, IO_PORTX_FUNC_USB2_0);

}

/*
 * The 'logical module' device list for USB2.0
 */
static struct lm_device dwc_devices[] = {
	{
		.resource = {
			.start = DWC_PORT0_BASE + REG_SIZE,
			.end   = DWC_PORT0_BASE + REG_SIZE * 2,
			.flags = IORESOURCE_MEM,
		},
		.irq = RF_USB20_INT,	/* irq  for dwc_otg Port 0*/
		.id = 0,
	 },

	{
		.resource = {
			.start = DWC_PORT1_BASE + REG_SIZE,
			.end   = DWC_PORT1_BASE + REG_SIZE * 2,
			.flags = IORESOURCE_MEM,
		},
		.irq = RG_USB20_INT,	/* irq for dwc_otg Port 1*/
		.id = 1,
	 },
};


/*
 * Get the mapped irq vector index
 */
static unsigned int __init dwc_get_irq(int inst)
{
	
        /* This is to be replace by call to device tree dwc_otg module to retrieve irq based on inst*/
	return dwc_devices[inst].irq; 
}

/*
 * Init and register a usb 2.0 device
 */
static void __init dwc_init_and_register_device(int inst)
{
        printk(KERN_DEBUG "%s: %d\n", __func__, inst);
	dwc_bringup_port(inst);
	printk(KERN_INFO "USB 2.0 port %d is brought up\n", inst);

	dwc_devices[inst].irq = dwc_get_irq(inst);
	printk(KERN_INFO "USB 2.0 port %d irq = %u\n", inst, dwc_devices[inst].irq);

	lm_device_register(&dwc_devices[inst]);
}

/*
 * Usb 2.0 subsystem init
 */
static int __init ubi32_dwc_init(void)
{
        printk(KERN_INFO "%s:\n", __func__);
#ifdef CONFIG_DWC_PORT0_ENABLE
	dwc_init_and_register_device(0);
#endif
#ifdef CONFIG_DWC_PORT1_ENABLE
	dwc_init_and_register_device(1);
#endif
	return 0;
}

subsys_initcall(ubi32_dwc_init);

#define MAX_WAIT 20
#ifdef DEBUG_PERF
u32 bucket[MAX_WAIT];
#endif
/*
 * Make sure the data DMA device wrote into shared memory is ready to read
 */
void ubi32_dwc_dma_ready(void __iomem *fbasep)
{
      u32 fbase = (u32)fbasep;
      int k = MAX_WAIT;
#ifdef DEBUG_PERF
      static u32 rdy_cnt = 0; /* number of times we got ready immediately */
      static u32 wk_cnt = 0;  /* number of times we did not get ready after k attempts */
      static u32 w_cnt = 0;   /* number of times we got ready after n < k attempts */

      if ( (rdy_cnt + wk_cnt + w_cnt) == 0) memset(bucket, 0, MAX_WAIT);
#endif

#ifdef DEBUG
        struct ubicom32_io_port *ctl = (struct ubicom32_io_port *)fbase;

	printk(KERN_DEBUG "dma_ready wcmdq(%x, %p), stall(%x, %p)\n", fbase + IO_STATUS1,
		&ctl->status1, fbase + IO_CTL0, &ctl->ctl0);
#endif


        /*
	 * Polls dma_wcmdq_empty bit of STATUS1
	 */

        if (likely(DWC_REG_TST(fbase + IO_STATUS1, DWC_STATUS1_WCMDQ_EMPTY_BIT))) {
#ifdef DEBUG_PERF
	  rdy_cnt++; /* we got it immediately get countes */
#endif
                return;
	}

#ifdef DWC_DMA_STALL

        /*
	 * Tell device stop feeding till the current written data is ready
	 */
        DWC_REG_SET(fbase + IO_CTL0, DWC_CTL0_WSTALL_BIT);

#endif
        /*
	 * Keep polling till it is ready
	 */
	while (!DWC_REG_TST(fbase + IO_STATUS1, DWC_STATUS1_WCMDQ_EMPTY_BIT) && --k) {}

#ifdef DWC_DMA_STALL
        /*
	 * clear the stall bit
	 */
        DWC_REG_CLR(fbase + IO_CTL0, DWC_CTL0_WSTALL_BIT);
#endif

      
#ifdef DEBUG_PERF
      printk(KERN_WARNING "%s WCMDQ not empty, ignore!\n", __FUNCTION__);
      if (k == 0) {
	wk_cnt++; /* we never got it in the k interval */
      } 
      else {
	w_cnt++; /* we got it after a certain k count */
	bucket[k]++;
      }
#endif

}
EXPORT_SYMBOL(ubi32_dwc_dma_ready);

