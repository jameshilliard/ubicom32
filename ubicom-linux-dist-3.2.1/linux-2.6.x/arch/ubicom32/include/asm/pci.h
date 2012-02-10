/*
 * arch/ubicom32/include/asm/pci.h
 *   Definitions of PCI operations for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 *   arch/arm
 */
#ifndef _ASM_UBICOM32_PCI_H
#define _ASM_UBICOM32_PCI_H

#include <asm/io.h>

/* The PCI address space does equal the physical memory
 * address space.  The networking and block device layers use
 * this boolean for bounce buffer decisions.
 */
#define PCI_DMA_BUS_IS_PHYS	(1)



/*
 * Perform a master read/write to the PCI bus.
 * These functions return a PCI_RESP_xxx code.
 */
extern u8 pci_read_u32(u8 pci_cmd, u32 address, u32 *data);
extern u8 pci_write_u32(u8 pci_cmd, u32 address, u32 data);
extern u8 pci_read_u16(u8 pci_cmd, u32 address, u16 *data);
extern u8 pci_write_u16(u8 pci_cmd, u32 address, u16 data);
extern u8 pci_read_u8(u8 pci_cmd, u32 address, u8 *data);
extern u8 pci_write_u8(u8 pci_cmd, u32 address, u8 data);

#define PCIBIOS_MIN_IO          0x100
#define PCIBIOS_MIN_MEM         0x10000000

#define pcibios_assign_all_busses()	0
#define pcibios_scan_all_fns(a, b)	0
extern void pcibios_resource_to_bus(struct pci_dev *dev, struct pci_bus_region *region,
	struct resource *res);

extern void pcibios_bus_to_resource(struct pci_dev *dev, struct resource *res,
	struct pci_bus_region *region);

#ifdef CONFIG_PCI
#if defined(IP8000)
/*
 * Register definition of the PCIe IP core
 */
struct pcie_core_regs {
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	volatile u32	wdma_pci_addr_0;	// 000
	volatile u32	wdma_pci_addr_1;
	volatile u32	wdma_ahb_addr;
	volatile u32	wdma_cntl;
	volatile u32	rsvd0[4];		// 010
	volatile u32	rdma_pci_addr_0;	// 020
	volatile u32	rdma_pci_addr_1;
	volatile u32	rdma_ahb_addr;
	volatile u32	rdma_cntl;
	volatile u32	rsvd1[4];		// 030
	volatile u32	imask;			// 040
	volatile u32	istatus;
	volatile u32	icmd_reg;
	volatile u32	info;
	volatile u32	imsistatus;		// 050
	volatile u32	imsiaddr;
	volatile u32	rsvd2;
	volatile u32	slot_cap_reg;
	volatile u32	deviceid;		// 060
	volatile u32	sub_id;
	volatile u32	class_code;
	volatile u32	slotcsr;
	volatile u32	prm_csr;		// 070
	volatile u32	dev_csr;
	volatile u32	link_csr;
	volatile u32	root_csr;
	volatile u32	pciahb0_ahbbase;	// 080
	volatile u32	pciahb1_ahbbase;
	volatile u32	rsvd3[2];
	volatile u32	ahbpcilo0_ahbbase;	// 090
	volatile u32	ahbpcihi0_ahbbase;
	volatile u32	ahbpcilo1_ahbbase;
	volatile u32	ahbpcihi1_ahbbase;
	volatile u32	ahbpci_timer;		// 0a0
	volatile u32	rsvd4;
	volatile u32	gen2_conf;
	volatile u32	aspmconf;
	volatile u32	pmstatus;		// 0b0
	volatile u32	pmconf2;
	volatile u32	pmconf1;
	volatile u32	pmconf0;
#else	/* CONFIG_UBICOM32_IP8K_SHUTTLE */
	volatile u32	deviceid;		// 000 ven_dev_id
	volatile u32	cfg_prmcsr;		// 004
	volatile u32	class_code;		// 008
	volatile u32	rsvd1;			// 00C
	volatile u32	bar0;			// 010
	volatile u32	bar1;			// 014
	volatile u32	bar2;			// 018
	volatile u32	bar3;			// 01c
	volatile u32	bar4;			// 020
	volatile u32	bar5;			// 024
	volatile u32	rsvd2;			// 028
	volatile u32	sub_id;			// 02c sub_sys_id
	volatile u32	pci_devscr;		// 030
	volatile u32	pcie_linkscr;		// 034
	volatile u32	tv_vc_map;		// 038
	volatile u32	bus_dev_num;		// 03c
	volatile u32	msi_cnt_sts;		// 040
	volatile u32	ltssm;			// 044
	volatile u32	pm_cap;			// 048
	volatile u32	cfg_pmcsr;		// 04c
	volatile u32	aer_ecrc_cap;		// 050
	volatile u32	vc1_cap; 		// 054
	volatile u32	max_payload;		// 058
	volatile u32	clkreq;			// 05c
	volatile u32	aspm_l0s_cap;		// 060
	volatile u32	aspm_l1_cap;		// 064
	volatile u32	time_out_comp;		// 068
	volatile u32	rsvd3;			// 06c
	volatile u32	pm_data_03;		// 070
	volatile u32	pm_data_47;		// 074
	volatile u32	pm_data_scl_03;		// 078
	volatile u32	pm_data_scl_47;		// 07c
	volatile u32	msi_map0;		// 080
	volatile u32	msi_map1;		// 084
	volatile u32	msi_map2;		// 088
	volatile u32	msi_map3;		// 08c
	volatile u32	msi_map4;		// 090
	volatile u32	msi_map5;		// 094
	volatile u32	msi_map6;		// 098
	volatile u32	msi_map7;		// 09C
	volatile u32	err_counter0;		// 0a0
	volatile u32	err_counter1;		// 0a4
	volatile u32	err_counter2;		// 0a8
	volatile u32	err_counter3;		// 0ac
	volatile u32	fc_vc0_posted;		// 0b0
	volatile u32	fc_vc0_nonposted;	// 0b4
	volatile u32	fc_vc1_posted;		// 0b8
	volatile u32	fc_vc1_nonposted;	// 0bc
	volatile u32	axi_slv_win0_addr;	// 0c0
	volatile u32	axi_slv_win0_size;	// 0c4
	volatile u32	axi_slv_win0_lsb;	// 0c8
	volatile u32	axi_slv_win0_msb;	// 0cc
	volatile u32	axi_slv_win1_addr;	// 0d0
	volatile u32	axi_slv_win1_size;	// 0d4
	volatile u32	axi_slv_win1_lsb;	// 0d8
	volatile u32	axi_slv_win1_msb;	// 0dc
	volatile u32	axi_slv_win2_addr;	// 0e0
	volatile u32	axi_slv_win2_size;	// 0e4
	volatile u32	axi_slv_win2_lsb;	// 0e8
	volatile u32	axi_slv_win2_msb;	// 0eC
	volatile u32	axi_slv_win3_addr;	// 0f0
	volatile u32	axi_slv_win3_size;	// 0f4
	volatile u32	axi_slv_win3_lsb;	// 0f8
	volatile u32	axi_slv_win3_msb;	// 0fc
	volatile u32	axi_mas_win0_addr;	// 100
	volatile u32	axi_mas_win0_size;	// 104
	volatile u32	axi_mas_win0_lsb;	// 108
	volatile u32	axi_mas_win0_msb;	// 10c
	volatile u32	axi_mas_win1_addr;	// 110
	volatile u32	axi_mas_win1_size;	// 114
	volatile u32	axi_mas_win1_lsb;	// 118
	volatile u32	axi_mas_win1_msb;	// 11c
	volatile u32	axi_mas_win2_addr;	// 120
	volatile u32	axi_mas_win2_size;	// 124
	volatile u32	axi_mas_win2_lsb;	// 128
	volatile u32	axi_mas_win2_msb;	// 12c
	volatile u32	axi_mas_win3_addr;	// 130
	volatile u32	axi_mas_win3_size;	// 134
	volatile u32	axi_mas_win3_lsb;	// 138
	volatile u32	axi_mas_win3_msb;	// 13c
	volatile u32	imask;			// 140
	volatile u32	istatus; 		// 144
	volatile u32	icmd;			// 148
	volatile u32	imsistatus;		// 14C irstatus
	volatile u32	imsiaddr;		// 150
	volatile u32	slotcap;		// 154
	volatile u32	slotcsr;		// 158
	volatile u32	rootcsr;		// 15c
	volatile u32	cfg_control;		// 160
	volatile u32	cfg_wr_data;		// 164
	volatile u32	cfg_rd_data;		// 168
	volatile u32	info;			// 16c
	volatile u32	io_control;		// 170
	volatile u32	io_addr;		// 174
	volatile u32	io_wr_data;		// 178
	volatile u32	io_rd_data;		// 17c
	volatile u32	cfg_fbe;		// 180
	volatile u32	preftch_io_win;		// 184
	volatile u32	rsvd4[31];		// was[30] replaced by [31]
	volatile u32	pcie_conf;		// Now becomes 204 instead of 200
	volatile u32	rsvd5[10];		//
	volatile u32	pcie_devscr_2;		// datasheet230
	volatile u32	pcie_linkscr_2;		// datasheet234
	volatile u32	rsvd6[10];		//
	volatile u32	aspm_l0s_cap_2;		// 260
	volatile u32	rsvd7[39];		// was [40]
	volatile u32	k_cnt_config_0;		// Now is 300
	volatile u32	k_cnt_config_1;
	volatile u32	k_cnt_config_2;
	volatile u32	k_cnt_config_3;
	volatile u32	k_cnt_config_4;
	volatile u32	k_cnt_config_5;		// 314
#endif	/* CONFIG_UBICOM32_IP8K_SHUTTLE */
};

/*
 * Abstraction of a PCIe port
 *
 *	port		0/1, instance number (0: port RJ; 1: port RK)
 *	root_bus_nr	assigned bus number
 *	conf_lock	spinlock protecting this port
 *
 * 	ctl_irq		irq number for the controller
 * 	msi_irq_base	the base irq number of all msi interrupts of the controller
 * 	msi_irq_in_use	which irqs are being allocated
 * 	msi_addr	MSI capture address
 *
 * 	mem_base	cache of dev BAR0 start
 * 	mem_mask	To speed up address match.
 *
 *	mem_space_name	name
 *	res		resource from os
 *	phy		addr of NBR registers of the phy
 *	ctl		addr of NBR registers of the controller
 *	core		addr of BR registers of the controller
 */
struct pcie_port {
	u8			port;
	u8			root_bus_nr;
	u8			is_up;
	spinlock_t		conf_lock;
	unsigned long		flags;

	int			ctl_irq;
	int			inta_irq;
	int			msi_irq_base;
	u32			msi_irq_in_use;
	u32			msi_addr;

	struct ubicom32_io_port	*phy;
	struct ubicom32_io_port	*ctl;
	struct pcie_core_regs	*core;

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	/*
	 * Make sure the internal DMA does not mess up with others on the same cache line
	 */
	u32			pad1[7];
	u32			buf;
	u32			pad2[7];
#else
	u32			wr_cnt;
	u32			wr_timestamp;
#endif

	u32			mem_base;
	u32			mem_mask;
	char			io_space_name[16];
	char			mem_space_name[16];
	struct resource	res[2];
};

#define pci_domain_nr(bus) (((struct pci_sys_data *)bus->sysdata)->pport->port)
#define pci_proc_domain(bus) (1)

/* implement the pci_ DMA API in terms of the generic device dma_ one */
#include <asm-generic/pci-dma-compat.h>

#else
#define HAVE_ARCH_PCI_SET_DMA_MAX_SEGMENT_SIZE 1
#define HAVE_ARCH_PCI_SET_DMA_SEGMENT_BOUNDARY 1

#include <asm/pci-dma-v34.h>

#endif

/*
 * Per-controller structure
 */
struct pci_sys_data {
        struct list_head node;
        int             busnr;          /* primary bus number                   */
        u64             mem_offset;     /* bus->cpu memory mapping offset       */
        unsigned long   io_offset;      /* bus->cpu IO mapping offset           */
        struct pci_bus  *bus;           /* PCI bus                              */
        struct resource *resource[3];   /* Primary PCI bus resources            */
                                        /* Bridge swizzling                     */
        u8              (*swizzle)(struct pci_dev *, u8 *);
                                        /* IRQ mapping                          */
        int             (*map_irq)(struct pci_dev *, u8, u8);
        struct hw_pci   *hw;
#if defined(IP8000)
	struct pcie_port *pport;
#endif
};

struct hw_pci {
        struct list_head buses;
        int             nr_controllers;
        int             (*setup)(int nr, struct pci_sys_data *);
        struct pci_bus *(*scan)(int nr, struct pci_sys_data *);
        void            (*preinit)(void);
        void            (*postinit)(void);
        u8              (*swizzle)(struct pci_dev *dev, u8 *pin);
        int             (*map_irq)(struct pci_dev *dev, u8 slot, u8 pin);
};

/*
 * This is the standard PCI-PCI bridge swizzling algorithm.
 */
u8 pci_std_swizzle(struct pci_dev *dev, u8 *pinp);

/*
 * Call this with your hw_pci struct to initialise the PCI system.
 */
void pci_common_init(struct hw_pci *);

static  inline struct resource *
pcibios_select_root(struct pci_dev *pdev, struct resource *res)
{
        struct resource *root = NULL;

        if (res->flags & IORESOURCE_IO)
                root = &ioport_resource;
        if (res->flags & IORESOURCE_MEM)
                root = &iomem_resource;

        return root;
}

static inline void pcibios_set_master(struct pci_dev *dev)
{
        /* No special bus mastering setup handling */
}

extern void __iomem *pci_iomap(struct pci_dev *dev, int bar, unsigned long max);
extern void pci_iounmap(struct pci_dev *dev, void __iomem *);
#endif

#endif /* _ASM_UBICOM32_PCI_H */
