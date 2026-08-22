extern "C" void TrueFreeCamDetour()
{
    asm(
        "_TrueFreeCamDetour:;"
        // Check if cam_Free set
        "cmp byte ptr ds:[0x010A645F], 1;"
        "je .L_SKIP_CLAMP;"
        // Otherwise run bytes we stole and jmp to rest of clamp function
        "push ebp;"
        "mov ebp, esp;"
        "and esp, -0x08;"
        "jmp 0x007A9476;"
        //return immediately if cam_Free
        ".L_SKIP_CLAMP:;"
        "ret;"
    );
}
