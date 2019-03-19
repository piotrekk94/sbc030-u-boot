// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2003 Josef Baumgartner <josef.baumgartner@telex.de>
 *
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include <common.h>

#include <asm/timer.h>
#include <asm/immap.h>
#include <watchdog.h>

DECLARE_GLOBAL_DATA_PTR;

static volatile ulong timestamp = 0;

#ifndef CONFIG_SYS_WATCHDOG_FREQ
#define CONFIG_SYS_WATCHDOG_FREQ (CONFIG_SYS_HZ / 2)
#endif

#if defined(CONFIG_PIT)

#define PIT_TCR 0x21
#define PIT_CPRH 0x27
#define PIT_CPRM 0x29
#define PIT_CPRL 0x2b
#define PIT_CRH 0x2f
#define PIT_CRM 0x31
#define PIT_CRL 0x33
#define PIT_TSR 0x35
#define PIT_PRESCALER 32

#define DUART_IMR 11
#define DUART_ISR 11
#define DUART_CTUR 13
#define DUART_CTLR 15
#define DUART_START 29
#define DUART_STOP 31
#define DUART_MAX_DELAY (unsigned long)(4.3*65535)

static inline void writeRegPIT(uint8_t n, uint8_t data){
	volatile uint8_t *reg = (uint8_t*)(CONFIG_SYS_PIT_BASE + n);
	*reg = data;
}

static inline void writeRegDUART(uint8_t n, uint8_t data){
	volatile uint8_t *reg = (uint8_t*)(CONFIG_SYS_DUART_BASE + n);
	*reg = data;
}

static inline uint8_t readRegDUART(uint8_t n){
	volatile uint8_t *reg = (uint8_t*)(CONFIG_SYS_DUART_BASE + n);
	return *reg;
}

void __udelay(unsigned long usec)
{

	if(usec > DUART_MAX_DELAY){
		ulong base = timestamp;
		ulong delay = usec / 10000;
		while((base + delay) > timestamp);
		usec -= delay * 10000;
	}

	if(usec > 200){
		uint16_t divisor = (usec / 43) * 10;

		writeRegDUART(DUART_CTUR, divisor >> 8);
		writeRegDUART(DUART_CTLR, divisor);

		readRegDUART(DUART_START);

		while(!(readRegDUART(DUART_ISR) & 0x08));

		readRegDUART(DUART_STOP);
	}
}

void pit_interrupt(void *not_used)
{
	writeRegPIT(PIT_TSR, 0x01);
	timestamp+=10;
}

int timer_init(void)
{
	timestamp = 0;

	writeRegPIT(PIT_TCR, 0x00);

	writeRegPIT(PIT_CPRH, 0);
	writeRegPIT(PIT_CPRM, 0x04);
	writeRegPIT(PIT_CPRL, 0x80);

	irq_install_handler(CONFIG_SYS_PIT_IRQ, pit_interrupt, 0);

	writeRegPIT(PIT_TCR, 0xE1);

	return 0;
}

ulong get_timer(ulong base)
{
	return (timestamp - base);
}

#endif /* CONFIG_DUART */

#if defined(CONFIG_MCFTMR)
#ifndef CONFIG_SYS_UDELAY_BASE
#	error	"uDelay base not defined!"
#endif

#if !defined(CONFIG_SYS_TMR_BASE) || !defined(CONFIG_SYS_INTR_BASE) || !defined(CONFIG_SYS_TMRINTR_NO) || !defined(CONFIG_SYS_TMRINTR_MASK)
#	error	"TMR_BASE, INTR_BASE, TMRINTR_NO or TMRINTR_MASk not defined!"
#endif
extern void dtimer_intr_setup(void);

void __udelay(unsigned long usec)
{
	volatile dtmr_t *timerp = (dtmr_t *) (CONFIG_SYS_UDELAY_BASE);
	uint start, now, tmp;

	while (usec > 0) {
		if (usec > 65000)
			tmp = 65000;
		else
			tmp = usec;
		usec = usec - tmp;

		/* Set up TIMER 3 as timebase clock */
		timerp->tmr = DTIM_DTMR_RST_RST;
		timerp->tcn = 0;
		/* set period to 1 us */
		timerp->tmr =
		    CONFIG_SYS_TIMER_PRESCALER | DTIM_DTMR_CLK_DIV1 | DTIM_DTMR_FRR |
		    DTIM_DTMR_RST_EN;

		start = now = timerp->tcn;
		while (now < start + tmp)
			now = timerp->tcn;
	}
}

void dtimer_interrupt(void *not_used)
{
	volatile dtmr_t *timerp = (dtmr_t *) (CONFIG_SYS_TMR_BASE);

	/* check for timer interrupt asserted */
	if ((CONFIG_SYS_TMRPND_REG & CONFIG_SYS_TMRINTR_MASK) == CONFIG_SYS_TMRINTR_PEND) {
		timerp->ter = (DTIM_DTER_CAP | DTIM_DTER_REF);
		timestamp++;

		#if defined(CONFIG_WATCHDOG) || defined (CONFIG_HW_WATCHDOG)
		if ((timestamp % (CONFIG_SYS_WATCHDOG_FREQ)) == 0) {
			WATCHDOG_RESET ();
		}
		#endif    /* CONFIG_WATCHDOG || CONFIG_HW_WATCHDOG */
		return;
	}
}

int timer_init(void)
{
	volatile dtmr_t *timerp = (dtmr_t *) (CONFIG_SYS_TMR_BASE);

	timestamp = 0;

	timerp->tcn = 0;
	timerp->trr = 0;

	/* Set up TIMER 4 as clock */
	timerp->tmr = DTIM_DTMR_RST_RST;

	/* initialize and enable timer interrupt */
	irq_install_handler(CONFIG_SYS_TMRINTR_NO, dtimer_interrupt, 0);

	timerp->tcn = 0;
	timerp->trr = 1000;	/* Interrupt every ms */

	dtimer_intr_setup();

	/* set a period of 1us, set timer mode to restart and enable timer and interrupt */
	timerp->tmr = CONFIG_SYS_TIMER_PRESCALER | DTIM_DTMR_CLK_DIV1 |
	    DTIM_DTMR_FRR | DTIM_DTMR_ORRI | DTIM_DTMR_RST_EN;

	return 0;
}

ulong get_timer(ulong base)
{
	return (timestamp - base);
}

#endif				/* CONFIG_MCFTMR */

#if defined(CONFIG_MCFPIT)
#if !defined(CONFIG_SYS_PIT_BASE)
#	error	"CONFIG_SYS_PIT_BASE not defined!"
#endif

static unsigned short lastinc;

void __udelay(unsigned long usec)
{
	volatile pit_t *timerp = (pit_t *) (CONFIG_SYS_UDELAY_BASE);
	uint tmp;

	while (usec > 0) {
		if (usec > 65000)
			tmp = 65000;
		else
			tmp = usec;
		usec = usec - tmp;

		/* Set up TIMER 3 as timebase clock */
		timerp->pcsr = PIT_PCSR_OVW;
		timerp->pmr = 0;
		/* set period to 1 us */
		timerp->pcsr |= PIT_PCSR_PRE(CONFIG_SYS_PIT_PRESCALE) | PIT_PCSR_EN;

		timerp->pmr = tmp;
		while (timerp->pcntr > 0) ;
	}
}

void timer_init(void)
{
	volatile pit_t *timerp = (pit_t *) (CONFIG_SYS_PIT_BASE);
	timestamp = 0;

	/* Set up TIMER 4 as poll clock */
	timerp->pcsr = PIT_PCSR_OVW;
	timerp->pmr = lastinc = 0;
	timerp->pcsr |= PIT_PCSR_PRE(CONFIG_SYS_PIT_PRESCALE) | PIT_PCSR_EN;

	return 0;
}

ulong get_timer(ulong base)
{
	unsigned short now, diff;
	volatile pit_t *timerp = (pit_t *) (CONFIG_SYS_PIT_BASE);

	now = timerp->pcntr;
	diff = -(now - lastinc);

	timestamp += diff;
	lastinc = now;
	return timestamp - base;
}

void wait_ticks(unsigned long ticks)
{
	u32 start = get_timer(0);
	while (get_timer(start) < ticks) ;
}
#endif				/* CONFIG_MCFPIT */

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On M68K it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

unsigned long usec2ticks(unsigned long usec)
{
	return get_timer(usec);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On M68K it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}
