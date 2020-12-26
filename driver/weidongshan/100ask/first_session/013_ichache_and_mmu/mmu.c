
/**
 * 创建一级页表(每个条目为1M)
 * VA           PA           CB
 * 0            0            00
 * 0x40000000   0x40000000   11
 * 64M sdram:
 * 0x30000000   0x30000000   11
 * ......
 * 0x33f00000   0x33f00000   11
 * register: 0x48000000~0x5B00001C
 * 0x48000000   0x48000000   00
 * .......
 * 0x5B000000   0x5B000000   00
 * 
 * Framebuffer : 0x33c00000
 * 0x33c00000   0x33c00000   00
 * 
 * link address:
 * 0xB0000000   0x30000000   11
 * */
#define MMU_SECDESC_AP (3 << 10)
#define MMU_SECDESC_DOMAIN (0 << 5)
#define MMU_SECDESC_NCNB (0 << 2)
#define MMU_SECDESC_WB (3 << 2)
#define MMU_SECDESC_TYPE ((1 << 4) | (1 << 1))

#define MMU_SECDESC_FOR_IO (MMU_SECDESC_AP | MMU_SECDESC_DOMAIN | MMU_SECDESC_NCNB | MMU_SECDESC_TYPE)
#define MMU_SECDESC_FOR_MEM (MMU_SECDESC_AP | MMU_SECDESC_DOMAIN | MMU_SECDESC_WB | MMU_SECDESC_TYPE)

// 使用IO，即 CB 00不使用 cache
#define IO 1
#define MEM 0


/*
 * 每个条目，即1M开始有一个sec desc 
 */
void create_secdesc(unsigned int *ttb, unsigned int va, unsigned int pa, int io)
{
    int index;
    index = va / 0x100000;

    if (io)
        ttb[index] = (pa & 0xfff00000) | MMU_SECDESC_FOR_IO;
    else
        ttb[index] = (pa & 0xfff00000) | MMU_SECDESC_FOR_MEM;
}

void create_page_table(void)
{
    /* 1.页表在哪? 0x32000000(占据16KB) */
    /* ttb: translation table base */
    unsigned int *ttb = (unsigned int *)0x32000000;

    unsigned int va, pa;
    int index;

    /* 2. 根据va,pa设置页表条目 */

    /* 2.1 for sram/nor flash */
    create_secdesc(ttb, 0, 0, IO);

    /* 2.2 for sram when nor boot */
	create_secdesc(ttb, 0x40000000, 0x40000000, IO);

    /* 2.3 for 64M sdram */
	va = 0x30000000;
	pa = 0x30000000;
	for (; va < 0x34000000;)
	{
		create_secdesc(ttb, va, pa, MEM);
		va += 0x100000;
		pa += 0x100000;
	}

    /* 2.4 for register: 0x48000000~0x5B00001C */
	va = 0x48000000;
	pa = 0x48000000;
	for (; va <= 0x5B000000;)
	{
		create_secdesc(ttb, va, pa, IO);
		va += 0x100000;
		pa += 0x100000;
	}

	/* 2.5 for Framebuffer : 0x33c00000 */
	create_secdesc(ttb, 0x33c00000, 0x33c00000, IO);

	/* 2.6 for link address */
	create_secdesc(ttb, 0xB0000000, 0x30000000, MEM);
}
