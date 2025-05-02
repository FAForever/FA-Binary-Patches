#include "moho.h"

#define NON_GENERAL_REG(var_) [var_] "g"(var_)

char LODMultTable[508]; //127*4
bool applyToWorstOnly = false;
float smallShadowCutoff = 0;


void FirstLODCheck()
{
    //This is a first LOD check where engine compare max LODCuttof of an object with camera distance
    //Then it prepares an array of visible objects to apply Moho::Mesh::ComputeLOD on them later
    //Despite the fact that we are in Moho::MeshRenderer::Batch initial array contains not only meshes
    //but also other things like effects/decals/etc
    //So we have to check if we actually work with MeshInstance to prevent exceptions
    //The normal route is MeshInstance->Mesh->RMeshBlueprint->GroupID(if exists)
	asm(
        //default
        "movss xmm1, dword ptr ss:[esp+0x14];"
        
        
        "push eax;"
        "push ecx;"
        "mov eax, dword ptr [esi+0x30];"  //Potential MeshInstance. esi+0x2C = max LODCuttof 
        "mov ecx, dword ptr [eax+0x1C];"
        "cmp ecx, 0xFFFFFFFF;"           
        "jne Exit;"                       //Not a MeshInstance
        
        "mov eax, dword ptr [eax+0x14];"  //Mesh
        "mov eax, dword ptr [eax+0x20];"  //RMeshBlueprint
        "cmp eax, 0x0;"
        "je Exit;"                        //No MeshBP (projectiles don't have it for some reason)
        
        "mov ecx, %[smallShadowCutoff];"
        "test ecx, ecx;"
        "je NoSmallCutoff;"               //smallShadowCutoff disabled
        
        
        "mov ecx, dword ptr [esp+0x38];"   //[esp+0x38] Moho::CGeomSolid3
        "cmp dword ptr [ecx+0x148], 0x0;"  //When filtering meshes for shadow mask there is 
                                           //a simplified CGeomSolid3 without gameTime, mouse pos, zoom etc
                                           //values in it. Here we check for gameTime (seconds)
                                           //float CGeomSolid3 + 0x148. If there is no gameTime, we proceed
        "jne NoSmallCutoff;"
        "cmp byte ptr [eax+0x43], 0x0;"
        "je NoSmallCutoff;"                //MeshBp.IsSmallObject = False
        "movss xmm7, %[smallShadowCutoff];"   
        "comiss xmm1, xmm7;"               
        "jbe NoSmallCutoff;"               //Distance < smallShadowCutoff
        "pxor xmm0, xmm0;"
        "jmp Exit;"
        
        
        "NoSmallCutoff:;"
        "mov ecx, 0x00000000;"
        "mov cl, byte ptr [eax+0x42];"
        "cmp cl, 0x0;"                     
        "je Exit;"                        //No groupId in mesh bp
        
        "mov al, 0x4;"
        "mul cl;"
        "mov ecx, %[LODMultTable];"
        "add cx, ax;"
        "mov eax, dword ptr[ecx];"
        "cmp eax, 0;"
        "je Exit;"                       //No mult
        
        "mulss xmm0, dword ptr[ecx];"
        
        "Exit:;"
        "pop ecx;"
        "pop eax;"
        "jmp 0x005039F9;"
        
        :
        : NON_GENERAL_REG(LODMultTable),
          [smallShadowCutoff]"m"(smallShadowCutoff)
        :
	);
}

char savedMult[4];
char null[4];

void MeshComputeLOD()
{
    //Moho::Mesh::ComputeLOD
    //this is where best LOD is picked from all avalaible
    //ProcessBestLOD and ProcessWorstLOD are subparts of it
	asm(
        //default
        "movss xmm0, dword ptr [eax+0x8];"
        
        "push eax;"
        "push ebx;"
        
        "mov eax, 0x0;"
        "mov %[savedMult], 0x0;"
        "xorps xmm7, xmm7;"
        "mov eax, dword ptr [esp+0x8];"
        "mov eax, dword ptr [eax+0x20];"
        "cmp eax, 0x0;"  
        "je Exit2;"        
        
        "mov bl, byte ptr [eax+0x42];"
        "cmp bl, 0x0;"
        "je Exit2;"                      //No groupId
        
        "mov al, 0x4;"
        "mul bl;"
        "mov ebx, %[LODMultTable];"
        "add bx, ax;"
        "mov eax, dword ptr[ebx];"
        "cmp eax, 0;"
        "je Exit2;"                      //No mult
        
        "mov ebx, dword ptr[ebx];"
        "mov %[savedMult], eax;"
        
        "Exit2:;"
        "pop ebx;"
        "pop eax;"
        
        "comiss xmm0, xmm2;"
        "jbe RETURN1;"
        
        "jmp 0x007DDA7D;"
        
        "RETURN1:;"
        "movss xmm7, xmm0;" //Save modified LODCuttof to xmm7 for futher use in some checks            
        "pop esi;"
        "ret;"
        
        :
        : [applyToWorstOnly]"m"(applyToWorstOnly),
          NON_GENERAL_REG(LODMultTable),
          [savedMult]"m"(savedMult),
          [null]"m"(null)
        :
	);
}

void ProcessBestLOD()
{
    asm(
        "push eax;"
        "mov al, %[applyToWorstOnly];"
        "cmp al, 0x1;"
        "je Exit3;"
        
        "mov eax, %[savedMult];"
        "cmp eax, 0x0;"
        "je Exit3;"
        
        "mulss xmm0, %[savedMult];"
        
        
        "Exit3:;"
        "pop eax;"
        
        //default
        "comiss xmm1, xmm0;"
        "jbe RETURN1;"
        
        "jmp 0x007DDA88;"
        
        :
        : [applyToWorstOnly]"m"(applyToWorstOnly),
          [savedMult]"m"(savedMult)
        :
    );
}

void ProcessWorstLOD()
{
    asm(
        //default
        "addss xmm0, dword ptr ds:[0x00F57F00];"
        
        "push eax;"
        "mov eax, %[savedMult];"
        "cmp eax, 0x0;"
        "je Exit4;"
        "mulss xmm0, %[savedMult];"
        
        "Exit4:;"
        "pop eax;"
        "comiss xmm1, xmm0;"
        "jbe RETURN1;"
        
        //default
        "xor eax, eax;"
        "pop esi;"
        "ret;"

        :
        : [savedMult]"m"(savedMult)
        :
    );
}

int LuaSetLODMult(lua_State *l)
{
    int groupID = luaL_checknumber(l, 1);
    if  (groupID < 1 || groupID > 127)
    {
        luaL_error(l, "groupID should be from 1 to 127");
        return 0;
    }
    float mult = luaL_optnumber(l, 2, 0);
    
    unsigned char *ch = reinterpret_cast<unsigned char *>(&mult);
    
    for (int i = 0; i != 5; ++i)
    {
        LODMultTable[4*groupID + i] = ch[i];
    }

    return 0;
}

UIRegFunc SetGroupLODMult{"SetGroupLODMult", "SetGroupLODMult(group:int, mult:float)", LuaSetLODMult};


int LuaApplyMultToWorstLOD(lua_State *l)
{
    applyToWorstOnly = lua_toboolean(l, 1);
    
    return 0;
}

UIRegFunc ApplyMultToWorstLOD{"ApplyMultToWorstLODOnly", "ApplyMultToWorstLODOnly(apply:bool)", LuaApplyMultToWorstLOD};


int LuaSetSmallShadowCutoff(lua_State *l)
{
    smallShadowCutoff = luaL_optnumber(l, 1, 0);
   
    return 0;
}

UIRegFunc SetSmallShadowCutoff{"SetSmallShadowCutoff", "SetSmallShadowCutoff(distance:float)", LuaSetSmallShadowCutoff};