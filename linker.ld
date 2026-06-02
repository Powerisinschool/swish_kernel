ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);    /* Read + Execute */
    rodata PT_LOAD FLAGS(4);  /* Read Only */
    data PT_LOAD FLAGS(6);    /* Read + Write */
}

VMA = 0xffffffff80000000;
LMA = 0x100000;

SECTIONS
{
    /* Typically, 1MB is used for x86_64 kernel code setup */
    . = VMA;

    .text : ALIGN(0x1000) {
        *(.text .text.*)
    } :text

    .rodata : ALIGN(0x1000) {
        *(.rodata .rodata.*)
    } :rodata

    .data : ALIGN(0x1000) {
        *(.data .data.*)
    } :data

    .bss : ALIGN(0x1000) {
        *(.bss .bss.*)
        *(COMMON)
    } :data
}
