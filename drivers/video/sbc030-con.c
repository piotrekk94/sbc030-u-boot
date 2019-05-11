#include <common.h>
#include <malloc.h>
#include <video.h>
#include <linux/ctype.h>

#include <stdio_dev.h>

#define SBC030_CON_COLS 80
#define SBC030_CON_ROWS 25

static u16 *sbc030_con_buf = (u16*)0xe0080000;
static u16 *sbc030_con_font = (u16*)0xe0083000;
static u8 posx = 0, posy = 0;

static void console_scroll(void)
{
	for(u8 i = 0; i < SBC030_CON_ROWS - 1; i++)
		for(u8 j = 0; j < SBC030_CON_COLS; j++)
			sbc030_con_buf[i * SBC030_CON_COLS + j] = sbc030_con_buf[(i + 1) * SBC030_CON_COLS + j];
	memset(&sbc030_con_buf[(SBC030_CON_ROWS - 1) * SBC030_CON_COLS], 0, SBC030_CON_COLS * sizeof(*sbc030_con_buf));
}

static void console_new(void)
{
	posx = 0;
	posy++;
	if(posy == SBC030_CON_ROWS){
		posy--;
		console_scroll();	
	}
}

static void console_putc(const char c)
{
	sbc030_con_buf[posy * SBC030_CON_COLS + posx] = (0x07 << 8) | c;
	posx++;
	if(posx == SBC030_CON_COLS)
		console_new();
}

static void console_back(void)
{
	if(posx > 0){
		posx--;
	}else{
		posx = SBC030_CON_COLS - 1;
		if(posy > 0)
			posy--;
	}

	sbc030_con_buf[posy * SBC030_CON_COLS + posx] = 0;
}

static void sbc030_con_putc(struct stdio_dev *dev, const char c)
{
	switch(c){
		case '\r':
			posx = 0;
			break;
		case '\n':
			console_new();
			break;
		case '\b':
			console_back();
			break;
		default:
			if(isprint(c))
				console_putc(c);
	}
}

static void sbc030_con_puts(struct stdio_dev *dev, const char *s)
{
	int count = strlen(s);

	while(count--)
		sbc030_con_putc(dev, *s++);
}

static void sbc030_con_init(void)
{
	memset(sbc030_con_buf, 0, 0x1000);
	memcpy(sbc030_con_font, (void*)0xfc07f000, 0x1000);
}

int drv_video_init(void)
{
	struct stdio_dev console_dev;

	sbc030_con_init();

	memset(&console_dev, 0, sizeof(console_dev));

	strcpy(console_dev.name, "vga");

	console_dev.flags = DEV_FLAGS_OUTPUT;
	console_dev.putc = sbc030_con_putc;
	console_dev.puts = sbc030_con_puts;

	stdio_register(&console_dev);

	return 1;
}
