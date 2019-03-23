#include <common.h>
#include <spi.h>
#include <malloc.h>
#include <asm/io.h>

#define SPI0_BASE 0xE0010000
#define SPI1_BASE 0xE0020000
#define SPI_SR 0
#define SPI_CR 0
#define SPI_DR 1
#define SPI_DONE 0x02

struct sbc030_spi {
	struct spi_slave slave;
	void *base;
};

static inline struct sbc030_spi *to_sbc030_spi(struct spi_slave *slave)
{
	return container_of(slave, struct sbc030_spi, slave);
}

void spi_init (void)
{

}

void spi_set_speed(struct spi_slave *slave, uint hz)
{

}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	if((bus == 0 || bus == 1) && cs == 0)
		return 1;
	else
		return 0;
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode)
{
	struct sbc030_spi *ss;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	ss = spi_alloc_slave(struct sbc030_spi, bus, cs);
	if (!ss)
		return NULL;

	if(bus == 0)
		ss->base = SPI0_BASE;
	else
		ss->base = SPI1_BASE;

	return &ss->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct sbc030_spi *ss = to_sbc030_spi(slave);

	free(ss);
}

int spi_claim_bus(struct spi_slave *slave)
{
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{

}

void spi_cs_activate(struct spi_slave *slave)
{
	struct sbc030_spi *ss = to_sbc030_spi(slave);
	writeb(0x00, ss->base + SPI_CR);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	struct sbc030_spi *ss = to_sbc030_spi(slave);
	writeb(0x01, ss->base + SPI_CR);
}

static int spi_check_done(struct sbc030_spi *ss)
{
	return readb(ss->base + SPI_SR) & SPI_DONE;
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	struct sbc030_spi *ss = to_sbc030_spi(slave);
	const u8 *txd = dout;
	u8 *rxd = din;

	if(bitlen % 8)
		return -1;

	if (flags & SPI_XFER_BEGIN)
		spi_cs_activate(slave);

	for(int i = 0; i < bitlen / 8; i++) {

		if(txd)
			writeb(*txd++, ss->base + SPI_DR);
		else
			writeb(0xff, ss->base + SPI_DR);

		while(!spi_check_done(ss));

		if(rxd)
			*rxd++ = readb(ss->base + SPI_DR);

	}

	if (flags & SPI_XFER_END)
		spi_cs_deactivate(slave);

	return(0);
}
