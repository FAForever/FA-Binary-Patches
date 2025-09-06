#include "CObject.h"
#include "magic_classes.h"
#include "moho.h"
#include "utility.h"
#include <cmath>

#define NON_GENERAL_REG(var_) [var_] "g"(var_)


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

struct shaderVar lowerAlbTex      = {0,"LowerAlbTex", 11,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat0AlbTex     = {0,"Strat0AlbTex",12,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat1AlbTex     = {0,"Strat1AlbTex",12,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat2AlbTex     = {0,"Strat2AlbTex",12,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat3AlbTex     = {0,"Strat3AlbTex",12,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat4AlbTex     = {0,"Strat4AlbTex",12,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat5AlbTex     = {0,"Strat5AlbTex",12,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat6AlbTex     = {0,"Strat6AlbTex",12,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat7AlbTex     = {0,"Strat7AlbTex",12,15,  0,"mesh",4,15,  0,0,0,0};

struct shaderVar upperAlbTex      = {0,"UpperAlbTex", 11,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar lowerNormTex     = {0,"LowerNormTex",12,15,  0,"mesh",4,15,  0,0,0,0};

struct shaderVar strat0NormTex    = {0,"Strat0NormTex",13,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat1NormTex    = {0,"Strat1NormTex",13,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat2NormTex    = {0,"Strat2NormTex",13,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat3NormTex    = {0,"Strat3NormTex",13,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat4NormTex    = {0,"Strat4NormTex",13,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat5NormTex    = {0,"Strat5NormTex",13,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat6NormTex    = {0,"Strat6NormTex",13,15,  0,"mesh",4,15,  0,0,0,0};
struct shaderVar strat7NormTex    = {0,"Strat7NormTex",13,15,  0,"mesh",4,15,  0,0,0,0};


shaderVar* lowerAlbTexPtr = &lowerAlbTex;
shaderVar* s0AlbTexPtr    = &strat0AlbTex;
shaderVar* s1AlbTexPtr    = &strat1AlbTex;
shaderVar* s2AlbTexPtr    = &strat2AlbTex;
shaderVar* s3AlbTexPtr    = &strat3AlbTex;
shaderVar* s4AlbTexPtr    = &strat4AlbTex;
shaderVar* s5AlbTexPtr    = &strat5AlbTex;
shaderVar* s6AlbTexPtr    = &strat6AlbTex;
shaderVar* s7AlbTexPtr    = &strat7AlbTex;

shaderVar* upAlbTexPtr    = &upperAlbTex;
shaderVar* lowNormTexPtr  = &lowerNormTex;

shaderVar* s0NormTexPtr  = &strat0NormTex;
shaderVar* s1NormTexPtr  = &strat1NormTex;
shaderVar* s2NormTexPtr  = &strat2NormTex;
shaderVar* s3NormTexPtr  = &strat3NormTex;
shaderVar* s4NormTexPtr  = &strat4NormTex;
shaderVar* s5NormTexPtr  = &strat5NormTex;
shaderVar* s6NormTexPtr  = &strat6NormTex;
shaderVar* s7NormTexPtr  = &strat7NormTex;

const char *lowerAlbTexStore = "1234";
const char *s0AlbTexStore = "1234";
const char *s1AlbTexStore = "1234";
const char *s2AlbTexStore = "1234";
const char *s3AlbTexStore = "1234";
const char *s4AlbTexStore = "1234";
const char *s5AlbTexStore = "1234";
const char *s6AlbTexStore = "1234";
const char *s7AlbTexStore = "1234";

const char *upAlbTexStore = "1234";
const char *lowNormTexStore = "1234";

const char *s0NormTexStore = "1234";
const char *s1NormTexStore = "1234";
const char *s2NormTexStore = "1234";
const char *s3NormTexStore = "1234";
const char *s4NormTexStore = "1234";
const char *s5NormTexStore = "1234";
const char *s6NormTexStore = "1234";
const char *s7NormTexStore = "1234";



void SetMeshFxTextures()
{
    asm(
        "call 0x00438140;"
        
        "mov eax, %[lowerAlbTexStore];"
        "push eax;"
        "mov eax, %[lowerAlbTexPtr];"
        "call 0x438140;"
        
        "mov eax, %[s0AlbTexStore];"
        "push eax;"
        "mov eax, %[s0AlbTexPtr];"
        "call 0x438140;"

        "mov eax, %[s1AlbTexStore];"
        "push eax;"
        "mov eax, %[s1AlbTexPtr];"
        "call 0x438140;"  
        
        "mov eax, %[s2AlbTexStore];"
        "push eax;"
        "mov eax, %[s2AlbTexPtr];"
        "call 0x438140;"   
        
        "mov eax, %[s3AlbTexStore];"
        "push eax;"
        "mov eax, %[s3AlbTexPtr];"
        "call 0x438140;" 
        
        "mov eax, %[s4AlbTexStore];"
        "push eax;"
        "mov eax, %[s4AlbTexPtr];"
        "call 0x438140;"
        
        "mov eax, %[s5AlbTexStore];"
        "push eax;"
        "mov eax, %[s5AlbTexPtr];"
        "call 0x438140;"

        "mov eax, %[s6AlbTexStore];"
        "push eax;"
        "mov eax, %[s6AlbTexPtr];"
        "call 0x438140;"

        "mov eax, %[s7AlbTexStore];"
        "push eax;"
        "mov eax, %[s7AlbTexPtr];"
        "call 0x438140;"

        "mov eax, %[upAlbTexStore];"
        "push eax;"
        "mov eax, %[upAlbTexPtr];"
        "call 0x438140;"

        "mov eax, %[lowNormTexStore];"
        "push eax;"
        "mov eax, %[lowNormTexPtr];"
        "call 0x438140;" 
     
     
        :
        :[lowerAlbTexPtr]"m"(lowerAlbTexPtr), [s0AlbTexPtr]"m"(s0AlbTexPtr), [s1AlbTexPtr]"m"(s1AlbTexPtr),
         [s2AlbTexPtr]"m"(s2AlbTexPtr),[s3AlbTexPtr]"m"(s3AlbTexPtr),[s4AlbTexPtr]"m"(s4AlbTexPtr),
         [s5AlbTexPtr]"m"(s5AlbTexPtr),[s6AlbTexPtr]"m"(s6AlbTexPtr),[s7AlbTexPtr]"m"(s7AlbTexPtr), 
         [upAlbTexPtr]"m"(upAlbTexPtr), [lowNormTexPtr]"m"(lowNormTexPtr),
         [lowerAlbTexStore]"m"(lowerAlbTexStore), [s0AlbTexStore]"m"(s0AlbTexStore), [s1AlbTexStore]"m"(s1AlbTexStore),
         [s2AlbTexStore]"m"(s2AlbTexStore), [s3AlbTexStore]"m"(s3AlbTexStore), [s4AlbTexStore]"m"(s4AlbTexStore),
         [s5AlbTexStore]"m"(s5AlbTexStore), [s6AlbTexStore]"m"(s6AlbTexStore), [s7AlbTexStore]"m"(s7AlbTexStore), 
         [upAlbTexStore]"m"(upAlbTexStore), [lowNormTexStore]"m"(lowNormTexStore)
        :
    );
    
    asm(
        "mov eax, %[s0NormTexStore];"
        "push eax;"
        "mov eax, %[s0NormTexPtr];"
        "call 0x438140;"
        
        "mov eax, %[s1NormTexStore];"
        "push eax;"
        "mov eax, %[s1NormTexPtr];"
        "call 0x438140;"
        
        "mov eax, %[s2NormTexStore];"
        "push eax;"
        "mov eax, %[s2NormTexPtr];"
        "call 0x438140;"
        
        "mov eax, %[s3NormTexStore];"
        "push eax;"
        "mov eax, %[s3NormTexPtr];"
        "call 0x438140;"
        
        "mov eax, %[s4NormTexStore];"
        "push eax;"
        "mov eax, %[s4NormTexPtr];"
        "call 0x438140;"
        
        "mov eax, %[s5NormTexStore];"
        "push eax;"
        "mov eax, %[s5NormTexPtr];"
        "call 0x438140;"
        
        "mov eax, %[s6NormTexStore];"
        "push eax;"
        "mov eax, %[s6NormTexPtr];"
        "call 0x438140;"
        
        "mov eax, %[s7NormTexStore];"
        "push eax;"
        "mov eax, %[s7NormTexPtr];"
        "call 0x438140;"
        
        
        "jmp 0x007E1A6B;"
        
        :
        :[s0NormTexPtr]"m"(s0NormTexPtr),[s1NormTexPtr]"m"(s1NormTexPtr), [s2NormTexPtr]"m"(s2NormTexPtr),
         [s3NormTexPtr]"m"(s3NormTexPtr), [s4NormTexPtr]"m"(s4NormTexPtr), [s5NormTexPtr]"m"(s5NormTexPtr),
         [s6NormTexPtr]"m"(s6NormTexPtr), [s7NormTexPtr]"m"(s7NormTexPtr), 
         [s0NormTexStore]"m"(s0NormTexStore), [s1NormTexStore]"m"(s1NormTexStore),  [s2NormTexStore]"m"(s2NormTexStore),
         [s3NormTexStore]"m"(s3NormTexStore), [s4NormTexStore]"m"(s4NormTexStore), [s5NormTexStore]"m"(s5NormTexStore),
         [s6NormTexStore]"m"(s6NormTexStore), [s7NormTexStore]"m"(s7NormTexStore)
        :
    );
}

void StoreTerrainFxData()
{
    asm(
        "lea eax,dword ptr ds:[edi+0x64];"
        "push eax;"
        "mov %[lowerAlbTexStore], eax;"         //TerrainLowerAlbedoTexture
        "mov eax,0x10BFB80;"
        "call 0x438140;"
        
        "lea ecx,dword ptr ds:[edi+0x9C];"    
        "push ecx;"
        "mov %[s0AlbTexStore], ecx;"            //Stratum0AlbedoTexture
        "mov eax,0x10C0750;"
        "call 0x438140;"
        
        "lea edx,dword ptr ds:[edi+0xD4];"
        "push edx;"
        "mov %[s1AlbTexStore], edx;"            //Stratum1AlbedoTexture 
        "mov eax,0x10C0510;"
        "call 0x438140;"
        
        "lea eax,dword ptr ds:[edi+0x10C];"
        "push eax;"
        "mov %[s2AlbTexStore], eax;"            //Stratum2AlbedoTexture 
        "mov eax,0x10C0948;"
        "call 0x438140;"
        
        "lea ecx,dword ptr ds:[edi+0x144];"
        "push ecx;"
        "mov %[s3AlbTexStore], ecx;"            //Stratum3AlbedoTexture 
        "mov eax,0x10C08B8;"
        "call 0x438140;"
        
        "lea edx,dword ptr ds:[edi+0x17C];"
        "push edx;"
        "mov %[s4AlbTexStore], edx;"            //Stratum4AlbedoTexture 
        "mov eax,0x10BFBC8;"
        "call 0x438140;"
        
        "lea eax,dword ptr ds:[edi+0x1B4];"
        "push eax;"
        "mov %[s5AlbTexStore], eax;"            //Stratum5AlbedoTexture
        "mov eax,0x10C0288;"
        "call 0x438140;"
        
        
        "lea ecx,dword ptr ds:[edi+0x1EC];"
        "push ecx;"
        "mov %[s6AlbTexStore], ecx;"            //Stratum6AlbedoTexture
        "mov eax,0x10C0900;"
        "call 0x438140;"
        
        "lea edx,dword ptr ds:[edi+0x224];"
        "push edx;"
        "mov %[s7AlbTexStore], edx;"            //Stratum7AlbedoTexture
        "mov eax,0x10BFA18;"
        "call 0x438140;"
        
        
        "lea eax,dword ptr ds:[edi+0x25C];"
        "push eax;"
        "mov %[upAlbTexStore], eax;"            //UpperAlbedoTexture
        "mov eax,0x10C09D8;"
        "call 0x438140;"
        
        "lea ecx,dword ptr ds:[edi+0x294];"
        "push ecx;"
        "mov %[lowNormTexStore], ecx;"          //LowerNormalTexture
        "mov eax,0x10C0048;"
        "call 0x438140;"
        
        "lea edx,dword ptr ds:[edi+0x2CC];"
        "push edx;"
        "mov %[s0NormTexStore], edx;"          //Stratum0NormalTexture
        "mov eax,0x10C0318;"
        "call 0x438140;"
        
        "lea eax,dword ptr ds:[edi+0x304];"
        "push eax;"
        "mov %[s1NormTexStore], eax;"          //Stratum1NormalTexture
        "mov eax,0x10C01B0;"
        "call 0x438140;"
        
        "lea ecx,dword ptr ds:[edi+0x33C];"
        "push ecx;"
        "mov %[s2NormTexStore], ecx;"          //Stratum2NormalTexture
        "mov eax,0x10C0D98;"
        "call 0x438140;"
        
        "lea edx,dword ptr ds:[edi+0x374];"
        "push edx;"
        "mov %[s3NormTexStore], edx;"          //Stratum3NormalTexture
        "mov eax,0x10C0828;"
        "call 0x438140;"
        
        "lea eax,dword ptr ds:[edi+0x3AC];"
        "push eax;"
        "mov %[s4NormTexStore], eax;"          //Stratum4NormalTexture
        "mov eax,0x10BF7D8;"
        "call 0x438140;"
        
        "lea ecx,dword ptr ds:[edi+0x3E4];"
        "push ecx;"
        "mov %[s5NormTexStore], ecx;"          //Stratum5NormalTexture
        "mov eax,0x10C0BE8;"
        "call 0x438140;"
        
        "lea edx,dword ptr ds:[edi+0x41C];"
        "push edx;"
        "mov %[s6NormTexStore], edx;"          //Stratum6NormalTexture
        "mov eax,0x10C0090;"
        "call 0x438140;"
        
        "lea eax,dword ptr ds:[edi+0x454];"
        "push eax;"
        "mov %[s7NormTexStore], eax;"          //Stratum7NormalTexture
        "mov eax,0x10C0C78;"
        "call 0x438140;"
        
        "jmp 0x00800FBF;"
        
        :
        :[lowerAlbTexStore]"m"(lowerAlbTexStore), [s0AlbTexStore]"m"(s0AlbTexStore), [s1AlbTexStore]"m"(s1AlbTexStore),
         [s2AlbTexStore]"m"(s2AlbTexStore), [s3AlbTexStore]"m"(s3AlbTexStore), [s4AlbTexStore]"m"(s4AlbTexStore),
         [s5AlbTexStore]"m"(s5AlbTexStore), [s6AlbTexStore]"m"(s6AlbTexStore), [s7AlbTexStore]"m"(s7AlbTexStore), 
         [upAlbTexStore]"m"(upAlbTexStore), [lowNormTexStore]"m"(lowNormTexStore),
         [s0NormTexStore]"m"(s0NormTexStore), [s1NormTexStore]"m"(s1NormTexStore),  [s2NormTexStore]"m"(s2NormTexStore),
         [s3NormTexStore]"m"(s3NormTexStore), [s4NormTexStore]"m"(s4NormTexStore), [s5NormTexStore]"m"(s5NormTexStore),
         [s6NormTexStore]"m"(s6NormTexStore), [s7NormTexStore]"m"(s7NormTexStore)
        :
    );
}