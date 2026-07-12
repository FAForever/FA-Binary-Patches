// Forward the source Unit's four texture-scroll values into the
// ReconBlip Entity snapshot.
//
// Hook context at 0x005BF171:
//   EDI = ReconBlip
//   ESI = snapshot builder argument before conversion to Entity snapshot base
//
// ReconBlip + 0x270 stores the encoded source Unit reference.
// A valid reference is converted to the full Unit pointer by subtracting 4.
//
// The Unit contains an Entity subobject at +0x08:
//   Unit + 0x100 = Entity + 0x0F8
//   Unit + 0x104 = Entity + 0x0FC
//   Unit + 0x108 = Entity + 0x100
//   Unit + 0x10C = Entity + 0x104
//
// The destination fields are the corresponding scroll values in the
// Entity snapshot built for the ReconBlip.

void ReconBlipTrackScrollFix()
{
    asm(
        // Replay the six-byte instruction replaced by the hook.
        "SUB ESI,0xD8;"

        // Preserve the original flags produced by SUB and the registers
        // used by the bridge.
        "PUSHFD;"
        "PUSH EAX;"
        "PUSH ECX;"

        "MOV EAX,[EDI+0x270];"
        "TEST EAX,EAX;"
        "JE ReconBlipTrackScrollFix_Done;"
        "SUB EAX,4;"
        "TEST EAX,EAX;"
        "JE ReconBlipTrackScrollFix_Done;"

        "MOV ECX,[EAX+0x100];"
        "MOV [ESI+0x88],ECX;"
        "MOV ECX,[EAX+0x104];"
        "MOV [ESI+0x8C],ECX;"
        "MOV ECX,[EAX+0x108];"
        "MOV [ESI+0x90],ECX;"
        "MOV ECX,[EAX+0x10C];"
        "MOV [ESI+0x94],ECX;"

        "ReconBlipTrackScrollFix_Done:;"
        "POP ECX;"
        "POP EAX;"
        "POPFD;"

        // Resume immediately after the replaced instruction.
        "JMP 0x005BF177;"
    );
}
