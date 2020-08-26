SECTIONS {
    .text 0 : { *(.text) }
    .rodata : { *(.rodata) }
    /* 运行地址0x30000000, 保存地址0x800*/
    .data 0x30000000: AT(0x800) {*(.data)}
    .bss : {*(.bss) *(.COMMON)}
}
