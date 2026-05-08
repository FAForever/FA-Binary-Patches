void asm__MinimapRangeRenderHook()
{
    asm("mov    al, byte ptr _render_minimap_ranges;"
        "test   al, al;"
        "jz     0x007D1A3F;"
        "mov    ecx, [esp+0xAC];" // code overwritten by hook
        "jmp    0x007D1A06;"
        :
        :
        :
    );
}