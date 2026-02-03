#include "CObject.h"
#include "magic_classes.h"
#include "moho.h"
#define NON_GENERAL_REG(var_) [var_] "g"(var_)

char isFACTORYrequested[1];
char factoryCatName[11] = "RALLYPOINT";
bool shareFactoryAssist = false;

int EAXholder;
int ESIholder;

ConDescReg ShareFactoryAssistCommand_var{"ui_ShareFactoryAssistCommandToUnits", "", &shareFactoryAssist};

// Default function is hardcoded to split all selected units (Moho::EntitySet)
// into "REBUILDER"s and normal units.
// A simple hack allows us to use it for splitting into factories and other units
// So we can apply ISSUE_FactoryCommand to them later
void asm__SplitSelectedUnitsByGivenCategory()
{
    asm(
        "cmp %[isFACTORYrequested], 0x0;"   
        "je Default;"
        
        "push 0xA;"
        "push %[factoryCatName];"           //"RALLYPOINT"
        "jmp 0x0081EA4B;"
        
        
        "Default:;"
        "push 0x9;"
        "push 0xE20498;"                    //"REBUILDER"
        "jmp 0x0081EA4B;"
        
        
        :
        : [isFACTORYrequested] "m" (isFACTORYrequested), 
          NON_GENERAL_REG(factoryCatName)
        :
    );
}

void asm__targetIsStructureCase_prepareUnitLists()
{
    asm(
        "mov eax, dword ptr ds:[edi+0x4];"   // default code
        "mov esi, dword ptr ss:[ebp+0x44];"  //
        
        "mov ecx, [ESP+0x64];"              // num of rebuilders in selection  
        "test ecx,ecx;"                     // If there are any "REBUILDER"s use default logic               
        "jne Exit;"                         // for backward compatibility (there are no such units in FAF patch for now) 
                              
        "cmp %[shareFactoryAssist], 0x0;"   // if shareFactoryAssist enabled we continue
        "je Exit;"
        
        "mov %[EAXholder], eax;"            // save registers to mem to not mess with stack
        "mov %[ESIholder], esi;"            //

        "mov eax, [ESP+0x4];"
        "call 0x007ABDE0;"                 // deAlloc normal units
        "mov eax, [ESP+0x8];"                
        "call 0x007ABDE0;"                 // deAlloc "REBUILDER"s
        
        "mov esi, [esp+0x4];"              // Alloc for non-factory units
        "call 0x007AE180;"
        "mov esi, [esp+0x8];"              // Alloc for factories
        "call 0x007AE180;"

        
        "mov %[isFACTORYrequested], 0x1;"  // Here we call the function we patched above that will split selcted units 
        "call 0x0081E9E0;"                 // into factories and non-factories
        "mov %[isFACTORYrequested], 0x0;"
        
        "mov eax, %[EAXholder];"            // restore registers
        "mov esi, %[ESIholder];"            //

        "Exit:;"
        "jmp 0x0082022D;"
        
        :
        : [shareFactoryAssist] "m" (shareFactoryAssist),
          [isFACTORYrequested] "m" (isFACTORYrequested),
          [EAXholder] "m" (EAXholder),
          [ESIholder] "m" (ESIholder)
        :
    );
}

void asm__targetIsStructureCase_issueFactoryCommand()
{
    asm(
        "cmp %[shareFactoryAssist], 0x0;"   // is shareFactoryAssist enabled
        "je Exit2;"
        
        "lea ebx, ss:[esp+0x58];"           // pick factories
        "call 0x008B0B30;"                  // and call ISSUE_FactoryCommand
        "add esp, 0x8;"                     
        "lea eax, [esp+0x85C];"
        "push eax;"
        "mov byte ptr ss:[esp+0xE0C], 0x9;"
        "call 0x0057ABB0;"                  // deAlloc command
        
        "mov dword ptr ss:[esp+0xE08], 0x8;"
        "lea eax, [esp+0x14];"
        "call 0x007ABDE0;"                  // deAlloc non-factories
        "mov dword ptr ss:[esp+0xE08], 0xFFFFFFFF;"
        "lea eax, [esp+0x50];"
        "call 0x007ABDE0;"                  // deAlloc factories
        "jmp 0x00821EEA;"                   // return

        "Exit2:;"
        "add esp, 0x8;"                     // default
        "lea eax, [esp+0x85C];"             //
        "jmp 0x00820290;"
        
        :
        : [shareFactoryAssist] "m" (shareFactoryAssist)
        :
    );
}


void asm__targetIsMobileCase_issueFactoryCommand()
{
    asm(
        "movss dword ptr ds:[eax+0x20], xmm0;"  // default
        "push edx;"                             //
        "push eax;"                             //
        
        "cmp %[shareFactoryAssist], 0x0;"       // no share assist, use default route and
        "je 0x008203B9;"                        // and call ISSUE_Command for all units
                                
        "lea esi, [esp+0x1C];"                  // Alloc for non-factory units
        "call 0x007AE180;"
        "lea esi, [esp+0x58];"                  // Alloc for factories
        "call 0x007AE180;"

        "lea eax, [esp+0x58];"                  
        "push eax;"                             // 3rd arg - factories list
        "lea eax, [esp+0x20];"
        "push eax;"                             // 2nd arg - non-factories list    
        "mov eax, [esp+0x50];"
        "push eax;"                             // 1st arg - all units 
        
        "mov %[isFACTORYrequested], 0x1;" 
        "call 0x0081E9E0;"                      // split
        "mov %[isFACTORYrequested], 0x0;"
        
        "add esp, 0xC;"
        
        "lea ebx, [esp+0x1C];"                  // non-factories
        "call 0x008B05E0;"                      // call ISSUE_Command
        
        "lea ebx, [esp+0x58];"                  // factories
        "call 0x008B0B30;"                      // call ISSUE_FactoryCommand
        
        "lea eax, [esp+0x1C];"
        "call 0x007ABDE0;"                      // deAlloc non-factories
        "lea eax, [esp+0x58];"
        "call 0x007ABDE0;"                      // deAlloc factories
        
        
        "jmp 0x008203BE;"                       // go to default ending                                     
                                                // command will be deAlloced there
        :
        : [shareFactoryAssist] "m" (shareFactoryAssist),
          [isFACTORYrequested] "m" (isFACTORYrequested)
        :
    );
}


