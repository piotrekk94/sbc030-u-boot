// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew, Tsi-Chung.Liew@freescale.com.
 *
 * Modified to add device model (DM) support
 * (C) Copyright 2015  Angelo Dureghello <angelo@sysam.it>
 */

/*
 * Minimal serial functions needed to use one of the uart ports
 * as serial console interface.
 */

#include <common.h>
#include <serial.h>
#include <linux/compiler.h>

DECLARE_GLOBAL_DATA_PTR;

#define DUART_BASE 0x30000000
#define DUART_MR1 1
#define DUART_MR2 1
#define DUART_CSR 3
#define DUART_SR  3
#define DUART_CR  5
#define DUART_RB  7
#define DUART_TB  7
#define DUART_ACR 9

static inline void writeReg(uint8_t n, uint8_t data){
	volatile uint8_t *reg = (uint8_t*)(DUART_BASE + n);
	*reg = data;
}

static inline uint8_t readReg(uint8_t n){
	volatile uint8_t *reg = (uint8_t*)(DUART_BASE + n);
	return *reg;
}

static void duart_generic_setbrg(int baud)
{
	switch(baud){
	case 1200:
	writeReg(DUART_ACR, 0x30);
	writeReg(DUART_CSR, 0x66);
	break;
	case 2400:
	writeReg(DUART_ACR, 0x30);
	writeReg(DUART_CSR, 0x88);
	break;
	case 4800:
	writeReg(DUART_ACR, 0x30);
	writeReg(DUART_CSR, 0x99);
	break;
	case 9600:
	writeReg(DUART_ACR, 0x30);
	writeReg(DUART_CSR, 0xBB);
	break;
	case 19200:
	writeReg(DUART_ACR, 0xB0);
	writeReg(DUART_CSR, 0xCC);
	break;
	case 38400:
	default:
	writeReg(DUART_ACR, 0x30);
	writeReg(DUART_CSR, 0xCC);
	break;
	}
}

static int duart_serial_init(void)
{
	duart_generic_setbrg(CONFIG_BAUDRATE);
	writeReg(DUART_MR1, 0x13);
	writeReg(DUART_MR2, 0x07);
	writeReg(DUART_CR, 0x05);

	return 0;
}

static void duart_serial_putc(const char c)
{
	if (c == '\n')
		serial_putc('\r');

	uint8_t reg;
	do{
		reg = readReg(DUART_SR);
	}while((reg & (1<<2)) == 0);
	writeReg(DUART_TB, c);
}

static int duart_serial_getc(void)
{
	uint8_t reg;
	do{
		reg = readReg(DUART_SR);
	}while((reg & (1<<0)) == 0);
	return readReg(DUART_RB);
}

static void duart_serial_setbrg(void)
{
	int baud = gd->baudrate;
	duart_generic_setbrg(baud);
}

static int duart_serial_tstc(void)
{
	return readReg(DUART_SR) & (1<<2);
}

static struct serial_device duart_serial_drv = {
	.name	= "duart_serial",
	.start	= duart_serial_init,
	.stop	= NULL,
	.setbrg	= duart_serial_setbrg,
	.putc	= duart_serial_putc,
	.puts	= default_serial_puts,
	.getc	= duart_serial_getc,
	.tstc	= duart_serial_tstc,
};

void duart_serial_initialize(void)
{
	serial_register(&duart_serial_drv);
}

__weak struct serial_device *default_serial_console(void)
{
	return &duart_serial_drv;
}
