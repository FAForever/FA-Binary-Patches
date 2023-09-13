OUTPUT_FORMAT(pei-i386)
OUTPUT(section.pe)

_atexit = 0xA8211E;

SECTIONS {
  . = __image_base__ + 0x1000;
  .text : {
    *(.text*)
    *(.data*)
    *(.bss*)
    *(.rdata)
  }
  /DISCARD/ : {
    *(.text.startup)
    *(.rdata$zzz)
    *(.eh_frame*)
    *(.ctors)
    *(.reloc)
    *(.idata*)
  }
}