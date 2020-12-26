SECTIONS
{
	. = 0xB0000000;

	__code_start = .;

	. = ALIGN(4);
	.text      :
	{
	  *(.text)
	}

	. = ALIGN(4);
	.rodata : { *(.rodata) }

	. = ALIGN(4);
	.data : { *(.data) }

	. = ALIGN(4);
	__bss_start = .;
	.bss : { *(.bss) *(.COMMON) }
	_end = .;
}
