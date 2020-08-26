SECTIONS {
    .text 0 : { *(.text) }
    .rodata : { *(.rodata) }
    /* 运行地址0x30000000, 保存地址0x800*/
    .data 0x30000000: AT(0x800)
    {
        /* 获取 0x800 为一个变量*/
        data_load_addr = LOADADDR(.data);
        . = ALIGN(4);
        data_start = .;
        *(.data)
        data_end = .;
    }
    . = ALIGN(4);
    bss_start = .;
    .bss : {*(.bss) *(.COMMON)}
    .bss_end = .;
}
