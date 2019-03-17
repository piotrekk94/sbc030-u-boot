#include <common.h>
#include <command.h>

#define BI_LAST		0
#define BI_MACHTYPE	1
#define BI_CPUTYPE	2
#define BI_FPUTYPE	3
#define BI_MMUTYPE	4
#define BI_MEMCHUNK	5
#define BI_RAMDISK	6
#define BI_COMMAND	7
#define SBC030_MACHTYPE	14

static u32 get_bi_addr(u32 addr, u32 size)
{
	u32 ret = addr + size;

	ret += (4096 - (ret % 4096));

	return ret;
}

static u32 add_record(u32 bi_addr, u16 tag, u32 data)
{
	*(u16*)(bi_addr) = tag;
	*(u16*)(bi_addr + 2) = 8;
	*(u32*)(bi_addr + 4) = data;
	return 8;
}

static u32 add_meminfo(u32 bi_addr, u16 tag, u32 addr, u32 size)
{
	*(u16*)(bi_addr) = tag;
	*(u16*)(bi_addr + 2) = 12;
	*(u32*)(bi_addr + 4) = addr;
	*(u32*)(bi_addr + 8) = size;
	return 12;
}

static u32 add_bootargs(u32 bi_addr)
{
	u32 size = 4;
	char *bootargs = env_get("bootargs");

	size += strlen(bootargs);

	*(u16*)(bi_addr) = BI_COMMAND;
	*(u16*)(bi_addr + 2) = size;

	memcpy((u8*)(bi_addr + 4), bootargs, strlen(bootargs));	

	return size;
}

int do_boot68(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 img_addr, img_size;
	u32 mem_addr, mem_size;
	u32 initrd_addr, initrd_size;
	u32 bi_addr;

	if (argc < 5)
		return 1;

	img_addr = simple_strtoul(argv[1], NULL, 16);
	img_size = simple_strtoul(argv[2], NULL, 16);
	mem_addr = simple_strtoul(argv[3], NULL, 16);
	mem_size = simple_strtoul(argv[4], NULL, 16);

	bi_addr = get_bi_addr(img_addr, img_size);

	printf("bi_addr: %x\n", bi_addr);

	bi_addr += add_record(bi_addr, BI_MACHTYPE, SBC030_MACHTYPE);
	bi_addr += add_record(bi_addr, BI_CPUTYPE, 2);
	bi_addr += add_record(bi_addr, BI_FPUTYPE, 0);
	bi_addr += add_record(bi_addr, BI_MMUTYPE, 2);
	bi_addr += add_meminfo(bi_addr, BI_MEMCHUNK, mem_addr, mem_size);

	if (argc > 6){
		initrd_addr = simple_strtoul(argv[5], NULL, 16);
		initrd_size = simple_strtoul(argv[6], NULL, 16);
		bi_addr += add_meminfo(bi_addr, BI_RAMDISK, initrd_addr, initrd_size);
	}

	bi_addr += add_bootargs(bi_addr);
	bi_addr += add_record(bi_addr, BI_LAST, 0);

	return 0;
}

#ifdef CONFIG_SYS_LONGHELP
static char boot68_help_text[] =
	"[img-addr img-size mem-addr mem-size [initrd-addr initrd-size]]\n"
	"    - boot Linux m68k binary stored in memory";
#endif

U_BOOT_CMD(
	boot68, CONFIG_SYS_MAXARGS, 1, do_boot68,
	"boot Linux m68k binary from memory", boot68_help_text
);
