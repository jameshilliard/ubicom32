/*
 * Command for accessing gpio pins.
 *
 * Copyright (C) 2009 Ubicom, Inc.
 */
#include <common.h>
#include <asm/gpio.h>

static int do_gpio(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	const char *cmd;
	unsigned long pin;
	char *endp;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	pin = simple_strtoul(argv[1], &endp, 10);

	if (argc == 2) {
		int value = UBICOM32_GPIO_GET_PIN(pin);
		printf("GPIO(%lu)=%d\n", pin, value);
		return 0;
	}

	cmd = argv[2];

	if (strcmp(cmd, "enable") == 0) {
		UBICOM32_GPIO_ENABLE(pin);
		return 0;
	}

	if (strcmp(cmd, "disable") == 0) {
		UBICOM32_GPIO_DISABLE(pin);
		return 0;
	}

	if (strcmp(cmd, "input") == 0) {
		UBICOM32_GPIO_SET_PIN_INPUT(pin);
		return 0;
	}

	if (strcmp(cmd, "output") == 0) {
		UBICOM32_GPIO_SET_PIN_OUTPUT(pin);
		return 0;
	}

	if (strcmp(cmd, "toggle") == 0) {
		UBICOM32_GPIO_SET_PIN_TOGGLE(pin);
		return 0;
	}

	if (strcmp(cmd, "high") == 0 || strcmp(cmd, "1") == 0) {
		UBICOM32_GPIO_SET_PIN_HIGH(pin);
		return 0;
	}

	if (strcmp(cmd, "low") == 0 || strcmp(cmd, "0") == 0) {
		UBICOM32_GPIO_SET_PIN_LOW(pin);
		return 0;
	}

	if (strcmp(cmd, "get") == 0) {
		int value = UBICOM32_GPIO_GET_PIN(pin);
		printf("GPIO(%lu)=%d\n", pin, value);
		return 0;
	}

usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

U_BOOT_CMD(
	gpio,	3,	1,	do_gpio,
	"configure and use gpio pins",
	"<pin-nr> [<cmd>] - read the input from <pin-nr> or issue a command\n"
	"<pin-nr> - 0+pin for Port A pins, 32+pin for port B pins, 64+pin for port C...\n"
	"           Example: pin-nr 98 is Port D, Pin 2 (same as in Linux)\n"
	"<cmd>    - enable/disable, input/output, toggle/high/low (be careful!!!)\n");

