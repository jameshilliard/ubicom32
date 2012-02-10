/*
 * sound/ubicom32/ubi32-wm8988.c
 *	Interface to ubicom32 virtual audio peripheral - using WM8988 DAC
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
 */

#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <sound/core.h>
#include <sound/tlv.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <asm/ubicom32.h>
#include <asm/gpio.h>
#include <asm/audio.h>
#include <asm/ubi32-wm8988.h>
#include "ubi32.h"

#define DRIVER_NAME "snd-ubi32-wm8988"

/*
 * wm8988 register cache
 * We can't read the WM8988 register space when we
 * are using 2 wire for device control, so we cache them instead.
 */
static u16 wm8988_reg[] = {
	0x0097, 0x0097, 0x0079, 0x0079,  /*  0 */
	0x0000, 0x0008, 0x0000, 0x000a,  /*  4 */
	0x0000, 0x0000, 0x00ff, 0x00ff,  /*  8 */
	0x000f, 0x000f, 0x0000, 0x0000,  /* 12 */
	0x0000, 0x007b, 0x0000, 0x0032,  /* 16 */
	0x0000, 0x00c3, 0x00c3, 0x00c0,  /* 20 */
	0x0000, 0x0000, 0x0000, 0x0000,  /* 24 */
	0x0000, 0x0000, 0x0000, 0x0000,  /* 28 */
	0x0000, 0x0000, 0x0050, 0x0050,  /* 32 */
	0x0050, 0x0050, 0x0050, 0x0050,  /* 36 */
	0x0079, 0x0079, 0x0079,          /* 40 */
};

/*
 * read wm8988 register cache
 */
static inline u16 wm8988_read_reg_cache(unsigned int reg)
{
	if (reg > WM8988_CACHE_REGNUM)
		return -1;
	return wm8988_reg[reg];
}

/*
 * write wm8988 register cache
 */
static inline void wm8988_write_reg_cache(unsigned int reg, u16 value)
{
	if (reg > WM8988_CACHE_REGNUM)
		return;
	wm8988_reg[reg] = value;
}

static int wm8988_write(struct i2c_client *client, unsigned int reg,
	u16 value)
{
	u8 data[2];

	/* data is
	 *   D15..D9 WM8753 register offset
	 *   D8...D0 register data
	 */
	data[0] = (reg << 1) | ((value >> 8) & 0x0001);
	data[1] = value & 0x00ff;

	wm8988_write_reg_cache(reg, value);

	if (i2c_master_send(client, data, 2) == 2) {
		return 0;
	}
	else
	{	snd_printk(KERN_WARNING"wm8988 i2c master send error!\n");
		return -EIO;
	}
}

static int wm8988_set_bias_level(struct i2c_client *client,
				 enum snd_soc_bias_level level)
{
	u16 pwr_reg = wm8988_read_reg_cache(WM8988_PWR1) & 0xfe3e;

	switch (level) {
	case SND_SOC_BIAS_ON:
		/* set vmid to 50k and unmute dac */
		wm8988_write(client, WM8988_PWR1, pwr_reg | 0x00c0);
		break;
	case SND_SOC_BIAS_PREPARE:
		/* set vmid to 5k for quick power up */
		wm8988_write(client, WM8988_PWR1, pwr_reg | 0x01c1);
		break;
	case SND_SOC_BIAS_STANDBY:
		/* mute dac and set vmid to 500k, enable VREF */
		wm8988_write(client, WM8988_PWR1, pwr_reg | 0x0141);
		break;
	case SND_SOC_BIAS_OFF:
		wm8988_write(client, WM8988_PWR1, 0x0001);
		break;
	}
	return 0;
}

/*
 * initialise the WM8988 driver
 * register the mixer and dsp interfaces with the kernel
 */
static int wm8988_init(struct i2c_client *client)
{
	u16 reg=0;
	int ret = 0;

	wm8988_write(client, WM8988_RESET, 0);

	/* set the update bits */ 
	reg=wm8988_read_reg_cache(WM8988_IFACE); 
	wm8988_write(client, WM8988_IFACE, reg & 0xfff7);

	reg=wm8988_read_reg_cache(WM8988_SRATE); 
	wm8988_write(client, WM8988_SRATE, reg | 0x0020);

	reg = wm8988_read_reg_cache(WM8988_LDAC);
	wm8988_write(client, WM8988_LDAC, reg | 0x0100);

	reg = wm8988_read_reg_cache(WM8988_RDAC);
	wm8988_write(client, WM8988_RDAC, reg | 0x0100);

	reg = wm8988_read_reg_cache(WM8988_LOUTM1);
	wm8988_write(client, WM8988_LOUTM1, reg | 0x0100);

	reg = wm8988_read_reg_cache(WM8988_ROUTM2);
	wm8988_write(client, WM8988_ROUTM2, reg | 0x0100);

	reg = wm8988_read_reg_cache(WM8988_LOUT2V);
	wm8988_write(client, WM8988_LOUT2V, reg | 0x017f);

	reg = wm8988_read_reg_cache(WM8988_ROUT2V);
	wm8988_write(client, WM8988_ROUT2V, reg | 0x017f);

	reg=wm8988_read_reg_cache(WM8988_PWR1); 
	wm8988_write(client, WM8988_PWR1, reg | 0x00c0);

	reg=wm8988_read_reg_cache(WM8988_PWR2); 
	wm8988_write(client, WM8988_PWR2, reg | 0x0198);

	reg=wm8988_read_reg_cache(WM8988_ADCDAC); 
	wm8988_write(client, WM8988_ADCDAC, reg & 0xfff7);

#if SND_UBI32_DEBUG
	snd_printk(KERN_INFO "wm8988 IFACE:%x\n",wm8988_reg[WM8988_IFACE]);
	snd_printk(KERN_INFO "wm8988 SRATE:%x\n",wm8988_reg[WM8988_SRATE]);
	snd_printk(KERN_INFO "wm8988 LDAC:%x\n",wm8988_reg[WM8988_LDAC]);
	snd_printk(KERN_INFO "wm8988 RDAC:%x\n",wm8988_reg[WM8988_RDAC]);
	snd_printk(KERN_INFO "wm8988 LOUTM1:%x\n",wm8988_reg[WM8988_LOUTM1]);
	snd_printk(KERN_INFO "wm8988 ROUTM2:%x\n",wm8988_reg[WM8988_ROUTM2]);
	snd_printk(KERN_INFO "wm8988 LOUT2V:%x\n",wm8988_reg[WM8988_LOUT2V]);
	snd_printk(KERN_INFO "wm8988 ROUT2V:%x\n",wm8988_reg[WM8988_ROUT2V]);
	snd_printk(KERN_INFO "wm8988 PWR1:%x\n",wm8988_reg[WM8988_PWR1]);
	snd_printk(KERN_INFO "wm8988 PWR2:%x\n",wm8988_reg[WM8988_PWR2]);
	snd_printk(KERN_INFO "wm8988 ADCDAC:%x\n",wm8988_reg[WM8988_ADCDAC]);
#endif
	return ret;
}


/*
 * snd_ubi32_wm8988_setup_mclk
 */
static int snd_ubi32_wm8988_setup_mclk(struct ubi32_wm8988_platform_data *pdata)
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
	case UBI32_WM8988_MCLK_PWM_0:
		ioc->function |= 2;
		ioc->ctl0 |= ctl0;
		ioc->ctl1 = ctlx;
		if (!ioa->function) {
			ioa->function = 3;
		}
		return 0;

	case UBI32_WM8988_MCLK_PWM_1:
		ioc->function |= 2;
		ioc->ctl0 |= ctl0 << 16;
		ioc->ctl2 = ctlx;
		if (!ioe->function) {
			ioe->function = 3;
		}
		return 0;

	case UBI32_WM8988_MCLK_PWM_2:
		ioh->ctl0 |= ctl0;
		ioh->ctl1 = ctlx;
		if (!iod->function) {
			iod->function = 3;
		}
		return 0;

	case UBI32_WM8988_MCLK_CLKDIV_1:
		ioa->gpio_mask &= ~(1 << 7);
		ioa->ctl1 &= ~(0xFF << 14);
		ioa->ctl1 |= ((div - 1) << 14);
		ioa->gpio_ctl |= (1 << 7);
		return 0;

	case UBI32_WM8988_MCLK_OTHER:
		return 0;
	}

	return 1;
}


/*
 * Module properties
 */
static const struct i2c_device_id snd_ubi32_wm8988_id[] = {
	{"wm8988", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ubicom32audio_id);

static int index = SNDRV_DEFAULT_IDX1; /* Index 0-MAX */

/*
 * The dB scale for the Cirrus Logic wm8988.  The output range is from
 * -127.5 dB to 0 dB.
 */
static const DECLARE_TLV_DB_SCALE(snd_ubi32_wm8988_db, -12750, 50, 0);

#define ubi32_wm8988_mute_info	snd_ctl_boolean_stereo_info

/*
 * Private data for wm8988 chip
 */
struct ubi32_wm8988_priv {
	/*
	 * The current volume settings
	 */
	uint8_t volume[2];

	/*
	 * Bitmask of mutes MSB (unused, ..., unused, right_ch, left_ch) LSB
	 */
	uint8_t mute;

	/*
	 * Lock to protect this struct because callbacks are not atomic.
	 */
	spinlock_t lock;
};

/*
 * The info for the wm8988.  The volume currently has one channel,
 * and 255 possible settings.
 */
static int ubi32_wm8988_volume_info(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 255; // 8 bits in cirrus logic wm8988 volume register
	return 0;
}

static int ubi32_wm8988_volume_get(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct ubi32_wm8988_priv *wm8988_priv;
	unsigned long flags;

	wm8988_priv = snd_ubi32_priv_get_drv(ubi32_priv);

	spin_lock_irqsave(&wm8988_priv->lock, flags);

	ucontrol->value.integer.value[0] = wm8988_priv->volume[0];
	ucontrol->value.integer.value[1] = wm8988_priv->volume[1];

	spin_unlock_irqrestore(&wm8988_priv->lock, flags);

	return 0;
}

static int vol_ramping(int step, uint8_t cur_vol, uint8_t new_vol)
{
	int diff = new_vol - cur_vol;

	if (abs(diff) <= abs(step)) {	// done -- last adj
		return	new_vol;
	}
	if (abs(diff) < abs(step * 2)) {
		return	cur_vol + (diff >> 1);
	}
	return	cur_vol + step;
}

static int ubi32_wm8988_volume_put(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct i2c_client *client = (struct i2c_client *)ubi32_priv->client;
	struct ubi32_wm8988_priv *wm8988_priv;
	unsigned long flags;
	int changed, step;
	uint8_t volume_reg_value_left, volume_reg_value_right,
		cur_vol_left, cur_vol_right;

	changed = 0;

	wm8988_priv = snd_ubi32_priv_get_drv(ubi32_priv);
	volume_reg_value_left = 255 - (uint8_t)ucontrol->value.integer.value[0];
	volume_reg_value_right = 255 - (uint8_t)ucontrol->value.integer.value[1];
	cur_vol_left = 255 - wm8988_priv->volume[0];
	cur_vol_right = 255 - wm8988_priv->volume[1];
	step = 25;	// so maximum iteration is 4 (24 may get 5)
	if (volume_reg_value_left < cur_vol_left || volume_reg_value_right < cur_vol_right) {
		step = -25;
	}

#if SND_UBI32_DEBUG
	snd_printk(KERN_INFO "Setting volume: writing %d,%d to WM8988 volume registers\n", volume_reg_value_left, volume_reg_value_right);
#endif
	spin_lock_irqsave(&wm8988_priv->lock, flags);

	while (1) {
#if SND_UBI32_DEBUG
	    snd_printk(KERN_INFO "step %d vl %d vr %d\n", step, cur_vol_left, cur_vol_right);
#endif
	    if (cur_vol_left != volume_reg_value_left) {
		cur_vol_left = vol_ramping(step, cur_vol_left, volume_reg_value_left);

		wm8988_write(client, WM8988_LDAC, (255-cur_vol_left)|0x100);
#if SND_UBI32_DEBUG
		snd_printk(KERN_INFO "wm8988 LDAC:%x\n",wm8988_reg[WM8988_LDAC]);
#endif
		wm8988_priv->volume[0] = ucontrol->value.integer.value[0];
		changed = 1;
	    }

	    if (cur_vol_right != volume_reg_value_right) {
		cur_vol_right = vol_ramping(step, cur_vol_right, volume_reg_value_right);

		wm8988_write(client, WM8988_RDAC, (255-cur_vol_right)|0x100);
#if SND_UBI32_DEBUG
		snd_printk(KERN_INFO "wm8988 RDAC:%x\n",wm8988_reg[WM8988_RDAC]);
#endif
		wm8988_priv->volume[1] = ucontrol->value.integer.value[1];
		changed = 1;
	    }
	    if (cur_vol_left == volume_reg_value_left && cur_vol_right == volume_reg_value_right) {
#if SND_UBI32_DEBUG
		snd_printk(KERN_INFO "wm8988:not changing!\n");
#endif
		break;
	    }
	    step <<= 1;	// exponential stepping for Vol change
	}

	spin_unlock_irqrestore(&wm8988_priv->lock, flags);

	return changed;
}

static struct snd_kcontrol_new ubi32_wm8988_volume __devinitdata = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
		  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
	.name = "PCM Playback Volume",
	.info = ubi32_wm8988_volume_info,
	.get = ubi32_wm8988_volume_get,
	.put = ubi32_wm8988_volume_put,
	.tlv.p = snd_ubi32_wm8988_db,
};

static int ubi32_wm8988_mute_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct ubi32_wm8988_priv *wm8988_priv;
	unsigned long flags;

	wm8988_priv = snd_ubi32_priv_get_drv(ubi32_priv);

	spin_lock_irqsave(&wm8988_priv->lock, flags);

	ucontrol->value.integer.value[0] = wm8988_priv->mute & 1;
	ucontrol->value.integer.value[1] = (wm8988_priv->mute & (1 << 1)) ? 1 : 0;

	spin_unlock_irqrestore(&wm8988_priv->lock, flags);

	return 0;
}

static int ubi32_wm8988_mute_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct i2c_client *client = (struct i2c_client *)ubi32_priv->client;
	struct ubi32_wm8988_priv *wm8988_priv;
	unsigned long flags;
	int changed;

	u16 reg=0;
	static int toggle=0;
	
        changed = 0;

	wm8988_priv = snd_ubi32_priv_get_drv(ubi32_priv);

	spin_lock_irqsave(&wm8988_priv->lock, flags);
#if SND_UBI32_DEBUG
	snd_printk(KERN_INFO"mute is:%d value is:%d %d\n",wm8988_priv->mute,ucontrol->value.integer.value[0],ucontrol->value.integer.value[1]);
#endif
	if(toggle==1) {
		reg=wm8988_read_reg_cache(WM8988_ADCDAC); 
		wm8988_write(client, WM8988_ADCDAC, reg & ~(1<<3));
#if SND_UBI32_DEBUG
		snd_printk(KERN_INFO "wm8988 ADCDAC:%x\n",wm8988_reg[WM8988_ADCDAC]);
#endif
		wm8988_priv->mute = 3; 
		changed=1;
	}
	else if(toggle==3) {
		reg=wm8988_read_reg_cache(WM8988_ADCDAC); 
		wm8988_write(client, WM8988_ADCDAC, reg | (1<<3));
#if SND_UBI32_DEBUG
		snd_printk(KERN_INFO "wm8988 ADCDAC:%x\n",wm8988_reg[WM8988_ADCDAC]);
#endif
		wm8988_priv->mute = 0; 
		changed=1;
	}
	else {
		changed=0;
	}
	toggle++;
	if(toggle==4) {
		toggle=0;
	}
	
	spin_unlock_irqrestore(&wm8988_priv->lock, flags);
	
	return changed;
}

static struct snd_kcontrol_new ubi32_wm8988_mute __devinitdata = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE,
	.name = "PCM Playback Switch",
	.info = ubi32_wm8988_mute_info,
	.get = ubi32_wm8988_mute_get,
	.put = ubi32_wm8988_mute_put,
};

/*
 * snd_ubi32_wm8988_free
 *	Card private data free function
 */
void snd_ubi32_wm8988_free(struct snd_card *card)
{
	struct ubi32_snd_priv *ubi32_priv;
	struct ubi32_wm8988_priv *wm8988_priv;

	ubi32_priv = card->private_data;
	wm8988_priv = snd_ubi32_priv_get_drv(ubi32_priv);
	if (wm8988_priv) {
		kfree(wm8988_priv);
	}
}

/*
 * snd_ubi32_wm8988_dac_init
 */
static int snd_ubi32_wm8988_dac_init(struct i2c_client *client, const struct i2c_device_id *id)
{
	/*
	 * Initialize the WM8988 DAC over the I2C interface
	 */
	snd_printk(KERN_INFO "Initializing WM8988 DAC\n");

	wm8988_init(client);

	snd_printk(KERN_INFO "WM8988 DAC Initialized\n");
	return 0;
}

/*
 * snd_ubi32_wm8988_i2c_probe
 */
static int snd_ubi32_wm8988_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;
	struct ubi32_wm8988_priv *wm8988_priv;
	struct ubi32_wm8988_platform_data *pdata;
	int err, ret;
	struct platform_device *pdev;

	/*
	 * pdev is audio device
	 */
	pdev = client->dev.platform_data;
	if (!pdev) {
		snd_printk(KERN_WARNING "wm8988 platform_data null!\n");
		return -ENODEV;
	}

	/*
	 * pdev->dev.platform_data is ubi32-pcm platform_data
	 */
	pdata = audio_device_priv(pdev);
	if (!pdata) {
		return -ENODEV;
	}

	/*
	 * Initialize the WM8988 DAC
	 */
	ret = snd_ubi32_wm8988_dac_init(client, id);
	snd_printk(KERN_INFO"wm8988 inited!\n");
	if (ret < 0) {
		/*
		 * Initialization failed.  Propagate the error.
		 */
		return ret;
	}

	if (snd_ubi32_wm8988_setup_mclk(pdata)) {
		return -EINVAL;
	}

	/*
	 * Create a snd_card structure
	 */
	card = snd_card_new(index, "Ubi32-WM8988", THIS_MODULE, sizeof(struct ubi32_snd_priv));
	if (card == NULL) {
		return -ENOMEM;
	}

	card->private_free = snd_ubi32_wm8988_free; /* Not sure if correct */
	ubi32_priv = card->private_data;

	/*
	 * WM8988 DAC has a minimum sample rate of 30khz and an
	 * upper limit of 96khz for its auto-detect.
	 */
	ubi32_priv->min_sample_rate = 30000;
	ubi32_priv->max_sample_rate = 96000;

	/*
	 * Initialize the snd_card's private data structure
	 */
	ubi32_priv->card = card;
	ubi32_priv->client = client;

	/*
	 * Create our private data structure
	 */
	wm8988_priv = kzalloc(sizeof(struct ubi32_wm8988_priv), GFP_KERNEL);
	if (!wm8988_priv) {
		snd_card_free(card);
		return -ENOMEM;
	}
	snd_ubi32_priv_set_drv(ubi32_priv, wm8988_priv);
	spin_lock_init(&wm8988_priv->lock);

	/*
	 * Initial volume is set to max by probe function
	 */
	wm8988_priv->volume[0] = 0xFF;
	wm8988_priv->volume[1] = 0xFF;

	/*
	 * The WM8988 starts off unmuted (bit set = not muted)
	 */
	wm8988_priv->mute = 3;

	/*
	 * Create the new PCM instance
	 */
	err = snd_ubi32_pcm_probe(ubi32_priv, pdev);
	if (err < 0) {
		snd_card_free(card);
		snd_printk(KERN_WARNING"wm8988 pcm probe failed!\n");
		return err; /* What is err?  Need to include correct file */
	}

	strcpy(card->driver, "Ubi32-WM8988");
	strcpy(card->shortname, "Ubi32-WM8988");
	snprintf(card->longname, sizeof(card->longname),
		"%s at sendirq=%d.%d recvirq=%d.%d regs=%p", 
		card->shortname, ubi32_priv->tx_irq, ubi32_priv->irq_idx,
		ubi32_priv->rx_irq, ubi32_priv->irq_idx, ubi32_priv->adr);

	snd_card_set_dev(card, &client->dev);

	/*
	 * Set up the mixer components
	 */
	err = snd_ctl_add(card, snd_ctl_new1(&ubi32_wm8988_volume, ubi32_priv));
	if (err) {
		snd_printk(KERN_WARNING "wm8988 Failed to add volume mixer control\n");
	}
	err = snd_ctl_add(card, snd_ctl_new1(&ubi32_wm8988_mute, ubi32_priv));
	if (err) {
		snd_printk(KERN_WARNING "wm8988 Failed to add mute mixer control\n");
	}
	
	/*
	 * Register the sound card 
	 */
	if ((err = snd_card_register(card)) != 0) {
		snd_printk(KERN_WARNING"wm8988 snd card register failed!\n");
		snd_printk(KERN_WARNING "snd_card_register error\n");
	}

	/* 
	 * Store card for access from other methods 
	 */
	i2c_set_clientdata(client, card);

	return 0;
}

/*
 * snd_ubi32_wm8988_i2c_remove
 */
static int __devexit snd_ubi32_wm8988_i2c_remove(struct i2c_client *client)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;

	card = i2c_get_clientdata(client);

	ubi32_priv = card->private_data;
	snd_ubi32_pcm_remove(ubi32_priv);

	snd_card_free(i2c_get_clientdata(client));
	i2c_set_clientdata(client, NULL);

	return 0;
}

/*
 * I2C driver description
 */
static struct i2c_driver snd_ubi32_wm8988_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.id_table	= snd_ubi32_wm8988_id,
	.probe		= snd_ubi32_wm8988_i2c_probe,
	.remove		= __devexit_p(snd_ubi32_wm8988_i2c_remove),
};

/*
 * Driver init
 */
static int __init snd_ubi32_wm8988_init(void)
{
	return i2c_add_driver(&snd_ubi32_wm8988_driver);
}
module_init(snd_ubi32_wm8988_init);

/*
 * snd_ubi32_wm8988_exit
 */
static void __exit snd_ubi32_wm8988_exit(void)
{
	i2c_del_driver(&snd_ubi32_wm8988_driver);
}
module_exit(snd_ubi32_wm8988_exit);

/*
 * Module properties
 */
MODULE_ALIAS("i2c:" DRIVER_NAME);
MODULE_AUTHOR("Patrick Tjin");
MODULE_DESCRIPTION("Driver for Ubicom32 audio devices WM8988");
MODULE_LICENSE("GPL");
