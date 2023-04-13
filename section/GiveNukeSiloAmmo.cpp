// Change unit:GiveNukeSiloAmmo(int) behaviour to be able to set work progress manually
// this way we can save half done missiles after unit transfer (works with SMLs and SMDs)
//
// Now when passing any second argumnent (just use True), the function will change the number of finished blocks instead of adding the missile to silo
// As you know missile launchers build missiles in different way compare to normal factories (see Moho::CAiSiloBuildImpl::SiloTick)
// every job is divided into "blocks". The number of blocks is calculated using this formula:
//
// blocks = missileBuildTime / launcherBuildPower * 10
//
// for standard SML there will be 450000/1500*10 = 3000 blocks
//
// so 3000 blocks is 100%. Then if you need to set, let say, 35% just send 1050 (integer) as first argument and True\False\whatever as second. Example:
// unit:GiveNukeSiloAmmo(1050, true)

char Blocks[1];

void SiloAmmoCheckArgs()
{
    asm(
        "mov %[Blocks], 0;"
        "cmp eax, 2;"
        "je 0x6CED8B;"
        "cmp eax, 3;" //change blocks if third arg exists
        "jne 0x6CED79;"
        "mov %[Blocks], 1;"
        "jmp 0x6CED8B;"
        :
        : [Blocks] "m" (Blocks)
        :
    );
}

void SiloAmmoSetBlocks()
{
    asm(
        "cmp %[Blocks], 0;"
        "je Finish;"
        "mov [ebx+0x48], eax;" //here we set the number of blocks. This value is used by Moho::CAiSiloBuildImpl::SiloTick for calculations
        "jmp 0x6CEDFA;"

        //default code
        "Finish:;"
        "mov edx, [edi];"
        "push eax;"
        "push 1;"
        "jmp 0x6CEDF6;"
        :
        : [Blocks] "m" (Blocks)
        :
    );
}