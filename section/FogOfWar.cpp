#include "moho.h"

//When app starts it executes a constructor for all default .fx vars
//example: 0x00BE1300 But there is no reason to hook there, as we can
//create same struct manually. Just use short names <15, otherwise
//need to call std::string to get a pointer.

struct shaderVar
{ // 0x48 bytes
	int empty;
    char varName[16];
    int varNameLen;
    int varNameMaxLen;
    
    int empty2;
    char fxFileName[16];
    int fxNameLen;
    int fxNameMaxLen;
    
    int ptr1;
    int ptr2;
    int ptr3;
    int ptr4;
};

struct shaderVar inverseMatrix = {0,"InverseWldProj",14,15,  0,"frame",5,15,  0,0,0,0};
struct shaderVar wldProjMatrix = {0,"WorldToProj",   11,15,  0,"frame",5,15,  0,0,0,0};
struct shaderVar fogTexture    = {0,"FogOfWarTex",   11,15,  0,"frame",5,15,  0,0,0,0};

shaderVar* inverseMatrixVar = &inverseMatrix;
shaderVar* wldProjMatrixVar = &wldProjMatrix;
shaderVar* fogTextureVar = &fogTexture;


const char *textureStore = "111111111111";
const char *texturePath = "/textures/engine/newFogOfWar.dds";

float worldToProjectionMatrix[16] = {};
float InverseWorldToProjectionMatrix[16] = {};

const float *wldToProjArray    = worldToProjectionMatrix;
const float *InvWldToProjArray = InverseWorldToProjectionMatrix;



void SetNewFramefxVars()
{
    asm(
        "movss dword ptr ss:[esp], xmm0;"  // default
        "call eax;"                         
        
        
        //InverseWorldToProjectMatrix
        "mov esi, %[inverseMatrixVar];"
        "call 0x00437ED0;"                 // ShaderVar::Exists
        "test al, al;"
        "je noVar_1;"
        
        "mov ecx, [esi+0x40];"
        "mov edx, [ecx];"
        "mov eax, [edx+0x14];"            
        "mov ebx, %[InvWldToProjArray];"
        "push ebx;"
        "call eax;"                         // SetMatrix
        
        
        "noVar_1:;"
        
        //WorldToProjectMatrix
        "mov esi, %[wldProjMatrixVar];"
        "call 0x00437ED0;"                 // ShaderVar::Exists
        "test al, al;"
        "je noVar_2;"
        
        "mov ecx, [esi+0x40];"
        "mov edx, [ecx];"
        "mov eax, [edx+0x14];"            
        "mov ebx, %[wldToProjArray];"
        "push ebx;"
        "call eax;"                         // SetMatrix
        
        
        "noVar_2:;"
        
        //Load and set fog texture
        "mov eax, %[textureStore];"
        "cmp dword ptr[eax], 0x31313131;"
        "jne fogTextureExists;"
        
        
        "lea ecx, [edi+0x40];"             //ECX should be ShaderTechnique + some offset. It differs for each texture var
                                           //and always points to 0. But without it texture load will crash.
              
        "push 0x1;"                        //allowCreate
        "push 0x0;"  
        "push %[texturePath];"
        "push %[textureStore];"
        "call 0x00441370;"                 //Load texture from path. Called once.
        
        "fogTextureExists:;"
        "mov eax, %[textureStore];"
        "push eax;"
        "mov eax, %[fogTextureVar];"
        "call 0x00438140;"

        "jmp 0x007F613D;"
        :
        :[wldProjMatrixVar]"m"(wldProjMatrixVar),
         [inverseMatrixVar]"m"(inverseMatrixVar),
         [InvWldToProjArray]"m"(InvWldToProjArray),
         [wldToProjArray]"m"(wldToProjArray),
         [textureStore]"m"(textureStore),
         [texturePath]"m"(texturePath),
         [fogTextureVar]"m"(fogTextureVar)
        :
    );
}

void InverseMat4x4()
{
    const float *m = wldToProjArray;
    float inv[16];
    float det;
    int i;

    inv[0]  =  m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    inv[4]  = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    inv[8]  =  m[4] * m[9]  * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
    inv[12] = -m[4] * m[9]  * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
    inv[1]  = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
    inv[5]  =  m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
    inv[9]  = -m[0] * m[9]  * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
    inv[13] =  m[0] * m[9]  * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
    inv[2]  =  m[1] * m[6]  * m[15] - m[1] * m[7]  * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7]  - m[13] * m[3] * m[6];
    inv[6]  = -m[0] * m[6]  * m[15] + m[0] * m[7]  * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7]  + m[12] * m[3] * m[6];
    inv[10] =  m[0] * m[5]  * m[15] - m[0] * m[7]  * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7]  - m[12] * m[3] * m[5];
    inv[14] = -m[0] * m[5]  * m[14] + m[0] * m[6]  * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6]  + m[12] * m[2] * m[5];
    inv[3]  = -m[1] * m[6]  * m[11] + m[1] * m[7]  * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9]  * m[2] * m[7]  + m[9]  * m[3] * m[6];
    inv[7]  =  m[0] * m[6]  * m[11] - m[0] * m[7]  * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8]  * m[2] * m[7]  - m[8]  * m[3] * m[6];
    inv[11] = -m[0] * m[5]  * m[11] + m[0] * m[7]  * m[9]  + m[4] * m[1] * m[11] - m[4] * m[3] * m[9]  - m[8]  * m[1] * m[7]  + m[8]  * m[3] * m[5];
    inv[15] =  m[0] * m[5]  * m[10] - m[0] * m[6]  * m[9]  - m[4] * m[1] * m[10] + m[4] * m[2] * m[9]  + m[8]  * m[1] * m[6]  - m[8]  * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det != 0)
    {
        det = 1.0 / det;
    }
    
    for (i = 0; i < 16; i++)
        InverseWorldToProjectionMatrix[i] = inv[i] * det;
}

//Save WorldToProjection matrix from particle.fx
//Also make inverse version of it in InverseMat4x4
//TODO: call InverseMat4x4()once per frame instead of every time particle.fx is called
//probably by comparinig some floats in matrix and if they stay unchanged
//then we are in same frame or camera isn't moving so no need to call InverseMat4x4
void SaveMatrices()
{
    asm(
        "lea eax, ds:[ebx+0x9C];"
        "mov %[wldToProjArray], eax;" //Save matrix
        "push eax;"
        "call edx;"
        
        "call %[InverseMat4x4];"

        "jmp 0x00495342;"
        :
        :[wldToProjArray]"m"(wldToProjArray),
         [InverseMat4x4]"i"(InverseMat4x4)
        :
    );
}


//////////DEBUG PART////////////////

int LuaMousePosXYZ(lua_State *l)
{
    float mX = luaL_checknumber(l, 1);
    float mY = luaL_checknumber(l, 2);
    float mZ = luaL_checknumber(l, 3);
    float mW = 1.0f;

    float line1_1 = wldToProjArray[0];
    float line1_2 = wldToProjArray[4];
    float line1_3 = wldToProjArray[8];
    float line1_4 = wldToProjArray[12];
    
    float line2_1 = wldToProjArray[1];
    float line2_2 = wldToProjArray[5];
    float line2_3 = wldToProjArray[9];
    float line2_4 = wldToProjArray[13];
    
    float line3_1 = wldToProjArray[2];
    float line3_2 = wldToProjArray[6];
    float line3_3 = wldToProjArray[10];
    float line3_4 = wldToProjArray[14];
    
    float line4_1 = wldToProjArray[3];
    float line4_2 = wldToProjArray[7];
    float line4_3 = wldToProjArray[11];
    float line4_4 = wldToProjArray[15];
    
    float viewX = line1_1*mX + line1_2*mY + line1_3*mZ + line1_4*mW;
    float viewY = line2_1*mX + line2_2*mY + line2_3*mZ + line2_4*mW;
    float viewZ = line3_1*mX + line3_2*mY + line3_3*mZ + line3_4*mW;
    float viewW = line4_1*mX + line4_2*mY + line4_3*mZ + line4_4*mW;
    
    InverseMat4x4();
    
    float inViewX = InvWldToProjArray[0]*viewX + InvWldToProjArray[4]*viewY + InvWldToProjArray[8]*viewZ + InvWldToProjArray[12]*viewW;
    float inViewY = InvWldToProjArray[1]*viewX + InvWldToProjArray[5]*viewY + InvWldToProjArray[9]*viewZ + InvWldToProjArray[13]*viewW;
    float inViewZ = InvWldToProjArray[2]*viewX + InvWldToProjArray[6]*viewY + InvWldToProjArray[10]*viewZ + InvWldToProjArray[14]*viewW;
    float inViewW = InvWldToProjArray[3]*viewX + InvWldToProjArray[7]*viewY + InvWldToProjArray[11]*viewZ + InvWldToProjArray[15]*viewW;
    
    char buf[16] = {};
    
    LogF("--------MOUSE_WORLD_POS-------------");
    LogF("-");
    sprintf_s(buf, sizeof(buf), "%f", mX);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", mY);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", mZ);
    LogF(buf);
    LogF("-");
    
    LogF("-----MOUSE_Pos * WorldToProjection4x4-----");
    LogF("-");
    sprintf_s(buf, sizeof(buf), "%f", viewX);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", viewY);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", viewZ);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", viewW);
    LogF(buf);
    LogF("-");
    
    // LogF("-----MOUSE_Projection_Pos * INVERSE_WorldToProjection4x4-----");
    // LogF("-");
    // sprintf_s(buf, sizeof(buf), "%f", inViewX);
    // LogF(buf);
    // sprintf_s(buf, sizeof(buf), "%f", inViewY);
    // LogF(buf);
    // sprintf_s(buf, sizeof(buf), "%f", inViewZ);
    // LogF(buf);
    // sprintf_s(buf, sizeof(buf), "%f", inViewW);
    // LogF(buf);
    // LogF("-");
    

    LogF("----------WorldToProj4x4---------------");
    LogF("-");
    LogF("--ROW 1--");
    sprintf_s(buf, sizeof(buf), "%f", line1_1);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line1_2);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line1_3);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line1_4);
    LogF(buf);
    LogF("-");
    
    LogF("--ROW 2--");
    sprintf_s(buf, sizeof(buf), "%f", line2_1);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line2_2);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line2_3);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line2_4);
    LogF(buf);
    LogF("-");
    
    LogF("--ROW 3--");
    sprintf_s(buf, sizeof(buf), "%f", line3_1);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line3_2);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line3_3);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line3_4);
    LogF(buf);
    LogF("-");
    
    LogF("--ROW 4--");
    sprintf_s(buf, sizeof(buf), "%f", line4_1);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line4_2);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line4_3);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", line4_4);
    LogF(buf);
    LogF("-");
    
    
    LogF("----------InverseWorldToProj4x4---------------");
    LogF("-");
    LogF("--ROW 1--");
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[0]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[4]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[8]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[12]);
    LogF(buf);
    LogF("-");
    
    LogF("--ROW 2--");
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[1]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[5]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[9]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[13]);
    LogF(buf);
    LogF("-");
    
    LogF("--ROW 3--");
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[2]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[6]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[10]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[14]);
    LogF(buf);
    LogF("-");
    
    LogF("--ROW 4--");
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[3]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[7]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[11]);
    LogF(buf);
    sprintf_s(buf, sizeof(buf), "%f", InverseWorldToProjectionMatrix[15]);
    LogF(buf);
    LogF("-");
    

     return 0;
}

UIRegFunc MousePosXYZReg{"MouseWorldPosToProjection", "UI_MousePosXYZ(posX:float, posY:float, PosZ:float)", LuaMousePosXYZ};