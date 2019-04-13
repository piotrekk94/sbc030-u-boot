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
#include <netdev.h>

DECLARE_GLOBAL_DATA_PTR;

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

int board_late_init(void)
{
	return 0;
}

#ifdef CONFIG_MMC
int board_mmc_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_MMC_SPI
	struct mmc *mmc;
	mmc = mmc_spi_init(0, 0, 25000000, 0);
	if(mmc)
		mmc_init(mmc);
#endif
	return rc;
}
#endif

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_ENC28J60
	rc = enc28j60_initialize(1, 0, 20000000, 0);
#endif
	return rc;
}
#endif

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
