/*
 * sound/ubicom32/ubi32-cs4525.c
 *	Interface to ubicom32 virtual audio peripheral - using CS4525 DAC
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
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/gpio.h>
#include <linux/proc_fs.h>

#include <asm/ubi32-cs4525.h>

#include <sound/core.h>
#include <sound/tlv.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/initval.h>

#include "ubi32.h"

#define DRIVER_NAME "snd-ubi32-cs4525"

/*
 * Module properties
 */
static const struct i2c_device_id snd_ubi32_cs4525_id[] = {
	{"cs4525", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ubicom32audio_id);

static int index = SNDRV_DEFAULT_IDX1; /* Index 0-MAX */

/*
 * The dB scale for the Cirrus Logic cs4525.  The output range is from
 * -127.5 dB to 0 dB.
 */
static const DECLARE_TLV_DB_SCALE(snd_ubi32_cs4525_db, -12750, 50, 0);

#define ubi32_cs4525_mute_info	snd_ctl_boolean_stereo_info

/*
 * Private data for cs4525 chip
 */
struct ubi32_cs4525_priv {
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

	struct proc_dir_entry *proc_root;
};

#define UBI32_CS4525_REG_CLOCKCFG		0x01
#define UBI32_CS4525_REG_INPUTCFG		0x02
#define UBI32_CS4525_REG_AUXCFG			0x03
#define UBI32_CS4525_REG_OUTPUTCFG		0x04
#define UBI32_CS4525_REG_FOLDBKCFG		0x05
#define UBI32_CS4525_REG_MIXERCFG		0x06
#define UBI32_CS4525_REG_TONECFG		0x07
#define UBI32_CS4525_REG_TONECTRL		0x08
#define UBI32_CS4525_REG_EQCFG			0x09
#define UBI32_CS4525_REG_VOLUMECFG		0x55
#define UBI32_CS4525_REG_SENSITIVITY		0x56
#define UBI32_CS4525_REG_MASTERVOL		0x57
#define UBI32_CS4525_REG_CH_A_VOL		0x58
#define UBI32_CS4525_REG_CH_B_VOL		0x59
#define UBI32_CS4525_REG_SUB_VOL 		0x5A
#define UBI32_CS4525_REG_MUTECTRL		0x5B
#define UBI32_CS4525_REG_LIMITERCFG1		0x5C
#define UBI32_CS4525_REG_LIMITERCFG2		0x5D
#define UBI32_CS4525_REG_LIMITERCFG3		0x5E
#define UBI32_CS4525_REG_POWERCTRL		0x5F
#define UBI32_CS4525_REG_INTERRUPT		0x60
#define UBI32_CS4525_REG_INTSTATUS		0x61
#define UBI32_CS4525_REG_AMPERROR		0x62
#define UBI32_CS4525_REG_DEVICEID		0x63

static const uint8_t ubi32_cs4525_vol[20] = {0xff,120,100,80,69,63,57,51,45,39,33,27,23,19,15,13,11,9,7,4};

static int snd_ubi32_cs4525_read(struct i2c_client *client, uint8_t reg, uint8_t *val)
{
	int ret;

	ret = i2c_master_send(client, &reg, 1);
	if (ret != 1) {
		snd_printk(KERN_ERR "Failed to write to CS4525 register\n");
		return -EIO;
	}
	ret = i2c_master_recv(client, val, 1);
	if (ret != 1) {
		snd_printk(KERN_ERR "Failed to read CS4525 register\n");
		return -EIO;
	}
	return 0;
}

static int snd_ubi32_cs4525_write(struct i2c_client *client, uint8_t reg, uint8_t val)
{
	uint8_t send[2];
	int ret;

	send[0] = reg;
	send[1] = val;
	ret = i2c_master_send(client, send, 2);
	if (ret != 2) {
		snd_printk(KERN_ERR "Failed to write to CS4525 register\n");
		return -EIO;
	}
	return 0;
}

/*
 * The info for the cs4525.  The volume currently has one channel,
 * and 255 possible settings.
 */
static int ubi32_cs4525_volume_info(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 255;
	return 0;
}

static int ubi32_cs4525_volume_get(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct ubi32_cs4525_priv *cs4525_priv;
	unsigned long flags;

	cs4525_priv = snd_ubi32_priv_get_drv(ubi32_priv);

	spin_lock_irqsave(&cs4525_priv->lock, flags);

	ucontrol->value.integer.value[0] = cs4525_priv->volume[0];
	ucontrol->value.integer.value[1] = cs4525_priv->volume[1];

	spin_unlock_irqrestore(&cs4525_priv->lock, flags);

	return 0;
}

static int vol_ramping(int step, uint8_t cur_vol, uint8_t new_vol)
{
	int diff = new_vol - cur_vol;

	if (abs(diff) <= abs(step)) {
		return	new_vol;
	}
	if (abs(diff) < abs(step * 2)) {
		return	cur_vol + (diff >> 1);
	}
	return	cur_vol + step;
}

static int ubi32_cs4525_volume_put(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct i2c_client *client = (struct i2c_client *)ubi32_priv->client;
	struct ubi32_cs4525_priv *cs4525_priv;
	unsigned long flags;
	int ret, changed, step;
	char send[2];
	uint8_t volume_reg_value_left, volume_reg_value_right,
		cur_vol_left, cur_vol_right;

	changed = 0;

	cs4525_priv = snd_ubi32_priv_get_drv(ubi32_priv);
	volume_reg_value_left = 255 - (uint8_t)ucontrol->value.integer.value[0];
	volume_reg_value_right = 255 - (uint8_t)ucontrol->value.integer.value[1];
	cur_vol_left = 255 - cs4525_priv->volume[0];
	cur_vol_right = 255 - cs4525_priv->volume[1];
	step = 25;	// so maximum iteration is 4 (24 may get 5)
	if (volume_reg_value_left < cur_vol_left || volume_reg_value_right < cur_vol_right) {
		step = -25;
	}

#if SND_UBI32_DEBUG
	snd_printk(KERN_INFO "Setting volume: writing %d,%d to CS4525 volume registers\n", volume_reg_value_left, volume_reg_value_right);
#endif
	spin_lock_irqsave(&cs4525_priv->lock, flags);

	while (1) {
#if SND_UBI32_DEBUG
	    printk("step %d vl %d vr %d\n", step, cur_vol_left, cur_vol_right);
#endif
	    if (cur_vol_left != volume_reg_value_left) {
		cur_vol_left = vol_ramping(step, cur_vol_left, volume_reg_value_left);
		send[1] = cur_vol_left;
		send[0] = UBI32_CS4525_REG_CH_A_VOL;
		ret = i2c_master_send(client, send, 2);
		if (ret != 2) {
			snd_printk(KERN_ERR "Failed to set channel A volume on CS4525\n");
			return changed;
		}
		cs4525_priv->volume[0] = ucontrol->value.integer.value[0];
		changed = 1;
	    }

	    if (cur_vol_right != volume_reg_value_right) {
		cur_vol_right = vol_ramping(step, cur_vol_right, volume_reg_value_right);
		send[1] = cur_vol_right;
		send[0] = UBI32_CS4525_REG_CH_B_VOL;
		ret = i2c_master_send(client, send, 2);
		if (ret != 2) {
			snd_printk(KERN_ERR "Failed to set channel B volume on CS4525\n");
			return changed;
		}
		cs4525_priv->volume[1] = ucontrol->value.integer.value[1];
		changed = 1;
	    }

	    if ((cur_vol_left == volume_reg_value_left) && (cur_vol_right == volume_reg_value_right)) {
		break;
	    }
	    step <<= 1;	// exponential stepping for Vol change
	    /*
	     * == msleep(50) which requires <linux/delay.h>. 90 ms max
	     */
	    schedule_timeout_uninterruptible(msecs_to_jiffies(50));
	}

	spin_unlock_irqrestore(&cs4525_priv->lock, flags);

	return changed;
}

static struct snd_kcontrol_new ubi32_cs4525_volume __devinitdata = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
		  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
	.name = "PCM Playback Volume",
	.info = ubi32_cs4525_volume_info,
	.get = ubi32_cs4525_volume_get,
	.put = ubi32_cs4525_volume_put,
	.tlv.p = snd_ubi32_cs4525_db,
};

static int ubi32_cs4525_mute_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct ubi32_cs4525_priv *cs4525_priv;
	unsigned long flags;

	cs4525_priv = snd_ubi32_priv_get_drv(ubi32_priv);

	spin_lock_irqsave(&cs4525_priv->lock, flags);

	ucontrol->value.integer.value[0] = cs4525_priv->mute & 1;
	ucontrol->value.integer.value[1] = (cs4525_priv->mute & (1 << 1)) ? 1 : 0;

	spin_unlock_irqrestore(&cs4525_priv->lock, flags);

	return 0;
}

static int ubi32_cs4525_mute_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct i2c_client *client = (struct i2c_client *)ubi32_priv->client;
	struct ubi32_cs4525_priv *cs4525_priv;
	unsigned long flags;
	int ret, changed;
	uint8_t submute;
	uint8_t regval;

        changed = 0;
	submute = 0;

	cs4525_priv = snd_ubi32_priv_get_drv(ubi32_priv);

	spin_lock_irqsave(&cs4525_priv->lock, flags);

	ret = snd_ubi32_cs4525_read(client, UBI32_CS4525_REG_MUTECTRL, &regval);
	if (ret) {
		snd_printk(KERN_ERR "Failed to read mute register\n");
		return changed;
	}

	if ((cs4525_priv->mute & 1) != ucontrol->value.integer.value[0]) {
		if (ucontrol->value.integer.value[0]) {
			cs4525_priv->mute |= 1;
			regval &= ~(1 << 0);
#if SND_UBI32_DEBUG
			snd_printk(KERN_INFO "Unmuted channel A\n");
#endif
		} else {
			cs4525_priv->mute &= ~1;
			regval |= (1 << 0);
			submute++;
#if SND_UBI32_DEBUG
			snd_printk(KERN_INFO "Muted channel A\n");
#endif
		}
	}

	if (((cs4525_priv->mute & 2) >> 1) != ucontrol->value.integer.value[1]) {
		if (ucontrol->value.integer.value[1]) {
			cs4525_priv->mute |= (1 << 1);
			regval &= ~(1 << 1);
#if SND_UBI32_DEBUG
			snd_printk(KERN_INFO "Unmuted channel B\n");
#endif
		} else {
			cs4525_priv->mute &= ~(1 << 1);
			regval &= ~(1 << 1);
			submute++;
#if SND_UBI32_DEBUG
			snd_printk(KERN_INFO "Muted channel B\n");
#endif
		}
	}

	changed = 1;

	if (submute == 2) {
		regval &= ~(1 << 2); // sub
		regval &= ~(1 << 3); // adc
	} else {
		regval |= (1 << 2); // sub
		regval |= (1 << 3); // adc
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_MUTECTRL, regval);
	if (ret) {
		snd_printk(KERN_ERR "Failed to write mute register\n");
		return changed;
	}

	spin_unlock_irqrestore(&cs4525_priv->lock, flags);

	return changed;
}

static struct snd_kcontrol_new ubi32_cs4525_mute __devinitdata = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE,
	.name = "PCM Playback Switch",
	.info = ubi32_cs4525_mute_info,
	.get = ubi32_cs4525_mute_get,
	.put = ubi32_cs4525_mute_put,
};

/*
 * snd_ubi32_cs4525_free
 *	Card private data free function
 */
void snd_ubi32_cs4525_free(struct snd_card *card)
{
	struct ubi32_snd_priv *ubi32_priv;
	struct ubi32_cs4525_priv *cs4525_priv;

	ubi32_priv = card->private_data;
	cs4525_priv = snd_ubi32_priv_get_drv(ubi32_priv);
	if (cs4525_priv) {
		kfree(cs4525_priv);
	}
}

/*
 * snd_ubi32_cs4525_dac_init
 */
static int snd_ubi32_cs4525_dac_init(struct i2c_client *client, const struct i2c_device_id *id,
				     struct ubi32_cs4525_platform_data *pdata)
{
	int ret;
	int i = 0;

	/*
	 * Initialize the CS4525 DAC over the I2C interface
	 */
	snd_printk(KERN_INFO "Initializing CS4525 DAC\n");

	/*
	 * Make sure to turn power off
	 */
	if (gpio_request(pdata->power_enable_pin, "CS4525_PWR")) {
		snd_printk(KERN_INFO "CS4525 could not request power pin %d\n", pdata->power_enable_pin);
		return -EBUSY;
	}
	gpio_direction_output(pdata->power_enable_pin, !pdata->power_enable_polarity);

	/*
	 * Put the DAC in reset
	 */
	if (gpio_request(pdata->reset_pin, "CS4525_RST")) {
		snd_printk(KERN_INFO "CS4525 could not request reset pin %d\n", pdata->reset_pin);
		return -EBUSY;
	}
	gpio_direction_output(pdata->reset_pin, 0);

	msleep(50);

	/*
	 * Turn power on
	 */
	gpio_direction_output(pdata->power_enable_pin, pdata->power_enable_polarity);

	msleep(150);

	/*
	 * Bring the DAC out of reset
	 */
	gpio_set_value(pdata->reset_pin, 1);

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_POWERCTRL, 0xFF);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_FOLDBKCFG, 0x7D);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_OUTPUTCFG, 0x30);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_CLOCKCFG, 0x10);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_INPUTCFG, 0x01);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_OUTPUTCFG, 0x40);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_LIMITERCFG1, 0x03);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_MASTERVOL, ubi32_cs4525_vol[5]);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_MUTECTRL, 0x00);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_POWERCTRL, 0xA0);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	ret = snd_ubi32_cs4525_write(client, UBI32_CS4525_REG_EQCFG, 0x43);
	if(ret) {
		snd_printk(KERN_WARNING "Failed to write CS4525\n");
		return ret;
	}

	return 0;
}


/*
 * snd_ubi32_cs4525_proc_write_routing
 */
static ssize_t snd_ubi32_cs4525_proc_write_routing(struct file *file, const char *buf, size_t count, loff_t *data)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	struct i2c_client *client = (struct i2c_client *)pde->data;
	unsigned char reg02;
	char *page;
	int res;

	page = kmalloc(count, GFP_KERNEL);
	if (page == NULL) {
		return -ENOBUFS;
	}

	if (copy_from_user(page, buf, count)) {
		kfree(page);
		return -EINVAL;
	}

	res = snd_ubi32_cs4525_read(client, 0x02, &reg02);
	if (res) {
		printk("Failed to read\n");
		goto done;
	}

	if (page[0] == '1') {
		reg02 &= ~0x80;
	} else {
		reg02 |= 0x80;
	}

	res = snd_ubi32_cs4525_write(client, 0x02, reg02);
	if (res) {
		printk("Failed to write\n");
		goto done;
	}
	res = count;
done:
	kfree(page);
	return res;
}

/*
 * snd_ubi32_cs4525_proc_write
 */
static ssize_t snd_ubi32_cs4525_proc_write(struct file *file, const char *buf, size_t count, loff_t *data)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	struct i2c_client *client = (struct i2c_client *)pde->data;
	char *page;
	int res;
	int i;
	char *p;

	/*
	 * Must be sent in pairs, so an odd count is bad
	 */
	if (count & 1) {
		return -EINVAL;
	}

	page = kmalloc(count, GFP_KERNEL);
	if (page == NULL) {
		return -ENOBUFS;
	}
	if (copy_from_user(page, buf, count)) {
		kfree(page);
		return -EINVAL;
	}

	p = page;
	for (i = 0; i < (count / 2); i++) {
		res = snd_ubi32_cs4525_write(client, p[0], p[1]);
		p += 2;
		if (res) {
			snd_printk(KERN_WARNING "Failed to write %02x %02x\n", p[0], p[1]);
			goto done;
		}
	}

	res = count;
done:
	kfree(page);
	return res;
}

/*
 * File operations for the proc_fs, we must cast here since proc_fs' definitions
 * differ from file_operations definitions.
 */
static struct file_operations snd_ubi32_cs4525_proc_routing_fops = {
	.write = (ssize_t (*) (struct file *, const char __user *, size_t, loff_t *))snd_ubi32_cs4525_proc_write_routing,
};

/*
 * File operations for the proc_fs, we must cast here since proc_fs' definitions
 * differ from file_operations definitions.
 */
static struct file_operations snd_ubi32_cs4525_proc_fops = {
	.write = (ssize_t (*) (struct file *, const char __user *, size_t, loff_t *))snd_ubi32_cs4525_proc_write,
};

/*
 * snd_ubi32_cs4525_i2c_probe
 */
static int snd_ubi32_cs4525_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;
	struct ubi32_cs4525_priv *cs4525_priv;
	struct ubi32_cs4525_platform_data *pdata;
	int err, ret;
	struct platform_device *pdev;

	/*
	 * pdev is the audio device
	 */
	pdev = client->dev.platform_data;
	if (!pdev) {
		return -ENODEV;
	}

	/*
	 * pdev->dev.platform_data is our platform_data
	 */
	pdata = audio_device_priv(pdev);
	if (!pdata) {
		return -ENODEV;
	}

	/*
	 * Initialize the CS4525 DAC
	 */
	ret = snd_ubi32_cs4525_dac_init(client, id, pdata);
	if (ret < 0) {
		/*
		 * Initialization failed.  Propagate the error.
		 */
		return ret;
	}

	/*
	 * Create a snd_card structure
	 */
	card = snd_card_new(index, "Ubi32-CS4525", THIS_MODULE, sizeof(struct ubi32_snd_priv));
	if (card == NULL) {
		return -ENOMEM;
	}

	card->private_free = snd_ubi32_cs4525_free; /* Not sure if correct */
	ubi32_priv = card->private_data;

	/*
	 * CS4525 DAC has a minimum sample rate of 30khz and an
	 * upper limit of 216khz for it's auto-detect.
	 */
	ubi32_priv->min_sample_rate = 30000;
	ubi32_priv->max_sample_rate = 216000;

	/*
	 * Initialize the snd_card's private data structure
	 */
	ubi32_priv->card = card;
	ubi32_priv->client = client;

	/*
	 * Create our private data structure
	 */
	cs4525_priv = kzalloc(sizeof(struct ubi32_cs4525_priv), GFP_KERNEL);
	if (!cs4525_priv) {
		snd_card_free(card);
		return -ENOMEM;
	}
	snd_ubi32_priv_set_drv(ubi32_priv, cs4525_priv);
	spin_lock_init(&cs4525_priv->lock);

	/*
	 * Initial volume is set to max by probe function
	 */
	cs4525_priv->volume[0] = 0xFF;
	cs4525_priv->volume[1] = 0xFF;

	/*
	 * The CS4525 starts off unmuted (bit set = not muted)
	 */
	cs4525_priv->mute = 3;

	/*
	 * Create the new PCM instance
	 */
	err = snd_ubi32_pcm_probe(ubi32_priv, pdev);
	if (err < 0) {
		snd_card_free(card);
		return err; /* What is err?  Need to include correct file */
	}

	strcpy(card->driver, "Ubi32-CS4525");
	strcpy(card->shortname, "Ubi32-CS4525");
	snprintf(card->longname, sizeof(card->longname),
		"%s at sendirq=%d.%d recvirq=%d.%d regs=%p", 
		card->shortname, ubi32_priv->tx_irq, ubi32_priv->irq_idx,
		ubi32_priv->rx_irq, ubi32_priv->irq_idx, ubi32_priv->adr);

	snd_card_set_dev(card, &client->dev);

	/*
	 * Set up the mixer components
	 */
	err = snd_ctl_add(card, snd_ctl_new1(&ubi32_cs4525_volume, ubi32_priv));
	if (err) {
		snd_printk(KERN_WARNING "Failed to add volume mixer control\n");
	}
	err = snd_ctl_add(card, snd_ctl_new1(&ubi32_cs4525_mute, ubi32_priv));
	if (err) {
		snd_printk(KERN_WARNING "Failed to add mute mixer control\n");
	}

	/*
	 * Register the sound card 
	 */
	if ((err = snd_card_register(card)) != 0) {
		snd_printk(KERN_WARNING "snd_card_register error\n");
	}

	/* 
	 * Store card for access from other methods 
	 */
	i2c_set_clientdata(client, card);

	/*
	 * Create the proc entry
	 */
	cs4525_priv->proc_root = proc_mkdir("cs4525", NULL);
	if (!cs4525_priv->proc_root) {
		printk(KERN_WARNING "Failed to make proc node\n");
	} else {
		struct proc_dir_entry *pde = create_proc_entry("access", S_IWUGO, cs4525_priv->proc_root);
		if (!pde) {
			printk(KERN_WARNING "Failed to create access node\n");
		} else {
			pde->data = (void *)client;
			pde->proc_fops = &snd_ubi32_cs4525_proc_fops;
		}

		pde = create_proc_entry("routing", S_IWUGO, cs4525_priv->proc_root);
		if (!pde) {
			printk(KERN_WARNING "Failed to create routing node\n");
		} else {
			pde->data = (void *)client;
			pde->proc_fops = &snd_ubi32_cs4525_proc_routing_fops;
		}
	}

	return 0;
}

/*
 * snd_ubi32_cs4525_i2c_remove
 */
static int __devexit snd_ubi32_cs4525_i2c_remove(struct i2c_client *client)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;
	struct ubi32_cs4525_priv *cs4525_priv;

	card = i2c_get_clientdata(client);

	ubi32_priv = card->private_data;
	cs4525_priv = snd_ubi32_priv_get_drv(ubi32_priv);
	snd_ubi32_pcm_remove(ubi32_priv);
	remove_proc_entry("access", cs4525_priv->proc_root);
	remove_proc_entry("routing", cs4525_priv->proc_root);
	remove_proc_entry("cs4525", NULL);

	snd_card_free(i2c_get_clientdata(client));
	i2c_set_clientdata(client, NULL);

	return 0;
}

/*
 * I2C driver description
 */
static struct i2c_driver snd_ubi32_cs4525_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.id_table	= snd_ubi32_cs4525_id,
	.probe		= snd_ubi32_cs4525_i2c_probe,
	.remove		= __devexit_p(snd_ubi32_cs4525_i2c_remove),
};

/*
 * Driver init
 */
static int __init snd_ubi32_cs4525_init(void)
{
	return i2c_add_driver(&snd_ubi32_cs4525_driver);
}
module_init(snd_ubi32_cs4525_init);

/*
 * snd_ubi32_cs4525_exit
 */
static void __exit snd_ubi32_cs4525_exit(void)
{
	i2c_del_driver(&snd_ubi32_cs4525_driver);
}
module_exit(snd_ubi32_cs4525_exit);

/*
 * Module properties
 */
MODULE_ALIAS("i2c:" DRIVER_NAME);
MODULE_AUTHOR("Patrick Tjin");
MODULE_DESCRIPTION("Driver for Ubicom32 audio devices CS4525");
MODULE_LICENSE("GPL");
