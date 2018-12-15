// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include <common.h>
#include <asm/immap.h>
#include <asm/cache.h>

volatile int *cf_icache_status = (int *)ICACHE_STATUS;
volatile int *cf_dcache_status = (int *)DCACHE_STATUS;

void flush_cache(ulong start_addr, ulong size)
{
	/* Must be implemented for all M68k processors with copy-back data cache */
}

int icache_status(void)
{
	return *cf_icache_status;
}

int dcache_status(void)
{
	return *cf_dcache_status;
}

void icache_enable(void)
{
	icache_invalid();
	*cf_icache_status = 1;
}

void icache_disable(void)
{
	*cf_icache_status = 0;
	icache_invalid();
}

void icache_invalid(void)
{

}

/*
 * data cache only for ColdFire V4 such as MCF547x_8x, MCF5445x
 * the dcache will be dummy in ColdFire V2 and V3
 */
void dcache_enable(void)
{
	dcache_invalid();
	*cf_dcache_status = 1;
}

void dcache_disable(void)
{
	*cf_dcache_status = 0;
	dcache_invalid();
}

void dcache_invalid(void)
{

}

__weak void invalidate_dcache_range(unsigned long start, unsigned long stop)
{
	/* An empty stub, real implementation should be in platform code */
}
__weak void flush_dcache_range(unsigned long start, unsigned long stop)
{
	/* An empty stub, real implementation should be in platform code */
}
