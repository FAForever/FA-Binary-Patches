asm(R"(
.section h0; .set h0,0x06C8B48 #Moho::UnitTestCommandCaps_LuaFuncDef+0xD8
    #test    [eax+0x180], ecx   # test toggle caps
    test    [eax+0x17C], ecx    # test command caps
)");