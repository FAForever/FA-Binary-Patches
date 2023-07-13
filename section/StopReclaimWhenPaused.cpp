char ReclaimRate[4];
char MassPerTick[4];
char EnergyPerTick[4];
char IsValueModified[1];

//Engine uses 3 values in reclaim cycle. They are claculated when relcaim starts using data from lua's :GetReclaimCosts():
//float ReclaimRate - what part of an object will be reclaimed in 1 tick. The number is negative and can't be zero. So I use -1e-8 to stop relclaim
//float MassPerTick - how much mass you'll get per 1 tick. It doesn't rely on ReclaimRate, so we have to set it to 0 manually.
//float EnergyPerTick - same to Mass.

void StopReclaim()
{
	asm(
        //default code
        "mov eax, [edx+0x14];"
        "call eax;"
        "mov edx, [eax];"
        
        
        "push eax;"
        "mov %[IsValueModified], 0x0;"
        "mov eax, [edi+0x1C];"
        "cmp byte ptr [eax+0x2A0], 0x0;" //unitIsPaused
        "je End;"
        
        
        //Stop reclaim
        "mov %[IsValueModified], 0x1;"
        "mov eax, [edi+0x64];"
        "mov %[ReclaimRate], eax;"
        "mov eax, 0xb22bcc77;"
        "mov [edi+0x64], eax;"
        "mov eax, [edi+0x68];"
        "mov %[EnergyPerTick], eax;"
        "mov eax, 0x0;"
        "mov [edi+0x68], eax;"
        "mov eax, [edi+0x6C];"
        "mov %[MassPerTick], eax;"
        "mov eax, 0x0;"
        "mov [edi+0x6C], eax;"
        

        "End:;"
        "pop eax;"
        "jmp 0x61FF82;"
        
        :
        : [ReclaimRate] "m" (ReclaimRate), [MassPerTick] "m" (MassPerTick), [EnergyPerTick] "m" (EnergyPerTick), [IsValueModified] "m" (IsValueModified)
        :
	);
}

void RestoreReclaimValues()
{
	asm(
        //default code
        "mov dword ptr fs:[0x00000000], ecx;"

        "cmp %[IsValueModified], 0x0;"
        "je End2;"
        "push eax;"
        "mov %[IsValueModified], 0x0;"
        "mov eax, %[ReclaimRate];"
        "mov [edi+0x64], eax;"
        "mov eax, %[EnergyPerTick];"
        "mov [edi+0x68], eax;"
        "mov eax, %[MassPerTick];"
        "mov [edi+0x6C], eax;"
        "pop eax;"
        
        
        "End2:;"
        "jmp 0x620078;"
        
        :
        : [ReclaimRate] "m" (ReclaimRate), [MassPerTick] "m" (MassPerTick), [EnergyPerTick] "m" (EnergyPerTick), [IsValueModified] "m" (IsValueModified)
        :
	);
}