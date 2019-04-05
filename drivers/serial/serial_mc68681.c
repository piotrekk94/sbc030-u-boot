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

#define DUART_BASE 0xF0000000
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

struct brg_spec {
	int baud;
	uint8_t cr1;
	uint8_t cr2;
	uint8_t acr;
	uint8_t csr;
};

static struct brg_spec uart_brg_settings[] = {
	{1200, 0x90, 0xb0, 0x30, 0x66},
	{2400, 0x90, 0xb0, 0x30, 0x88},
	{4800, 0x90, 0xb0, 0x30, 0x99},
	{9600, 0x90, 0xb0, 0x30, 0xbb},
	{19200, 0x90, 0xb0, 0xb0, 0xcc},
	{38400, 0x90, 0xb0, 0x30, 0xcc},
	{115200, 0x80, 0xa0, 0x30, 0x88},
};

static void duart_generic_setbrg(int baud, int port)
{
	int i;
	
	for(i = 0; i < ARRAY_SIZE(uart_brg_settings); i++){
		if(baud == uart_brg_settings[i].baud){
			writeReg(DUART_CR + port * 16, uart_brg_settings[i].cr1);
			writeReg(DUART_CR + port * 16, uart_brg_settings[i].cr2);
			writeReg(DUART_ACR + port * 16, uart_brg_settings[i].acr);
			writeReg(DUART_CSR + port * 16, uart_brg_settings[i].csr);
			return;
		}		
	}
}

static void duart_port_init(int port)
{
	writeReg(DUART_MR1 + port * 16, 0x13);
	writeReg(DUART_MR2 + port * 16, 0x07);
	writeReg(DUART_CR + port * 16, 0x05);
}

static int duart_serial_init(void)
{
	duart_generic_setbrg(CONFIG_BAUDRATE, 0);
	duart_port_init(0);

	duart_generic_setbrg(38400, 1);
	duart_port_init(1);

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
	duart_generic_setbrg(baud, 0);
}

static int duart_serial_tstc(void)
{
	return readReg(DUART_SR) & (1<<0);
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
