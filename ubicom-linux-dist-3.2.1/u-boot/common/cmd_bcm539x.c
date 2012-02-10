/*
 * Command for accessing the bcm539x chip over spi.
 *
 * Copyright (C) 2009 Ubicom, Inc.
 */
#include <common.h>
#include <spi.h>
#include <asm/switch-bcm539x.h>

static int do_bcm539x(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 0;
	int inst = 0;
	int write = 0;
	int device = 0;
	const char *cmd;
	char *buf;
	char *endp;
	char out[64];
	struct spi_slave *slave;
	struct switch_device *dev;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	        /* FIXME: Make these parameters run-time configurable */
        slave = spi_setup_slave(CONFIG_DEFAULT_SPI_BUS, device, 1000000,
                        CONFIG_DEFAULT_SPI_MODE);
        if (!slave) {
                printf("Invalid device %d, giving up.\n", device);
                return 1;
        }

        spi_claim_bus(slave);

	dev = bcm539x_probe((struct spi_device *)slave);
        if (!dev) {
                printf("Probe failed.\n");
		ret = 1;
                goto nodev;
        }


	if (strcmp(cmd, "probe") == 0) {
		goto done;
	}

	if (strcmp(cmd, "reset") == 0) {
		bcm539x_handle_reset(dev, NULL, 0);
		goto done;
	}

	write = argc > 2;
	buf = write ? argv[2] : out;
	inst = argc > 3 ? simple_strtol(argv[3], &endp, 10) : 0;

	if (strcmp(cmd, "enable") == 0) {
		if (write)
			bcm539x_handle_enable_write(dev, buf, inst);
		else {
			bcm539x_handle_enable_read(dev, buf, inst);
			printf("read returned %s", buf);
		}
		goto done;
	}

	if (strcmp(cmd, "enable_vlan") == 0) {
		if (write)
			bcm539x_handle_enable_vlan_write(dev, buf, inst);
		else {
			bcm539x_handle_enable_vlan_read(dev, buf, inst);
			printf("read returned %s", buf);
		}
		goto done;
	}

	if (strcmp(cmd, "vlan_ports") == 0) {
		if (write)
			bcm539x_handle_vlan_ports_write(dev, buf, inst);
		else {
			bcm539x_handle_vlan_ports_read(dev, buf, inst);
			printf("read returned %s", buf);
		}
		goto done;
	}

done:
	bcm539x_remove(dev);
nodev:
	spi_release_bus(slave);
	spi_free_slave(slave);
	return ret;

usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

U_BOOT_CMD(
	bcm539x,	3,	1,	do_bcm539x,
	"configure and use the bcm539x switch",
	"probe		- check availability\n");

