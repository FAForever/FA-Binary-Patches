OUTPUT_FORMAT(pei-i386)
OUTPUT(section.pe)

"_atexit"                   = 0xA8211E;
"__Znwj"                    = 0xA825B9;
"__ZdlPvj"                  = 0x958C40;
"__imp__GetModuleHandleA@4" = 0xC0F378;
"__imp__GetProcAddress@8"   = 0xC0F48C;
"___CxxFrameHandler3"       = 0xA8958C;
"___std_terminate"          = 0xA994FB;
"??_7type_info@@6B@"        = 0xD72A88;
"__CxxThrowException@8"     = 0xA89950;
"_memset"                   = 0xA89110;
"__invoke_watson"           = 0xA848E8;
"_strcpy"                   = 0xA944E0;
"_memcpy"                   = 0xA84A60;
"_floor"                    = 0xA8DF30;
"_memmove"                  = 0xA84A60;
"_sprintf"                  = 0xA82E9D;
"_sprintf_s"                = 0xA82F32;
"__purecall"                = 0xA82547;
"_atoi"                     = 0xA835CE;

SECTIONS {
    . = __image_base__ + 0x1000;
    .text : {
        *(.text*)
        *(.data*)
        *(.bss*)
        *(.rdata)
    }
    .ctors : {
        _FIRST_CTOR = .;
        *(.ctors)
        *(.CRT*)
        _END_CTOR = .;
    }
    .clang : {
        clangfile.o
    }
    /DISCARD/ : {
        *(.rdata$zzz)
        *(.eh_frame*)
        *(.reloc)
        *(.idata*)
    }
}
