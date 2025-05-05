#include "moho.h"

const char *meshGroupField = "LODGroup";

char eaxStorage[4];
char espStorage[4];
char ebpStorage[4];
char edxStorage[4];

char *fieldName;
float fieldValue;
char *rBlueprint;


//Blueprint loading on game start. We are inside Moho::SCR_LuaBuildObject (recursive function)
//For now it is used for "LODGroup", but additional fields can be added in the future if needed.
 
void GetBpFieldValues()
{
	asm(
        "mov %[eaxStorage], eax;"
        "mov %[espStorage], esp;"
        "mov %[ebpStorage], ebp;"
        "mov %[edxStorage], edx;"
        
        "mov %[fieldName], eax;"
        
        "mov ecx, dword ptr [ecx+0x58];"
        "mov %[fieldValue], ecx;"
        
        "mov ecx, dword ptr [esp+0x4];"
        "mov %[rBlueprint], ecx;"
        
        "jmp 0x004CF7F7;"
        :
        : [fieldName] "m" (fieldName),
          [fieldValue] "m" (fieldValue),
          [rBlueprint] "m" (rBlueprint),
          [eaxStorage] "m" (eaxStorage),
          [espStorage] "m" (espStorage),
          [ebpStorage] "m" (ebpStorage),
          [edxStorage] "m" (edxStorage)
        :
	);
}

void ProcessBpFieldValues()
{
    if (not strcmp(fieldName, meshGroupField)) //(not strcmp) == equal
    {
        if (fieldValue < 128)
        {
            int8_t groupID = int8_t(fieldValue);
            *(rBlueprint + 0x42) = reinterpret_cast<int8_t>(groupID);
        }
    }
    
    asm(
        //fixing registers
        "mov eax, %[eaxStorage];"
        "mov esp, %[espStorage];"
        "mov ebp, %[ebpStorage];"
        "mov edx, %[edxStorage];"

        //default code
        "mov ecx, dword ptr [esi+0x4];"
        "mov ebp, eax;"
        "push ebp;"
        "CALL 0x008D94E0;" //gpg::RType::GetFieldNamed
        "jmp 0x004CF7FD;"
        
        :
        : [eaxStorage] "m" (eaxStorage),
          [espStorage] "m" (espStorage),
          [ebpStorage] "m" (ebpStorage),
          [edxStorage] "m" (edxStorage)
        :
    );
}

void InitRMeshBlueprint()
{
	asm(
        //default
        "mov dword ptr ds:[esi+0x64], eax;"
        "mov dword ptr ds:[esi+0x68], eax;"
        
        //new clear mem
        "mov byte ptr [esi+0x42], 0x0;"
        
        "jmp 0x005283DB;"
	);
}