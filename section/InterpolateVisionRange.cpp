
// I only got an hour, ask 4z0t to rewrite

void UserEntityOnTick() {
    register int *unit asm("ecx");
    int *handle = (int *) *(unit + 0x6);
    if (handle != 0) {
        int *entry = (int *) *(handle + 0x2);
        if (entry != 0) {
            *(entry + 0x6) = *(entry + 0x9); // set prev circle rad to cur
        }
    }
}

void OnTickbeat() {
    asm(R"(
        mov     eax, [0x10A81B0]
        mov     edx, [eax+0x1C]
        mov     ecx, offset [0x10A81B0]
        call    edx
    )");


    // this is another entry point we could use
}
