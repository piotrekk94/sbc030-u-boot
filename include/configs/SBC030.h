/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuation settings for the Freescale MCF5208EVBe.
 *
 * Copyright (C) 2004-2008 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
 */

#ifndef _SBC030_H
#define _SBC030_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_SYS_LOAD_ADDR	0x00000000

#define CONFIG_SYS_CLK		24000000 /* CPU Core Clock */
/*
 * Low Level Configuration Settings
 * (address mappings, register initial values, etc.)
 * You should know what you are doing if you make changes here.
 */
/* Definitions for initial stack pointer and data area (in DPRAM) */
#define CONFIG_SYS_INIT_RAM_ADDR	0xF8000000
#define CONFIG_SYS_INIT_RAM_SIZE	0x4000	/* Size of used area in internal SRAM */
#define CONFIG_SYS_GBL_DATA_OFFSET	((CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE) - 0x10)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_MONITOR_BASE		(CONFIG_SYS_SDRAM_BASE + 127 * 1024 * 1024 + 0x400)
#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor */

#define CONFIG_SYS_BOOTPARAMS_LEN	64*1024
#define CONFIG_SYS_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc() */

#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define CONFIG_SYS_NUM_IRQS		64

#define CONFIG_DUART
#define CONFIG_SYS_DUART_BASE		0xf0000000
#define CONFIG_SYS_DUART_CLK		3686400
#define CONFIG_SYS_DUART_IRQ		25

#define CONFIG_PIT
#define CONFIG_SYS_PIT_BASE		0xf4000000
#define CONFIG_SYS_PIT_CLK		3686400
#define CONFIG_SYS_PIT_IRQ		26

#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_LEGACY
#define CONFIG_SYS_FLASH_LEGACY_512Kx8
#define CONFIG_SYS_FLASH_BASE		0xFC000000
#define CONFIG_SYS_FLASH_SIZE		0x80000
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	128
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE, }

#define CONFIG_ENV_SIZE			0x1000
#define CONFIG_ENV_ADDR			(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_RAM_SIZE)

#define CONFIG_EXTRA_ENV_SETTINGS \
	"update=dhcp $kernel_addr u-boot.bin && mtd erase nor0 && mtd write nor0 $kernel_addr && reset\0" \
	"ramdisk=dhcp $ramdisk_addr ramdisk.bin && setenv ramdisk_size $filesize\0" \
	"kernel=dhcp $kernel_addr linux.bin && setenv kernel_size $filesize\0" \
	"ram_start=0\0" \
	"ram_size=8000000\0" \
	"ramdisk_addr=1000000\0" \
	"kernel_addr=1000\0" \
	"linux=run ramdisk && run kernel && boot68 $kernel_addr $kernel_size $ram_start $ram_size $ramdisk_addr $ramdisk_size && dcache off && icache off && go $kernel_addr\0" \
	"bootmenu_0=Boot Linux using TFTP=run linux\0" \
	"bootmenu_1=Update U-Boot using TFTP=run update\0" \
	"bootmenu_delay=2\0" \
	"bootargs=console=ttyS0\0" \
	"ethaddr=98:5d:ad:43:dd:38"

#define CONFIG_BOOTCOMMAND \
	"bootmenu"

/* Cache Configuration */
#define CONFIG_SYS_CACHELINE_SIZE	16

#define ICACHE_STATUS			(CONFIG_SYS_INIT_RAM_ADDR + \
					 CONFIG_SYS_INIT_RAM_SIZE - 8)
#define DCACHE_STATUS			(CONFIG_SYS_INIT_RAM_ADDR + \
					 CONFIG_SYS_INIT_RAM_SIZE - 4)

#endif /* _SBC030_H */
