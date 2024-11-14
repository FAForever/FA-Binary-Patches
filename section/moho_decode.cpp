#pragma GCC optimize("O3")

#include "desync_fix_global.h"

const char* safe_quit = "safeQuit";
uint32_t tick_num = 0;
static bool update_pl_count = true;
uint32_t terminated = 0;
uint32_t num_clients = 0;
uint32_t current_num_clients = 0;
bool paused = false;

int strCmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void Conexecute()
{
	register int eax asm("eax");
	asm(
		"push ebp;"
		"mov ebp,esp;"
		"and esp,0xFFFFFFF8;"
		"sub esp,0x8;"
		"push esi;"
		"mov esi,eax;"
		"mov eax,dword ptr [esi];"
		"push edi;"
		"mov edi,dword ptr [0x10A7704];"
		"push eax;"
		"call 0x90C590;"
		"add esp,0x4;"
		"cmp eax,0x1;"
		"je L0xABEL_0x0041CC15;"
		"push eax;"
		"push 0x1;"
		"push edi;"
		"push 0xE0A220;"
		"push esi;"
		"call 0x90C1D0;"
		"add esp,0x14;"
		"L0xABEL_0x0041CC15:;"
		"mov dword ptr [esp+0x8],esi;"
		"mov dword ptr [esp+0xC],0x1;"
		"mov ecx,dword ptr [esi];"
		"push 0x1;"
		"push ecx;"
		"call 0x90CA90;"
		"mov esi,eax;"
	);
	if(strCmp(eax, safe_quit) == 0)
	{
		asm(
			"mov dword ptr [0x011FD24F], 0x1;"
			"add esp,0x8;"
			"pop edi;"
			"xor eax,eax;"
			"pop esi;"
			"mov esp,ebp;"
			"pop ebp;"
		);
		return;
	}
	asm(

		"add esp,0x8;"
		"test esi,esi;"
		"jne L0xABEL_0x0041CC42;"
		"push 0xE00AD0;"
		"lea ecx,dword ptr [esp+0xC];"
		"call 0x4154B0;"
		"L0xABEL_0x0041CC42:;"
		"push esi;"
		"call 0x41CC90;"
		"add esp,0x4;"
		"pop edi;"
		"xor eax,eax;"
		"pop esi;"
		"mov esp,ebp;"
		"pop ebp;"
	);
	return;
}

__attribute__((noinline)) void timed_out()
{
	LogF("SafeQuit sync timed out");
	return;
}

__attribute__((noinline)) void p_Version()
{
	LogF("Exe GitSHA: %s", gitsha);
	return;
}

__attribute__((noinline)) void xor_sync()
{
	register int eax asm("eax");
	asm("xor eax,eax;");
	for(int i = 0; i<16; i++)
	{
		sync_buffer[i] = eax;
	}
	return;
}

void SessionEndGame()
{
	register int eax asm("eax");
	asm(
		"Sleep = 0x00C0F574;"
		"GetCurrentThread = 0x00C0F588;"
		"SuspendThread = 0x00C0F4F8;"
		"WaitForSingleObject = 0x00C0F524;"
	);
	if(current_num_clients > 1 && current_num_clients < 17 && !game_ended)
	{
		asm(
			"mov dword ptr [0x011FD243], 0x1;" // send network var
			"push 0x2500;"
			"push dword ptr [0x11FD253];"
			"call dword ptr [WaitForSingleObject];"
		);
		if(eax == 0x00000102)
		{
			asm(
				"call %[func];"
				:
				: [func] "i" (&timed_out)
				: "memory"
			);
		}
/* 		asm(
			"push 100;"
			"call dword ptr [Sleep];"
		); */
	}
	asm(
		//"push 30000;"
		//"call dword ptr [Sleep];"
		"mov dword ptr [0x011FD243], 0xC;" //reset send trigger
		"mov eax,dword ptr [edi];"
		"push esi;"
		"mov esi,dword ptr [0x10C5234];"
		"push eax;"
		"call 0x90C590;"
		"add esp,0x4;"
		"test eax,eax;"
		"je L0xABEL_0x008984D8;"
		"push eax;"
		"push 0x0;"
		"push esi;"
		"push 0xE0A220;"
		"push edi;"
		"call 0x90C1D0;"
		"add esp,0x14;"
		"L0xABEL_0x008984D8:;"
		"mov eax,dword ptr [0x10C4F50];"
		"test eax,eax;"
		"mov esi,eax;"
		"jne L0xABEL_0x008984F1;"
		"push 0xE4AFCC;"
		"push edi;"
		"call 0x90C1D0;"
		"add esp,0x8;"
		"L0xABEL_0x008984F1:;"
		"mov edx,dword ptr [esi];"
		"mov eax,dword ptr [edx+0x4];"
		"mov ecx,esi;"
		"call eax;"
	);

	asm(
		"call %[func];"
		:
		: [func] "i" (&xor_sync)
		: "memory"
	);

	asm(
		"mov dword ptr [0x011FD23F], 0xB;" //RESET BLOCK VARIABLE
		//"mov dword ptr [0x011FD243], 0xC;" //reset send trigger
		"xor eax,eax;"
		"pop esi;"
		"ret;"
	);
}

__attribute__((noinline)) void err_print()
{
	LogF("Block is too late !");
	return;
}

__attribute__((noinline)) void term()
{
	LogF("terminated");
	return;
}

void Moho__CDecoder__DecodeMessage()
{
	register int eax asm("eax");
	asm(
		"push ebp;"
		"mov ebp,esp;"
		"and esp,0xFFFFFFF8;"
		"push 0xFFFFFFFF;"
		"push 0xBA8D10;"
		//"mov eax,dword ptr [0x0];"
		"push eax;"
		//"mov dword ptr [0x0],esp;"
		"sub esp,0x70;"
		"push esi;"
		"mov esi,ecx;"
		"lea ecx,dword ptr [esp+0x28];"
		"call 0x956DB0;"
		"mov ecx,dword ptr [edi];"
		"xor edx,edx;"
		"mov dword ptr [esp+0x28],0xE03BEC;"
		"mov dword ptr [esp+0x44],edi;"
		"mov dh,byte ptr [ecx+0x2];"
		"mov eax,dword ptr [edi];"
		"add eax,0x3;"
		"mov dl,byte ptr [ecx+0x1];" // <- test for size of the message
		"cmp edx,0x3;"
		"jae L0xABEL_0x006E419E;"
		"xor edx,edx;"
		"jmp L0xABEL_0x006E41A1;"
		"L0xABEL_0x006E419E:;"
		"add edx,0xFFFFFFFD;"
		"L0xABEL_0x006E41A1:;"
		"add edx,eax;"
		"mov dword ptr [esp+0x30],eax;"
		"mov dword ptr [esp+0x2C],eax;"
		"mov dword ptr [esp+0x34],edx;"
		"mov dword ptr [esp+0x7C],0x0;"
		"mov cl,byte ptr [ecx];"
		"lea eax,dword ptr [esp+0x28];"
		"mov dword ptr [esp+0x8],eax;"
		"movzx eax,cl;"

/* 		"cmp dword ptr [0x011FD23F], 0x1;"
		"jne skip;"
		"mov byte ptr [0x10A6380],0x0;"
		"inc dword ptr [0x011FD23F+0xC];"
		"cmp dword ptr [0x011FD23F+0xC], 0x4;"
		"jl skip;"
		"mov dword ptr [0x011FD23F+0xC], 0x0;"
		"mov dword ptr [0x011FD23F], 0x0;"
		"skip:;" */
	);
	switch (eax)
	{
	case 0: //decode_advance -> On_Beat()
		asm(
		"lea ecx,dword ptr [esp+0x8];"
		"push ecx;"
		"call 0x6E4400;"
		);
		break;
	case 1: //decode_set_command_source
		asm(
		"push esi;"
		"lea eax,dword ptr [esp+0xC];"
		"call 0x6E4440;"
		);
		break;
	case 2:	// command source terminated

		//This acts both, like a boolean and a 32bit integer
		//this is what i like about C the most, there is no limitation of what you can do with
		//the data type, i bet you to try this in Java and see what kind of shit code you'll have to write
		//tag_sent = 0;
		asm(
		"cmp dword ptr [0x011FD23F], 0x1;"
		"jne late;"
		);
		terminated = tick_num;
		asm("jmp next;");
		asm("late:;");
		asm(
		    "call %[func];"
		    :
		    : [func] "i" (&err_print)
		    : "memory"
		);

		asm(
		"next:;"
		"mov ecx,dword ptr [esi+0xC];"
		"mov edx,dword ptr [ecx];"
		"mov eax,dword ptr [edx+0x4];"
		"call eax;"
		);
		asm(
		    "call %[func];"
		    :
		    : [func] "i" (&term)
		    : "memory"
		);
		current_num_clients--;
		break;
	case 3: //verify checksum
		asm(
		"push esi;"
		"lea esi,dword ptr [esp+0xC];"
		"call 0x6E4480;"
		);
		break;
	case 4: //sim request pause
		paused = true;
		asm(
		"pause:;"
		"mov ecx,dword ptr [esi+0xC];" //3
		"mov edx,dword ptr [ecx];"
		"mov eax,dword ptr [edx+0xC];"
		"call eax;"
		);
		break;
	case 5: // sim resume
		paused = false;
		asm(
		"mov ecx,dword ptr [esi+0xC];" //4
		"mov edx,dword ptr [ecx];"
		"mov eax,dword ptr [edx+0x10];"
		"call eax;"
		);
		break;
	case 6:
		asm(
		"mov ecx,dword ptr [esi+0xC];"
		"mov edx,dword ptr [ecx];"
		"mov eax,dword ptr [edx+0x14];"
		"call eax;"
		);
		break;
	case 7:
		asm(
		"push esi;"
		"lea ecx,dword ptr [esp+0xC];"
		"call 0x6E44F0;"
		);
		break;
	case 8:
		asm(
		"push esi;"
		"lea ecx,dword ptr [esp+0xC];" //7
		"call 0x6E45D0;"
		);
		break;
	case 9:
		asm(
		"push esi;"
		"lea eax,dword ptr [esp+0xC];"
		"call 0x6E4670;"
		);
		break;
	case 0xA:
		asm(
		"push esi;"
		"lea esi,dword ptr [esp+0xC];" //9
		"call 0x6E46A0;"
		);
		break;
	case 0xB:
		asm(
		"push esi;"
		"lea ecx,dword ptr [esp+0xC];" //10
		"call 0x6E46E0;"
		);
		break;
	case 0xC:
		asm(
		"push esi;"
		"lea esi,dword ptr [esp+0xC];" //11
		"call 0x6E47D0;"
		);
		break;
	case 0xD:
		asm(
		"push esi;"
		"lea esi,dword ptr [esp+0xC];" //12
		"call 0x6E48E0;"
		);
		break;
	case 0xE:
		asm(
		"push esi;"
		"lea esi,dword ptr [esp+0xC];" //13
		"call 0x6E49F0;"
		);
		break;
	case 0xF:
		asm(
		"push esi;"
		"lea esi,dword ptr [esp+0xC];" //14
		"call 0x6E4A30;"
		);
		break;
	case 0x10:
		asm(
		"push esi;"
		"lea eax,dword ptr [esp+0xC];" //15
		"call 0x6E4A70;"
		);
		break;
	case 0x11:
		asm(
		"push esi;"
		"lea esi,dword ptr [esp+0xC];" //16
		"call 0x6E4AC0;"
		);
		break;
	case 0x12:
		asm(
		"push esi;"
		"lea esi,dword ptr [esp+0xC];" //17
		"call 0x6E4B00;"
		);
		break;
	case 0x13:
		asm(
		"push esi;"
		"lea esi,dword ptr [esp+0xC];" //18
		"call 0x6E4BB0;"
		);
		break;
	case 0x14:
		asm(
		"push esi;"
		"lea ecx,dword ptr [esp+0xC];" //19
		"call 0x6E4D80;"
		);
		break;
	case 0x15:
		asm(
		"lea ecx,dword ptr [esp+0x8];" //20
		"call 0x6E4BF0;"
		);
		break;
	case 0x16:
		asm(
		"lea ecx,dword ptr [esp+0x8];" //21
		"call 0x6E4CA0;"
		);
		break;
	case 0x17:
		asm(
		"call 0x6E4E70;" //22
		);
		break;
	default:
		asm(
			"movzx ecx,cl;"
			"push ecx;"
			"lea edx,dword ptr [esp+0x10];"
			"push 0xE2E568;"
			"push edx;"
			"call 0x938F10;"
			"add esp,0xC;"
			"push eax;"
			"lea ecx,dword ptr [esp+0x4C];"
			"mov byte ptr [esp+0x80],0x1;"
			"call 0x405130;"
			"push 0xEC2624;"
			"lea eax,dword ptr [esp+0x4C];"
			"push eax;"
			"mov dword ptr [esp+0x50],0xE2E73C;"
			"call 0xA89950;"
			"int3;"
		);
		return;
	}
	asm(
		"lea ecx,dword ptr [esp+0x28];"
		"mov dword ptr [esp+0x7C],0xFFFFFFFF;"
		"call 0x956DD0;"
		"mov ecx,dword ptr [esp+0x74];"
		"pop esi;"
		"mov esp,ebp;"
		"pop ebp;"
		"ret;"
	);
}

void sim_dispatch()
{
	register int eax asm("eax");
	asm(
		"Sleep = 0x00C0F574;"
		"ResetEvent = 0x00C0F4CC;"
		"push ebp;"
		"mov ebp,esp;"
		"and esp,0xFFFFFFF8;"
		"push 0xFFFFFFFF;"
		"push 0xBB49E0;"
		//"mov eax,dword ptr [0x0];"
		"push eax;"
		//"mov dword ptr [0x0],esp;"
		"sub esp,0x4C;"
		"push ebx;"
		"mov ebx,ecx;"
		"push esi;"
		"lea ecx,dword ptr [esp+0x10];"
		"call 0x9556D0;"
		"mov eax,dword ptr [edi+0x20];"
		"mov dword ptr [esp+0x8],eax;"

		"add eax,0x1;" // counts sim ticks
	);
		tick_num = eax;
		if(terminated && eax >= (terminated + 10)) //wait 10 more sim ticks
		{
			asm(
				"mov dword ptr [0x011FD23F], 0xB;" //RESET BLOCK VARIABLE
				"mov dword ptr [0x011FD243], 0xC;" //reset send trigger
				"mov byte ptr  [0x011FD24F], 0x0;" //reset safeQuit
			);
			tag_sent = 0;
			terminated = 0;
			p_index = 0;
			sender_sock = 1;
		}

		static bool overflow = false;
		if(eax <= 10)
		{
			asm(
				"mov dword ptr [0x011FD23F], 0xB;" //RESET BLOCK VARIABLE
				"mov dword ptr [0x011FD243], 0xC;" //reset send trigger
				"mov byte ptr  [0x011FD24F], 0x0;" //reset safeQuit
			);
			tag_sent = 0;
			terminated = 0;
			p_index = 0;
			sender_sock = 1;
			game_ended = false;
			if(!overflow)
			{
				update_pl_count = true;
			}
		}
		else
		{
			overflow = false;
		}

		if(eax == 0xFFFFFFFF)
		{
			overflow = true;
		}
	asm(
		"mov dword ptr [edi+0x20],eax;"
		"cmp byte ptr [ebx+0x4],0x0;"
		"jne L0xABEL_0x0073D916;"
		"lea ecx,dword ptr [esp+0x18];"
		"call 0xAC1E60;"
		"push 0xEC2180;"
		"lea eax,dword ptr [esp+0x1C];"
		"push eax;"
		"call 0xA89950;"
		"L0xABEL_0x0073D916:;"
		"mov ecx,dword ptr [ebx];"
		"call 0xAC1AD0;"
		"mov eax,0xE3333C;"
		"lea esi,dword ptr [esp+0x38];"
		"mov byte ptr [ebx+0x4],0x0;"
		"call 0x4E6DF0;"
		"mov dword ptr [esp+0x5C],0x0;"
		"cmp dword ptr [0x10C7694],0x0;"
		"jne L0xABEL_0x0073D962;"
		"call 0x408940;"
		"push 0xE00B5C;"
		"mov ecx,eax;"
		"call 0x417B60;"
		"mov esi,0x1;"
		"mov edx,eax;"
		"mov dword ptr [0x10C7694],eax;"
		"call 0x4151E0;"
		"L0xABEL_0x0073D962:;"
		"mov ecx,dword ptr [0x10C7694];"
		"mov dword ptr [esp+0x18],ecx;"
		"lea ecx,dword ptr [esp+0x20];"
		"call 0x9556D0;"
		"mov eax,dword ptr [esp+0x8];"
		"mov byte ptr [esp+0x5C],0x1;"
		"mov ecx,dword ptr [edi+0x8];"
		"mov edx,dword ptr [ecx];"
		"mov edx,dword ptr [edx+0x58];"
		"push eax;"
		"call edx;"
		"mov eax,dword ptr [0x10A63E8];"
		"test eax,eax;"
		"je L0xABEL_0x0073D999;"
		"push eax;"
		"call dword ptr [Sleep];"
		"L0xABEL_0x0073D999:;"
		"lea ecx,dword ptr [esp+0x20];"
		"mov byte ptr [esp+0x5C],0x0;"
		"call 0x955700;"
		"push edx;"
		"push eax;"
		"call 0x955520;"
		"mov ecx,dword ptr [esp+0x20];"
		"add esp,0x8;"
		"mov dword ptr [esp+0xC],edx;"
		"add ecx,0x24;"
		"lock xadd dword ptr [ecx],eax;"
		"lea esi,dword ptr [esp+0x38];"
		"mov dword ptr [esp+0x5C],0xFFFFFFFF;"
		"call 0x4E6E90;"
		"cmp byte ptr [ebx+0x4],0x0;"
		"je L0xABEL_0x0073D9EF;"
		"lea ecx,dword ptr [esp+0x28];"
		"call 0xAC1E60;"
		"push 0xEC2180;"
		"lea edx,dword ptr [esp+0x2C];"
		"push edx;"
		"call 0xA89950;"
		"L0xABEL_0x0073D9EF:;"
		"mov ecx,dword ptr [ebx];"
		"call 0xAC1AB0;"
		"mov byte ptr [ebx+0x4],0x1;"
		"mov ecx,ebx;"
		"mov ebx,edi;"
		"call 0x73DAD0;"
		"cmp byte ptr [edi+0x68],0x0;"
		"jne L0xABEL_0x0073DAB7;"
		"lea ecx,dword ptr [esp+0x10];"
		"call 0x955700;"
		"push edx;"
		"push eax;"
		"call 0x9554E0;"
		"fstp dword ptr [esp+0x10];"
		"mov eax,dword ptr [edi+0x224];"
		"lea ecx,dword ptr [edi+0x1B8];"
		"add eax,0x1;"
		"cdq;"
		"mov ebx,0x19;"
		"idiv ebx;"
		"mov esi,dword ptr [ecx+0x68];"
		"add esp,0x8;"
		"cmp edx,esi;"
		"jne L0xABEL_0x0073DA4D;"
		"lea eax,dword ptr [esi+0x1];"
		"cdq;"
		"mov esi,ebx;"
		"idiv esi;"
		"mov dword ptr [ecx+0x68],edx;"
		"L0xABEL_0x0073DA4D:;"
		"mov eax,dword ptr [ecx+0x6C];"
		"lea eax,dword ptr [ecx+eax*0x4];"
		"test eax,eax;"
		"je L0xABEL_0x0073DA61;"
		"movss xmm0,dword ptr [esp+0x8];"
		"movss dword ptr [eax],xmm0;"
		"L0xABEL_0x0073DA61:;"
		"mov eax,dword ptr [ecx+0x6C];"
		"add eax,0x1;"
		"cdq;"
		"mov esi,ebx;"
		"idiv esi;"
		"mov dword ptr [ecx+0x6C],edx;"
		"call 0x48BF00;"
		"movss xmm1,dword ptr [0xE4F814];"
		"divss xmm1,xmm0;"
		"mulss xmm1,dword ptr [0xE4F714];"
		"movss dword ptr [esp+0x8],xmm1;"
		"fld dword ptr [esp+0x8];"
		"push ecx;"
		"fstp dword ptr [esp];"
		"call 0x55AE10;"
		"add esp,0x4;"
		"cmp eax,dword ptr [edi+0x228];"
		"je L0xABEL_0x0073DAB7;"
		"mov ecx,dword ptr [edi+0x8];"
		"mov dword ptr [edi+0x228],eax;"
		"mov edx,dword ptr [ecx];"
		"push eax;"
		"mov eax,dword ptr [edx+0x40];"
		"call eax;"
		"L0xABEL_0x0073DAB7:;"
		"mov ecx,dword ptr [esp+0x54];"
		"pop esi;"
		//"mov dword ptr [0x0],ecx;"
		"pop ebx;"
		"mov esp,ebp;"
		"pop ebp;"
	//	"ret;"
	);
	return;
}

void Update_Pipeline_Stream()
{
	register int eax asm("eax");
	asm(
		"push ebp;"
		"mov ebp,esp;"
		"and esp,0xFFFFFFF8;"
		"push 0xFFFFFFFF;"
		"push 0xBAA10B;"
	//	"mov eax,dword ptr [0x0];"
		"push eax;"
	//	"mov dword ptr [0x0],esp;"
		"sub esp,0x88;"
		"push ebx;"
		"push esi;"
		"push edi;"
		"mov edi,ecx;"
		"lea ecx,dword ptr [edi+0x40C];"
		"mov dword ptr [esp+0x18],ecx;"
		"call 0xAC51B0;"
		"mov byte ptr [esp+0x1C],0x1;"
		"xor esi,esi;"
		"mov dword ptr [esp+0x9C],esi;"
		"mov eax,dword ptr [edi+0x420];"
		"cmp eax,esi;"
		"je L0xABEL_0x0053F098;"
		"mov ebx,dword ptr [edi+0x424];"
		"sub ebx,eax;"
		"sar ebx,0x2;"
		"je L0xABEL_0x0053F098;"
		"lea ebx,dword ptr [ebx];"
		"L0xABEL_0x0053F070:;"
		"mov ecx,dword ptr [edi+0x420];"
		"mov edx,dword ptr [ecx+esi*0x4];"
		"lea eax,dword ptr [edi+0x18470];"
		"push eax;"
		"lea eax,dword ptr [edi+0x184B8];"
		"push eax;"
		"mov eax,dword ptr [ebp+0x8];"
		"push eax;"
		"push edx;"
		"call 0x53C550;"
		"add esi,0x1;"
		"cmp esi,ebx;"
		"jb L0xABEL_0x0053F070;"
		"L0xABEL_0x0053F098:;"
		"mov eax,ebx;"
	);
		num_clients = eax;
		if(update_pl_count)
		{
			asm(
				"call %[func];"
				:
				: [func] "i" (&p_Version)
				: "memory"
			);
			current_num_clients = num_clients;
			update_pl_count = false;
		}
	asm(
		"push 0x33;"
		"lea eax,dword ptr [esp+0x44];"
		"push eax;"
		"xor edx,edx;"
		"call 0x483490;"
		"lea ecx,dword ptr [esp+0x20];"
		"mov byte ptr [esp+0x9C],0x1;"
		"call 0x956DB0;"
		"lea ecx,dword ptr [esp+0x40];"
		"mov dword ptr [esp+0x3C],ecx;"
		"mov ecx,dword ptr [esp+0x40];"
		"xor edx,edx;"
		"mov dword ptr [esp+0x20],0xE03BEC;"
		"mov dh,byte ptr [ecx+0x2];"
		"lea eax,dword ptr [ecx+0x3];"
		"mov dl,byte ptr [ecx+0x1];"
		"mov ecx,edx;"
		"cmp ecx,0x3;"
		"jae L0xABEL_0x0053F0E1;"
		"xor ecx,ecx;"
		"jmp L0xABEL_0x0053F0E4;"
		"L0xABEL_0x0053F0E1:;"
		"add ecx,0xFFFFFFFD;"
		"L0xABEL_0x0053F0E4:;"
		"add ecx,eax;"
		"mov dword ptr [esp+0x28],eax;"
		"mov dword ptr [esp+0x24],eax;"
		"mov dword ptr [esp+0x2C],ecx;"
		"mov dword ptr [esp+0x34],eax;"
		"mov dword ptr [esp+0x30],eax;"
		"mov dword ptr [esp+0x38],ecx;"
		"sub ecx,eax;"
		"cmp ecx,0x4;"
		"mov byte ptr [esp+0x9C],0x2;"
		"jb L0xABEL_0x0053F119;"
		"mov ecx,dword ptr [ebp+0x8];"
		"mov dword ptr [eax],ecx;"
		"add dword ptr [esp+0x34],0x4;"
		"jmp L0xABEL_0x0053F129;"
		"L0xABEL_0x0053F119:;"
		"push 0x4;"
		"lea edx,dword ptr [ebp+0x8];"
		"push edx;"
		"lea ecx,dword ptr [esp+0x28];"
		"call dword ptr [0xE03C08];"
		"L0xABEL_0x0053F129:;"
		"mov eax,dword ptr [edi];"
		"mov edx,dword ptr [eax+0x44];"
		"lea ecx,dword ptr [esp+0x40];"
		"push ecx;"
		"mov ecx,edi;"
		"call edx;"
		"mov ecx,dword ptr [esp+0x18];"
		"add dword ptr [edi+0x438],0x1;"
		"call 0xAC5200;"
		"mov eax,dword ptr [edi+0x18470];"
		"mov edx,dword ptr [eax+0x20];"
		"lea esi,dword ptr [edi+0x18470];"
		"mov ecx,esi;"
		"mov byte ptr [esp+0x1C],0x0;"
		"call edx;"
		"mov ecx,esi;"
		"call 0x9566C0;"
		"test eax,eax;"

		"je L0xABEL_0x0053F1A0;"
		"lea ebx,dword ptr [edi+0x4];"
		"lea ecx,dword ptr [ecx];"

		"L0xABEL_0x0053F170:;"
	);
	asm(
		"push esi;"
		"lea eax,dword ptr [esp+0x44];"
		"call 0x47BD40;"
		"mov eax,dword ptr [esp+0x40];"
		"movzx ecx,byte ptr [eax];"
		"mov ecx,dword ptr [ebx+ecx*0x4+0x8];"
		"test ecx,ecx;"
		"je L0xABEL_0x0053F195;"
		"mov edx,dword ptr [ecx];"
		"mov edx,dword ptr [edx];"
		"push ebx;"
		"lea eax,dword ptr [esp+0x44];"
		"push eax;"
		"call edx;"
		"L0xABEL_0x0053F195:;"
		"mov ecx,esi;"
		"call 0x9566C0;"
		"test eax,eax;"

		"jne L0xABEL_0x0053F170;"
		"L0xABEL_0x0053F1A0:;"
	);

	asm(
		"lea eax,dword ptr [esp+0x13];"
		"push eax;"
		"mov eax,0x3;"
		"lea esi,dword ptr [esp+0x44];"
		"mov byte ptr [esp+0x17],0x0;"
		"call 0x47C680;"
		"mov ecx,dword ptr [esp+0x40];"
		"mov byte ptr [ecx+0x1],0x3;"
		"mov edx,dword ptr [esp+0x40];"
		"mov byte ptr [edx+0x2],0x0;"
		"mov eax,dword ptr [esp+0x40];"
		"mov byte ptr [eax],0x0;"
		"mov ecx,dword ptr [esp+0x3C];"
		"mov ecx,dword ptr [ecx];"
		"xor edx,edx;"
		"mov dh,byte ptr [ecx+0x2];"
		"lea eax,dword ptr [ecx+0x3];"
		"mov dl,byte ptr [ecx+0x1];"
		"mov ecx,edx;"
		"cmp ecx,0x3;"
		"jae L0xABEL_0x0053F1EB;"
		"xor ecx,ecx;"
		"jmp L0xABEL_0x0053F1EE;"
		"L0xABEL_0x0053F1EB:;"
		"add ecx,0xFFFFFFFD;"
		"L0xABEL_0x0053F1EE:;"
		"add ecx,eax;"
		"cmp dword ptr [esp+0x30],0x0;"
		"mov dword ptr [esp+0x28],eax;"
		"mov dword ptr [esp+0x24],eax;"
		"mov dword ptr [esp+0x2C],ecx;"
		"je L0xABEL_0x0053F211;"
		"mov dword ptr [esp+0x34],eax;"
		"mov dword ptr [esp+0x30],eax;"
		"mov dword ptr [esp+0x38],ecx;"
		"jmp L0xABEL_0x0053F219;"
		"L0xABEL_0x0053F211:;"
		"mov ecx,dword ptr [esp+0x38];"
		"mov eax,dword ptr [esp+0x34];"
		"L0xABEL_0x0053F219:;"
		"sub ecx,eax;"
		"cmp ecx,0x4;"
		"mov edx,0x1;"
		"mov dword ptr [esp+0x14],edx;"
		"jb L0xABEL_0x0053F232;"
		"mov dword ptr [eax],edx;"
		"add dword ptr [esp+0x34],0x4;"
		"jmp L0xABEL_0x0053F246;"
		"L0xABEL_0x0053F232:;"
		"mov edx,dword ptr [esp+0x20];"
		"push 0x4;"
		"lea eax,dword ptr [esp+0x18];"
		"push eax;"
		"mov eax,dword ptr [edx+0x1C];"
		"lea ecx,dword ptr [esp+0x28];"
		"call eax;"
		"L0xABEL_0x0053F246:;"
		"mov ecx,dword ptr [esp+0x40];"
		"movzx edx,byte ptr [ecx];"
		"mov ecx,dword ptr [edi+edx*0x4+0xC];"
		"test ecx,ecx;"
		"lea eax,dword ptr [edi+0x4];"
		"je L0xABEL_0x0053F264;"
		"mov edx,dword ptr [ecx];"
		"mov edx,dword ptr [edx];"
		"push eax;"
		"lea eax,dword ptr [esp+0x44];"
		"push eax;"
		"call edx;"
		"L0xABEL_0x0053F264:;"
		"mov eax,dword ptr [edi+0x438];"
		"cmp eax,dword ptr [edi+0x43C];"
		"jne L0xABEL_0x0053F27D;"
		"lea ecx,dword ptr [edi+0x184C0];"
		"call 0x9556F0;"
		"L0xABEL_0x0053F27D:;"
		"lea ecx,dword ptr [esp+0x20];"
		"mov byte ptr [esp+0x9C],0x1;"
		"call 0x956DD0;"
		"mov eax,dword ptr [esp+0x40];"
		"cmp eax,dword ptr [esp+0x4C];"
		"je L0xABEL_0x0053F2A1;"
		"push eax;"
		"call 0xA82542;"
		"add esp,0x4;"
		"L0xABEL_0x0053F2A1:;"
		"mov ecx,dword ptr [esp+0x94];"
		"pop edi;"
		"pop esi;"
	//	"mov dword ptr [0x0],ecx;"
		"pop ebx;"
		"mov esp,ebp;"
		"pop ebp;"
		"ret 0x4;"
	);
}
#pragma GCC reset_options