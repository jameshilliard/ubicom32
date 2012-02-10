/*
 * sound/ubicom32/ubi32-generic.c
 *	Interface to ubicom32 virtual audio peripheral
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
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include <asm/ubicom32sound.h>

#include <asm/ubicom32.h>
#include <asm/ubi32-generic.h>

#define DRIVER_NAME "snd-ubi32-generic"

/*
 * Module properties
 */
static int index = SNDRV_DEFAULT_IDX1; /* Index 0-MAX */

/*
 * snd_ubi32_generic_setup_mclk
 */
static int snd_ubi32_generic_setup_mclk(struct ubi32_generic_platform_data *pdata)
{
	struct ubicom32_io_port *ioa = (struct ubicom32_io_port *)IO_PORT_RA;
	struct ubicom32_io_port *ioc = (struct ubicom32_io_port *)IO_PORT_RC;
	struct ubicom32_io_port *iod = (struct ubicom32_io_port *)IO_PORT_RD;
	struct ubicom32_io_port *ioe = (struct ubicom32_io_port *)IO_PORT_RE;
	struct ubicom32_io_port *ioh = (struct ubicom32_io_port *)IO_PORT_RH;
	unsigned int ctl0;
	unsigned int ctlx;
	unsigned int div;

	div = pdata->mclk_entries[0].div;

	ctl0 = (1 << 13);
	ctlx = ((div - 1) << 16) | (div / 2);

	switch (pdata->mclk_src) {
	case UBI32_GENERIC_MCLK_PWM_0:
		ioc->function |= 2;
		ioc->ctl0 |= ctl0;
		ioc->ctl1 = ctlx;
		if (!ioa->function) {
			ioa->function = 3;
		}
		return 0;

	case UBI32_GENERIC_MCLK_PWM_1:
		ioc->function |= 2;
		ioc->ctl0 |= ctl0 << 16;
		ioc->ctl2 = ctlx;
		if (!ioe->function) {
			ioe->function = 3;
		}
		return 0;

	case UBI32_GENERIC_MCLK_PWM_2:
		ioh->ctl0 |= ctl0;
		ioh->ctl1 = ctlx;
		if (!iod->function) {
			iod->function = 3;
		}
		return 0;

	case UBI32_GENERIC_MCLK_CLKDIV_1:
		ioa->gpio_mask &= ~(1 << 7);
		ioa->ctl1 &= ~(0xFF << 14);
		ioa->ctl1 |= ((div - 1) << 14);
		ioa->gpio_ctl |= (1 << 7);
		return 0;

	case UBI32_GENERIC_MCLK_OTHER:
		return 0;
	}

	return 1;
}

/*
 * Card private data free function
 */
void snd_ubi32_generic_free(struct snd_card *card)
{
	// Nothing to free if no mclk because ubi32_priv just maintains pointers
}

/*
 * Ubicom audio driver probe() method.  Args change depending on whether we use
 * platform_device or i2c_device.
 */
static int snd_ubi32_generic_probe(struct platform_device *dev)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;
	int err;

	struct ubi32_generic_platform_data *pdata;

	/*
	 * dev is audio device
	 */
	if (!dev) {
		snd_printk(KERN_WARNING "generic platform_data null!\n");
		return -ENODEV;
	}

	/*
	 * setup mclk if pdata exists
	 */
	pdata = audio_device_priv(dev);
	if (pdata) {
		if (snd_ubi32_generic_setup_mclk(pdata)) {
			snd_printk(KERN_WARNING "can not setup mclk!\n");
			return -EINVAL;
		}
	}

	/*
	 * Create a snd_card structure
	 */
	card = snd_card_new(index, "Ubi32-Generic", THIS_MODULE, sizeof(struct ubi32_snd_priv));

	if (card == NULL) {
		return -ENOMEM;
	}

	card->private_free = snd_ubi32_generic_free; /* Not sure if correct */
	ubi32_priv = card->private_data;

	/*
	 * Initialize the snd_card's private data structure
	 */
	ubi32_priv->card = card;

	/*
	 * Create the new PCM instance
	 */
	err = snd_ubi32_pcm_probe(ubi32_priv, dev);
	if (err < 0) {
		snd_card_free(card);
		return err;
	}

	strcpy(card->driver, "Ubi32-Generic");
	strcpy(card->shortname, "Ubi32-Generic");
	snprintf(card->longname, sizeof(card->longname),
		"%s at sendirq=%d.%d recvirq=%d.%d regs=%p",
		card->shortname, ubi32_priv->tx_irq, ubi32_priv->irq_idx,
		ubi32_priv->rx_irq, ubi32_priv->irq_idx, ubi32_priv->adr);

	snd_card_set_dev(card, &dev->dev);

	/* 
	 * Register the sound card
	 */
	if ((err = snd_card_register(card)) != 0) {
		snd_printk(KERN_INFO "snd_card_register error\n");
	}

	/* 
	 * Store card for access from other methods
	 */
	platform_set_drvdata(dev, card);

	return 0;
}

/*
 * Ubicom audio driver remove() method
 */
static int __devexit snd_ubi32_generic_remove(struct platform_device *dev)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;

	card = platform_get_drvdata(dev);
	ubi32_priv = card->private_data;
	snd_ubi32_pcm_remove(ubi32_priv);

	snd_card_free(platform_get_drvdata(dev));
	platform_set_drvdata(dev, NULL);
	return 0;
}

/*
 * Platform driver definition
 */
static struct platform_driver snd_ubi32_generic_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.probe = snd_ubi32_generic_probe,
	.remove = __devexit_p(snd_ubi32_generic_remove),
};

/*
 * snd_ubi32_generic_init
 */
static int __init snd_ubi32_generic_init(void)
{
	return platform_driver_register(&snd_ubi32_generic_driver);
}
module_init(snd_ubi32_generic_init);

/*
 * snd_ubi32_generic_exit
 */
static void __exit snd_ubi32_generic_exit(void)
{
	platform_driver_unregister(&snd_ubi32_generic_driver);
}
module_exit(snd_ubi32_generic_exit);

/*
 * Module properties
 */
MODULE_AUTHOR("Aaron Jow, Patrick Tjin");
MODULE_DESCRIPTION("Driver for Ubicom32 audio devices");
MODULE_LICENSE("GPL");
