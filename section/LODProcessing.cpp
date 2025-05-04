#include "moho.h"
#include "global.h"

#define NON_GENERAL_REG(var_) [var_] "g"(var_)

float LODMultTable[128];
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


//A copy of Moho::Mesh::ComputeLOD (0x007DDA50) with the support of LOD multipliers
//Only "mesh" and "distance" args are used, others is just a workaround to trick compiler
//Default function uses eax and xmm1 so there is no clear solution

int __cdecl ComputeLOD(int null, int null2, int null3, void *mesh, int null5, int null6, int null7, int null8, int null9, float distance)
{
    int *LODsArray = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(mesh) + 0x30);
    int *meshBpPtr = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(mesh) + 0x20);
    
    float LODmult = 1;
    
    if (not *LODsArray)
    {
        return 0;
    }
    
    if (*meshBpPtr)
    {
        int *meshBp = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(mesh) + 0x20);
        int8_t meshGroupID = *reinterpret_cast<int8_t*>((*meshBp) + 0x42);

        if (meshGroupID)
        {
            if (LODMultTable[meshGroupID])
            {
                LODmult = LODMultTable[meshGroupID];
            }
        }
    }

    int numOfLODs = (*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(mesh) + 0x34) - *LODsArray) >> 2;
    int finalMeshLOD = 0;
    float finalCutoff = 0;
    
    for (int i = 0; i != numOfLODs; ++i)
    {
        int *meshLOD = reinterpret_cast<int*>(*LODsArray);
        meshLOD = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(meshLOD) + (0x4 * i));

        
        bool worstLOD = *reinterpret_cast<bool*>((*meshLOD) + 0x4);
        float LODCutoff = *reinterpret_cast<float*>((*meshLOD) + 0x8);
        
        if (not worstLOD)
        {
            if (not applyToWorstOnly)
            {
                LODCutoff = LODCutoff * LODmult;
            }
            
            if (distance <= LODCutoff)
            {
                finalMeshLOD = *meshLOD;
                finalCutoff = LODCutoff;
                break;
            }

        } else
        { 
            LODCutoff = LODCutoff * LODmult; 
            if (distance <= LODCutoff + 20) //20 is default ren_MeshDissolve (0x00F57F00)    
            {
                finalMeshLOD = *meshLOD;
                finalCutoff = LODCutoff;
                break;
            }
        }     
        
    }
    
    asm(
        "movss xmm3, %[distance];"     //restore xmm3 because it's used later by default code
        "movss xmm7, %[finalCutoff];"  //also save modified cutoff for further use

        :
        : [finalCutoff] "m"(finalCutoff),
          [distance] "m"(distance)
        :
    );
    
    return finalMeshLOD;
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
    
    LODMultTable[groupID] = mult;

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