// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Copyright (C) 2004-2008, 2012 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
 */

#include <asm/io.h>
#include <config.h>
#include <common.h>
#include <spi.h>

DECLARE_GLOBAL_DATA_PTR;

#define PIT_BASE 0x20000000
#define PCDDR 9
#define PCDR 25


int checkboard(void)
{
	puts("Board: ");
	puts("SBC030\n");
	return 0;
};

int dram_init(void)
{
	gd->ram_size = 128 * 1024 * 1024;

	return 0;
};

/*
 * PC7 - MOSI
 * PC6 - MISO
 * PC4 - CS
 * PC2 - SCK
 */

#define PC7 (1<<7)
#define PC6 (1<<6)
#define PC4 (1<<4)
#define PC2 (1<<2)

int board_late_init(void)
{
	unsigned char dir = 0;
	dir |= PC7 | PC4 | PC2;
	writeb(0xff, PIT_BASE + PCDR);
	writeb(dir, PIT_BASE + PCDDR);
	return 0;
}

static void set_pin(unsigned char pin, unsigned char set)
{
	unsigned char reg;
	reg = readb(PIT_BASE + PCDR);

	if(set)
		reg |= pin;
	else
		reg &= ~pin;

	writeb(reg, PIT_BASE + PCDR);

}

#ifdef CONFIG_CMD_SPI

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return 1;
}

void spi_cs_activate(struct spi_slave *slave)
{
	set_pin(PC4, 0);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	set_pin(PC4, 1);
}

void sbc030_spi_sda(int set)
{
	set_pin(PC7, set);
}

void sbc030_spi_scl(int set)
{
	set_pin(PC2, set);
}

unsigned char sbc030_spi_read(void)
{
	return !!(readb(PIT_BASE + PCDR) & PC6);
}

#endif /* CONFIG_CMD_SPI */

#ifdef CONFIG_FLASH_CFI_LEGACY
ulong board_flash_get_legacy(ulong base, int banknum, flash_info_t *info)
{
	if (banknum == 0) {	/* non-CFI boot flash */
		info->portwidth = FLASH_CFI_8BIT;
		info->chipwidth = FLASH_CFI_BY8;
		info->interface = FLASH_CFI_X8;
		return 1;
	} else {
		return 0;
	}
}
#endif /* CONFIG_FLASH_CFI_LEGACY */
