MEMORY
{
    Flash (rx) : ORIGIN = 0x00000000, LENGTH = 32K
    SRAM (rw) : ORIGIN = 0x00000000, LENGTH = 2K
}

ENTRY(main)

SECTIONS {

    .text 0x0:
    {
        *(.text*);
        *(.rodata*);
        . = ALIGN(4);
        init_fn_start = .;
        KEEP(*(.yo_init));
        init_fn_end = .;
        kernel_text_end = .;
    } > Flash

	.data :
	ALIGN (256)
	{
		_data_start = .;
		*(.data*);
		_data_end = .;
	} > SRAM

	/* zero initialized data */
	.bss (NOLOAD) :
	ALIGN (256)
	{
		bss_start = .;
		*(.bss*)
		bss_end = .;
	} > SRAM
}
