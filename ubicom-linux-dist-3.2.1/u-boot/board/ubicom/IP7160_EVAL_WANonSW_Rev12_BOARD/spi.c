/*
 * SPI interface functions
 *
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <asm/gpio.h>
#include <spi.h>

struct spi_config {
	int pin_mosi;
	int pin_miso;
	int pin_cs;
	int pin_clk;
	int pin_reset;

	int clk_default;
	int num_chip_select;
	int bus_num;
} spi_config = { GPIO_RE_0, GPIO_RE_1, GPIO_RE_2, GPIO_RE_3, GPIO_RE_4, 
		 1, 1, 0 };

int  spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs < spi_config.num_chip_select;
}

int board_spi_read(void)
{
	return UBICOM32_GPIO_GET_PIN(spi_config.pin_miso);
}

void board_spi_sda(int bit)
{
	UBICOM32_GPIO_SET_PIN(spi_config.pin_mosi, bit);
}

void board_spi_scl(int bit)
{
	UBICOM32_GPIO_SET_PIN(spi_config.pin_clk, bit);
}

void board_spi_delay(void)
{
	/*
	 * XXX this is only half a clock wave, so by doing a udelay(1) here
	 * we limit the spi clock to 500KHz.
	 */
	udelay(1);
}

void spi_cs_activate(struct spi_slave *slave)
{
	/*
	 * Because the linux driver uses SPI_CS_HIGH.
	 */
	UBICOM32_GPIO_SET_PIN(spi_config.pin_cs, 0);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	UBICOM32_GPIO_SET_PIN(spi_config.pin_cs, 1);
}

int spi_write_then_read(struct spi_device *spi,
                const u8 *txbuf, unsigned n_tx,
                u8 *rxbuf, unsigned n_rx)
{
	int ret;
	/*
	 * Linux says max is 32 byte transfers, but we need in and out buf
	 * and we want to reuse spi_xfer so we just use one big array for both.
	 */
	u8 buf[64];

	memcpy(buf, txbuf, n_tx);
	ret = spi_xfer((struct spi_slave *)spi, (n_tx+n_rx)<<3, buf, buf, SPI_XFER_BEGIN|SPI_XFER_END);
	memcpy(rxbuf, buf+n_tx, n_rx);
	return ret;
}

void board_spi_init(void)
{
	/*
	 * Rev 1.2 boards have spi in a different place than 1.1/1.0
	 */
	if (strcmp(board_get_revision(), "1.2") == 0) {
		spi_config.pin_mosi = GPIO_RD_7;
	}

	/*
	 * Initialize the spi gpios.
	 */
	UBICOM32_GPIO_ENABLE(spi_config.pin_mosi);
	UBICOM32_GPIO_SET_PIN_OUTPUT(spi_config.pin_mosi);
	UBICOM32_GPIO_SET_PIN(spi_config.pin_mosi, 0);

	UBICOM32_GPIO_ENABLE(spi_config.pin_miso);
	UBICOM32_GPIO_SET_PIN_INPUT(spi_config.pin_miso);

	UBICOM32_GPIO_ENABLE(spi_config.pin_clk);
	UBICOM32_GPIO_SET_PIN_OUTPUT(spi_config.pin_clk);
	UBICOM32_GPIO_SET_PIN(spi_config.pin_clk, spi_config.clk_default);

#ifdef CONFIG_SWITCH_BCM539X
	/*
	 * Chip select is active low.
	 */
	UBICOM32_GPIO_ENABLE(spi_config.pin_cs);
	UBICOM32_GPIO_SET_PIN_OUTPUT(spi_config.pin_cs);
	UBICOM32_GPIO_SET_PIN(spi_config.pin_cs, 1);

	/*
	 * Reset is active low as well.
	 * A 10us pulse with 20us recovery is good enough.
	 */
	UBICOM32_GPIO_ENABLE(spi_config.pin_reset);
	UBICOM32_GPIO_SET_PIN_OUTPUT(spi_config.pin_reset);
	UBICOM32_GPIO_SET_PIN(spi_config.pin_reset, 0);
	udelay(10);
	UBICOM32_GPIO_SET_PIN(spi_config.pin_reset, 1);
	udelay(20);
#endif
}

