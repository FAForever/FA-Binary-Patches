#pragma GCC optimize("O3")

#include "desync_fix_global.h"

uint32_t tag_sent = 0;
uint32_t p_index = 0;
uint32_t sender_sock = 1;
uint32_t sync_buffer[17];
uint32_t discard = 0;
bool game_ended = false;

__attribute__((noinline)) void p_SetEvent()
{
	LogF("Callback packet received, exit sync is over");
	return;
}

__attribute__((noinline)) void p_Tag()
{
	LogF("Tag packet sent");
	return;
}

__attribute__((noinline)) void CheckClients()
{
	register int eax asm("eax");
	discard = 0;
	for(int i = 0; i<(current_num_clients-1); i++)
	{
		if (sync_buffer[i] == eax)
		{
			LogF("Discarded: %p", eax);
			discard = 1;
			break;
		}
	}
	return;
}

__attribute__((noinline)) void p_rptr()
{
	register int eax asm("eax");
	LogF("recv Ptr: %p", eax);
	return;
}

__attribute__((noinline)) void p_sptr()
{
	register int eax asm("eax");
	LogF("send Ptr: %p", eax);
	return;
}

void _recvfrom()
{
	register int eax asm("eax");
	asm(
		"recvfrom = 0x00C0F8D8;" //WINAPI
		"L0xABEL_0x0048A94C = 4761932;"
		"SetEvent = 0x00C0F578;"
		"push ebp;"
		"mov ebp,esp;"
		"and esp,0xFFFFFFF8;"
		"push 0xFFFFFFFF;"
		"push 0xBB3385;"
		//"mov eax,dword ptr [0x0];"
		"push eax;"
		//"mov byte ptr [0],esp;"
		"sub esp,0xB0;"
		"push ebx;"
		"push esi;"
		"lea eax,dword ptr [esp+0x18];"
		"push edi;"
		"mov edi,ecx;"
		"mov ecx,eax;"
		"mov dword ptr [esp+0x20],eax;"
		"mov dword ptr [esp+0x1C],ecx;"
		"mov dword ptr [esp+0xC4],0x0;"
		"mov ebx,dword ptr [esp+0x20];"
		"lea edx,dword ptr [esp+0x1C];"
		"cmp ebx,edx;"
		"jne L0xABEL_0x0048A338;"
		"lea eax,dword ptr [edi+0x2C];"
		"cmp dword ptr [eax+0x4],eax;"
		"jne L0xABEL_0x0048A2F3;"
		"push 0x218;"
		"call 0xA825B9;"
		"add esp,0x4;"
		"test eax,eax;"
		"je L0xABEL_0x0048A2ED;"
		"mov dword ptr [eax+0x4],eax;"
		"mov dword ptr [eax],eax;"
		"mov ebx,eax;"
		"jmp L0xABEL_0x0048A30E;"
		"L0xABEL_0x0048A2ED:;"
		"xor eax,eax;"
		"mov ebx,eax;"
		"jmp L0xABEL_0x0048A30E;"
		"L0xABEL_0x0048A2F3:;"
		"add dword ptr [edi+0x34],0xFFFFFFFF;"
		"mov ebx,dword ptr [eax+0x4];"
		"mov eax,dword ptr [ebx];"
		"mov ecx,dword ptr [ebx+0x4];"
		"mov dword ptr [eax+0x4],ecx;"
		"mov edx,dword ptr [ebx+0x4];"
		"mov eax,dword ptr [ebx];"
		"mov dword ptr [edx],eax;"
		"mov dword ptr [ebx+0x4],ebx;"
		"mov dword ptr [ebx],ebx;"
		"L0xABEL_0x0048A30E:;"
		"mov ecx,dword ptr [ebx];"
		"mov edx,dword ptr [ebx+0x4];"
		"mov dword ptr [ecx+0x4],edx;"
		"mov eax,dword ptr [ebx+0x4];"
		"mov ecx,dword ptr [ebx];"
		"mov dword ptr [eax],ecx;"
		"mov dword ptr [ebx],ebx;"
		"mov dword ptr [ebx+0x4],ebx;"
		"mov edx,dword ptr [esp+0x1C];"
		"mov dword ptr [ebx],edx;"
		"lea eax,dword ptr [esp+0x1C];"
		"mov dword ptr [ebx+0x4],eax;"
		"mov dword ptr [esp+0x1C],ebx;"
		"mov ecx,dword ptr [ebx];"
		"mov dword ptr [ecx+0x4],ebx;"
		"L0xABEL_0x0048A338:;"
		"lea edx,dword ptr [esp+0x4C];"
		"push edx;" // (fromlen) An optional pointer to the size, in bytes, of the buffer pointed to by the from parameter.
		"mov edx,dword ptr [edi+0x14];"
		"lea ecx,dword ptr [esp+0xAC];"
		"push ecx;" // (from) An optional pointer to a buffer in a sockaddr structure that will hold the source address upon return.
		"push 0x0;" // (flags) A set of flags that specify the way in which the call is made.
		"lea eax,dword ptr [ebx+0x18];"
		"push 0x200;" //(len) The length, in bytes, of the data pointed to by the buf parameter.
		"push eax;" //(buf) A buffer for the incoming data.
		"push edx;" // (s)A descriptor identifying a bound socket.
		"mov dword ptr [esp+0x64],0x10;"
		"mov dword ptr [esp+0x3C],eax;"
		"mov dword ptr [0x11FD247], eax;" //STORE NETWORK BUFFER POINTER (SEE REASON BELOW)
		"call dword ptr [recvfrom];" //recvfrom RETURNS NUMBER OF BYTES WRITTEN TO THE NET BUFFER IN EAX REGISTER
		"mov esi,eax;"
		"mov eax, dword ptr [0x11FD247];" //RETREIVE NETBUFFER PTR
	);
	asm(
		"cmp dword ptr [0x011FD243], 0x1;" // check if we are the sender
		"jne not_sender;"
		"cmp byte ptr [eax+0x3], 0xFE;"
		"jne not_received;"
		"cmp byte ptr [eax+0x2], 0x18;"
		"jne not_received;"
		"mov byte ptr [eax+0x3], 0x0;" //JUST IN CASE CLEAR THE VARIABLE FROM NET BUFFER
		"mov byte ptr [eax+0x2], 0x0;" //JUST IN CASE CLEAR THE VARIABLE FROM NET BUFFER
		"mov eax,dword ptr [esp+0xA8];" //get sender's socket
	);

	asm(
		"call %[func];"
		:
		: [func] "i" (&CheckClients)
		: "memory"
	);
	if(discard)
	{
		asm("jmp _skip;");
	}
	asm(
		"call %[func];"
		"mov eax,dword ptr [esp+0xA8];" //get sender's socket
		:
		: [func] "i" (&p_rptr)
		: "memory"
	);
	p_index++;
	sync_buffer[p_index] = eax;

	if((current_num_clients-1) - p_index == 0)
	{
		asm(
		"push dword ptr [0x11FD253];"
		"call dword ptr [SetEvent];"
		);
		p_index = 0;
		asm(
			"call %[func];"
			:
			: [func] "i" (&p_SetEvent)
			: "memory"
		);
	}
	asm(
		"jmp not_received;"
		"not_sender:;"
		"cmp byte ptr [eax+0x3], 0xA4;" //CHECK IF EXIT VARIABLE IS SET
		"jne _skip;"
		"cmp byte ptr [eax+0x2], 0x4E;" //CHECK IF EXIT VARIABLE IS SET
		"jne _skip;"
		"mov byte ptr [eax+0x3], 0x0;" //JUST IN CASE CLEAR THE VARIABLE FROM NET BUFFER
		"mov byte ptr [eax+0x2], 0x0;" //JUST IN CASE CLEAR THE VARIABLE FROM NET BUFFER
		"mov dword ptr [0x011FD23F], 0x1;" //WRITE INTO THE LOCAL MEMORY SO THAT BLOCK CAN READ IT LATER
		"mov eax,dword ptr [esp+0xA8];" //get sender's socket
	);
	if(sender_sock == 1)
	{
		sender_sock = eax;
	}
	asm(
		"_skip:;"
		"not_received:;"

	);

	asm(

		"test esi,esi;"
		"mov dword ptr [esp+0x10],esi;"
		"jl L0xABEL_0x0048A94C;" // function continues...
		"jmp 0x0048A374;" //THERE ARE A LOT MORE CODE FURTHER BUT I AM LAZY TO WRITE IT BECAUSE THERE IS SWITCH CASE
		//WHICH REQUIRES MANUAL CODING.
	);
}

void _sendto()
{
	register int eax asm("eax");
	asm(
		"htons = 0x00C0F8D0;"
		"htonl = 0x00C0F8CC;"
		"sendto = 0x00C0F8E0;"
		"push 0xFFFFFFFF;"
		"push 0xB9B638;"
		//"mov eax,dword ptr [0x0];"
		"push eax;"
		//"mov dword ptr [0x0],esp;"
		"sub esp,0x7C;"
		"push ebx;"
		"push ebp;"
		"mov ebp,dword ptr [esp+0x94];"
		"mov ax,word ptr [ebp+0x4A0];"
		"push esi;"
		"mov ebx,ecx;"
		"push edi;"
		"lea edi,dword ptr [ebx+0x18];"
		"mov word ptr [edi+0x5],ax;"
		"mov cx,word ptr [ebp+0x4A2];"
		"mov word ptr [edi+0x7],cx;"
		"movzx edx,word ptr [ebp+0x420];"
		"push edx;"
		"mov dword ptr [esp+0x18],edi;"
		"mov word ptr [esp+0x28],0x2;"

		"call dword ptr [htons];"

		"mov word ptr [esp+0x26],ax;"
		"mov eax,dword ptr [ebp+0x41C];"
		"push eax;"

		"call dword ptr [htonl];"

		"mov edx,dword ptr [ebp+0x418];"
		"mov ecx,dword ptr [ebx+0x14];"
		"push 0x10;" // (tolen) The size, in bytes, of the address pointed to by the to parameter.
		"mov dword ptr [esp+0x2C],eax;"
		"mov eax,dword ptr [edx+0x14];"
		"lea edx,dword ptr [esp+0x28];"
		"push edx;" // (to) An optional pointer to a sockaddr structure that contains the address of the target socket.
		"push 0x0;" // (flags) A set of flags that specify the way in which the call is made.
		"push ecx;" //(len) The length, in bytes, of the data pointed to by the buf parameter.
		"push edi;" //(buf) A pointer to a buffer containing the data to be transmitted.
		"push eax;" // (s) A descriptor identifying a (possibly connected) socket.
		"mov dword ptr [esp+0x28],ecx;"
		"cmp dword ptr [0x011FD243], 0x1;" //CHECK IF SESSIONENDGAME WAS TRIGGERED (sender only)
		"jne is_receiver;"
		"mov eax, edi;" //if here, it is sender
		"cmp byte ptr [eax + 0x3], 0;"  //overwrite only known memory, ie 0
		"jne skip;"
		"cmp byte ptr [eax + 0x2], 0;"
		"jne skip;"
		"mov byte ptr [eax + 0x3], 0xA4;" //WRITE VAR INTO THE PACKET HEADER
		"mov byte ptr [eax + 0x2], 0x4E;" //WRITE VAR INTO THE PACKET HEADER
		"mov eax, dword ptr [edx];"
	);
	asm(
		"call %[func];"
		:
		: [func] "i" (&p_sptr)
		: "memory"
	);
	asm(

		"jmp skip;"

		"is_receiver:;"

		"cmp dword ptr [0x011FD23F], 0x1;" //check if receiver got packet (block var)
		"jne skip;"
		"mov eax, dword ptr [edx];"
	);
	if(!terminated)
	{
		if(sender_sock == eax)
		{
			asm(
				//"mov dword ptr [0x011FD23F+0x30],eax;"
				"call %[func];"
				:
				: [func] "i" (&p_sptr)
				: "memory"
			);
			asm(
			"mov eax, edi;"
			"cmp byte ptr [eax + 0x3], 0;"     //overwrite only known memory, ie 0
			"jne skip;"
			"cmp byte ptr [eax + 0x2], 0;"
			"jne skip;"
			"mov byte ptr [eax + 0x3], 0xFE;" //if receiver got the packet, notify the sender with 0xFE
			"mov byte ptr [eax + 0x2], 0x18;"
			);
			//sender_sock = 0;
		}
	}
	asm(
		"skip:;"
		"call dword ptr [sendto];"

		"mov esi,dword ptr [ebp+0x418];"
		"call 0x489F30;"
		"cmp byte ptr [0x10A6381],0x0;"
		"mov ecx,dword ptr [ebp+0x41C];"
		"mov esi,eax;"
		"movzx eax,word ptr [ebp+0x420];"
		"mov dword ptr [esp+0x1C],esi;"
		"mov dword ptr [esp+0x20],edx;"
		"je L0xABEL_0x00488E59;"
		"mov edx,dword ptr [esp+0x10];"
		"push edx;"
		"push edi;"
		"mov edi,dword ptr [ebp+0x418];"
		"push eax;"
		"mov eax,dword ptr [esp+0x2C];"
		"push ecx;"
		"push eax;"
		"push esi;"
		"xor ecx,ecx;"
		"call 0x48B040;"
		"mov edi,dword ptr [esp+0x14];"
		"L0xABEL_0x00488E59:;"
		"cmp byte ptr [edi],0x4;"
		"jne L0xABEL_0x00488EC4;"
		"mov ecx,ebx;"
		"mov eax,ebp;"
		"call 0x488170;"
		"add eax,esi;"
		"adc edx,dword ptr [esp+0x20];"
		"add dword ptr [ebx+0x10],0x1;"
		"mov dword ptr [ebx+0x8],eax;"
		"mov dword ptr [ebx+0xC],edx;"
		"mov eax,dword ptr [ebp+0x514];"
		"lea ecx,dword ptr [ebp+0x510];"
		"cmp eax,ecx;"
		"je L0xABEL_0x00488EA0;"
		"L0xABEL_0x00488E87:;"
		"mov edx,dword ptr [ebx+0xC];"
		"cmp edx,dword ptr [eax+0xC];"
		"jl L0xABEL_0x00488EA0;"
		"jg L0xABEL_0x00488E99;"
		"mov edx,dword ptr [ebx+0x8];"
		"cmp edx,dword ptr [eax+0x8];"
		"jb L0xABEL_0x00488EA0;"
		"L0xABEL_0x00488E99:;"
		"mov eax,dword ptr [eax+0x4];"
		"cmp eax,ecx;"
		"jne L0xABEL_0x00488E87;"
		"L0xABEL_0x00488EA0:;"
		"mov ecx,dword ptr [ebx];"
		"mov edx,dword ptr [ebx+0x4];"
		"mov dword ptr [ecx+0x4],edx;"
		"mov ecx,dword ptr [ebx+0x4];"
		"mov edx,dword ptr [ebx];"
		"mov dword ptr [ecx],edx;"
		"mov dword ptr [ebx],ebx;"
		"mov dword ptr [ebx+0x4],ebx;"
		"mov ecx,dword ptr [eax];"
		"mov dword ptr [ebx],ecx;"
		"mov dword ptr [ebx+0x4],eax;"
		"mov dword ptr [eax],ebx;"
		"mov edx,dword ptr [ebx];"
		"mov dword ptr [edx+0x4],ebx;"
		"jmp L0xABEL_0x00488ED1;"
		"L0xABEL_0x00488EC4:;"
		"mov edx,dword ptr [ebp+0x418];"
		"mov eax,ebx;"
		"call 0x489ED0;"
		"L0xABEL_0x00488ED1:;"
		"mov ecx,dword ptr [esp+0x10];"
		"mov edx,dword ptr [esp+0x20];"
		"mov eax,dword ptr [ebp+0x418];"
		"push ecx;"
		"push edx;"
		"push esi;"
		"push 0x0;"
		"lea edx,dword ptr [eax+0x80];"
		"call 0x47D0A0;"
		"cmp dword ptr [0x10A6384],0x2;"
		"jl L0xABEL_0x0048901B;"
		"lea eax,dword ptr [esp+0x6C];"
		"push eax;"
		"mov esi,ebx;"
		"call 0x488BC0;"
		"add esp,0x4;"
		"mov ebx,eax;"
		"mov dword ptr [esp+0x94],0x0;"
		"mov edx,dword ptr [ebp];"
		"mov edx,dword ptr [edx+0x18];"
		"lea eax,dword ptr [esp+0x50];"
		"push eax;"
		"mov ecx,ebp;"
		"call edx;"
		"mov dword ptr [esp+0x14],eax;"
		"mov eax,dword ptr [esp+0x20];"
		"mov ecx,dword ptr [esp+0x1C];"
		"push eax;"
		"push ecx;"
		"lea esi,dword ptr [esp+0x3C];"
		"mov byte ptr [esp+0x9C],0x1;"
		"call 0x485CB0;"
		"add esp,0x8;"
		"mov esi,0x10;"
		"mov byte ptr [esp+0x94],0x2;"
		"cmp dword ptr [ebx+0x18],esi;"
		"jb L0xABEL_0x00488F60;"
		"mov ebx,dword ptr [ebx+0x4];"
		"jmp L0xABEL_0x00488F63;"
		"L0xABEL_0x00488F60:;"
		"add ebx,0x4;"
		"L0xABEL_0x00488F63:;"
		"mov ecx,dword ptr [esp+0x14];"
		"cmp dword ptr [ecx+0x18],esi;"
		"jb L0xABEL_0x00488F71;"
		"mov ecx,dword ptr [ecx+0x4];"
		"jmp L0xABEL_0x00488F74;"
		"L0xABEL_0x00488F71:;"
		"add ecx,0x4;"
		"L0xABEL_0x00488F74:;"
		"cmp dword ptr [eax+0x18],esi;"
		"jb L0xABEL_0x00488F7E;"
		"mov eax,dword ptr [eax+0x4];"
		"jmp L0xABEL_0x00488F81;"
		"L0xABEL_0x00488F7E:;"
		"add eax,0x4;"
		"L0xABEL_0x00488F81:;"
		"push 0xE00779;"
		"push ebx;"
		"push ecx;"
		"push eax;"
		"push 0xE05AB0;"
		"call 0x937C30;"
		"add esp,0x14;"
		"cmp dword ptr [esp+0x4C],esi;"
		"jb L0xABEL_0x00488FA9;"
		"mov edx,dword ptr [esp+0x38];"
		"push edx;"
		"call 0x957A60;"
		"add esp,0x4;"
		"L0xABEL_0x00488FA9:;"
		"xor ebx,ebx;"
		"cmp dword ptr [esp+0x68],esi;"
		"mov dword ptr [esp+0x4C],0xF;"
		"mov dword ptr [esp+0x48],ebx;"
		"mov byte ptr [esp+0x38],bl;"
		"jb L0xABEL_0x00488FCE;"
		"mov eax,dword ptr [esp+0x54];"
		"push eax;"
		"call 0x957A60;"
		"add esp,0x4;"
		"L0xABEL_0x00488FCE:;"
		"mov dword ptr [esp+0x94],0xFFFFFFFF;"
		"cmp dword ptr [esp+0x84],esi;"
		"mov dword ptr [esp+0x68],0xF;"
		"mov dword ptr [esp+0x64],ebx;"
		"mov byte ptr [esp+0x54],0x0;"
		"jb L0xABEL_0x00489000;"
		"mov ecx,dword ptr [esp+0x70];"
		"push ecx;"
		"call 0x957A60;"
		"add esp,0x4;"
		"L0xABEL_0x00489000:;"
		"mov esi,dword ptr [esp+0x1C];"
		"mov dword ptr [esp+0x84],0xF;"
		"mov dword ptr [esp+0x80],ebx;"
		"mov byte ptr [esp+0x70],0x0;"
		"L0xABEL_0x0048901B:;"
		"add word ptr [ebp+0x4A0],0x1;"
		"xor ebx,ebx;"
		"lea ecx,dword ptr [ebp+0x4B8];"
		"mov word ptr [ebp+0x4A2],bx;"
		"call 0x9566C0;"
		"test eax,eax;"
		"je L0xABEL_0x00489077;"
		"mov eax,dword ptr [0xF58DE4];"
		"push ebx;"
		"cdq;"
		"push 0x3E8;"
		"push edx;"
		"push eax;"
		"call 0xA899A0;"
		"mov esi,dword ptr [ebp+0x418];"
		"mov ebx,eax;"
		"mov dword ptr [esp+0x18],edx;"
		"call 0x489F30;"
		"mov esi,dword ptr [esp+0x1C];"
		"add eax,ebx;"
		"adc edx,dword ptr [esp+0x18];"
		"mov dword ptr [ebp+0x4A8],eax;"
		"mov dword ptr [ebp+0x4AC],edx;"
		"jmp L0xABEL_0x00489083;"
		"L0xABEL_0x00489077:;"
		"mov dword ptr [ebp+0x4A8],ebx;"
		"mov dword ptr [ebp+0x4AC],ebx;"
		"L0xABEL_0x00489083:;"
		"movzx eax,word ptr [edi+0x5];"
		"movzx edx,ax;"
		"and edx,0x8000007F;"
		"jns L0xABEL_0x00489097;"
		"dec edx;"
		"or edx,0xFFFFFF80;"
		"inc edx;"
		"L0xABEL_0x00489097:;"
		"shl edx,0x4;"
		"mov word ptr [edx+ebp+0x518],ax;"
		"movzx ecx,word ptr [edi+0x5];"
		"and ecx,0x8000007F;"
		"jns L0xABEL_0x004890B3;"
		"dec ecx;"
		"or ecx,0xFFFFFF80;"
		"inc ecx;"
		"L0xABEL_0x004890B3:;"
		"add ecx,0x52;"
		"shl ecx,0x4;"
		"add ecx,ebp;"
		"call 0x9556F0;"
		"mov eax,dword ptr [esp+0x20];"
		"push eax;"
		"push esi;"
		"mov esi,ebp;"
		"call 0x488260;"
		"add eax,dword ptr [esp+0x10];"
		"mov ecx,dword ptr [esp+0x20];"
		"mov dword ptr [ebp+0x438],eax;"
		"mov eax,dword ptr [esp+0x1C];"
		"mov dword ptr [ebp+0x430],eax;"
		"mov dword ptr [ebp+0x434],ecx;"
		"cmp byte ptr [edi],0x5;"
		"je L0xABEL_0x004890FC;"
		"mov dword ptr [ebp+0x448],eax;"
		"mov dword ptr [ebp+0x44C],ecx;"
		"L0xABEL_0x004890FC:;"
		"cmp byte ptr [edi],0x6;"
		"jne L0xABEL_0x0048910B;"
		"mov dword ptr [ebp+0x450],0x1E8480;"
		"L0xABEL_0x0048910B:;"
		"mov ecx,dword ptr [esp+0x8C];"
		"pop edi;"
		"pop esi;"
		"pop ebp;"
		//"mov dword ptr [0x0],ecx;"
		"pop ebx;"
		"add esp,0x88;"
		"ret 0x4;"
	);
}

void Gpg_Net_Entry()
{
	asm(
		"GetCurrentThread = 0x00C0F588;"
		"SetThreadPriority = 0x00C0F52C;"
		"WSAWaitForMultipleEvents = 0x00C0F930;"
		"WSAResetEvent = 0x00C0F8FC;"
		"CreateEvent = 0x00C0F57C;"

		"push 0xFFFFFFFF;"
		"push 0xBB33B8;"
	//	"mov eax,dword ptr [0x0];"
		"push eax;"
	//	"mov dword ptr [0x0],esp;"
		"sub esp,0x58;"
		"push ebx;"
		"push ebp;"
		"push esi;"
		"push edi;"
		"mov edi,ecx;"
		"mov dword ptr [esp+0x14],edi;"
		"mov dword ptr [esp+0x70],0x0;"
		"mov eax,dword ptr [edi];"
		"mov edx,dword ptr [eax+0xC];"
		"call edx;"
		"movzx eax,ax;"
		"push eax;"
		"lea ecx,dword ptr [esp+0x34];"
		"push 0xE05B04;"
		"push ecx;"
		"call 0x938F10;"
		"add esp,0xC;"
		"mov byte ptr [esp+0x70],0x1;"
		"cmp dword ptr [esp+0x48],0x10;"
		"mov eax,dword ptr [esp+0x34];"
		"jae L0xABEL_0x00489FEB;"
		"lea eax,dword ptr [esp+0x34];"
		"L0xABEL_0x00489FEB:;"
		"push eax;"
		"push 0xFFFFFFFF;"
		"call 0x9071D0;"
		"add esp,0x8;"
		"xor bl,bl;"
		"call 0x4141A0;"
		"push 0x2;"
		"call dword ptr [GetCurrentThread];"
		"push eax;"
		"call dword ptr [SetThreadPriority];"

		"xor ecx, ecx;"
		"push ecx;"
		"push ecx;"
		"push ecx;"
		"push ecx;"
		"call dword ptr [CreateEvent];"
		"mov dword ptr [0x11FD253], eax;"

		"lea ecx,dword ptr [edi+0x8];"
		"mov dword ptr [esp+0x1C],ecx;"
		"call 0xAC51B0;"
		"mov byte ptr [esp+0x20],0x1;"
		"mov byte ptr [esp+0x70],0x2;"
		"L0xABEL_0x0048A022:;"
		"cmp byte ptr [0x10A6381],0x0;"
		"jne L0xABEL_0x0048A048;"
		"mov eax,dword ptr [edi+0x18088];"
		"test eax,eax;"
		"je L0xABEL_0x0048A048;"
		"push eax;"
		"call 0xA8540F;"
		"add esp,0x4;"
		"mov dword ptr [edi+0x18088],0x0;"
		"L0xABEL_0x0048A048:;"
		"mov ecx,edi;"
		"call 0x48A280;"
		"mov ecx,edi;"
		"call 0x48AC40;"
		"cmp byte ptr [edi+0x50],0x0;"
		"mov ebx,eax;"
		"mov dword ptr [esp+0x18],ebx;"
		"je L0xABEL_0x0048A0AC;"
		"cmp byte ptr [edi+0x51],0x0;"
		"jne L0xABEL_0x0048A0AC;"
		"mov esi,dword ptr [edi+0x28];"
		"lea ebx,dword ptr [edi+0x24];"
		"cmp esi,ebx;"
		"je L0xABEL_0x0048A09F;"
		"L0xABEL_0x0048A072:;"
		"test esi,esi;"
		"je L0xABEL_0x0048A07E;"
		"lea ebp,dword ptr [esi-0x410];"
		"jmp L0xABEL_0x0048A080;"
		"L0xABEL_0x0048A07E:;"
		"xor ebp,ebp;"
		"L0xABEL_0x0048A080:;"
		"cmp byte ptr [ebp+0xE41],0x0;"
		"mov esi,dword ptr [esi+0x4];"
		"je L0xABEL_0x0048A09B;"
		"push ebp;"
		"call 0x486150;"
		"push ebp;"
		"call 0x957A60;"
		"add esp,0x4;"
		"L0xABEL_0x0048A09B:;"
		"cmp esi,ebx;"
		"jne L0xABEL_0x0048A072;"
		"L0xABEL_0x0048A09F:;"
		"cmp dword ptr [ebx+0x4],ebx;"
		"je L0xABEL_0x0048A1D5;"
		"mov ebx,dword ptr [esp+0x18];"
		"L0xABEL_0x0048A0AC:;"
		"mov ecx,dword ptr [esp+0x1C];"
		"call 0xAC5200;"

		"cmp byte ptr [0x10A6380],0x0;"
		"mov byte ptr [esp+0x20],0x0;"
		"je L0xABEL_0x0048A0CA;"
		"mov dword ptr [0x0], eax;"
		"L0xABEL_0x0048A0CA:;"

		"xor ebp,ebp;"
		"cmp dword ptr [0x10A6384],0x3;"
		"mov dword ptr [esp+0x28],ebp;"
		"jl L0xABEL_0x0048A132;"
		"mov esi,edi;"
		"call 0x489F30;"
		"mov ebp,eax;"
		"push edx;"
		"push ebp;"
		"lea esi,dword ptr [esp+0x54];"
		"mov dword ptr [esp+0x30],edx;"
		"call 0x485CB0;"
		"add esp,0x8;"
		"mov esi,0x10;"
		"mov byte ptr [esp+0x70],0x3;"
		"cmp dword ptr [eax+0x18],esi;"
		"jb L0xABEL_0x0048A108;"
		"mov eax,dword ptr [eax+0x4];"
		"jmp L0xABEL_0x0048A10B;"
		"L0xABEL_0x0048A108:;"
		"add eax,0x4;"
		"L0xABEL_0x0048A10B:;"
		"push ebx;"
		"push eax;"
		"push 0xE05B24;"
		"call 0x937C30;"
		"add esp,0xC;"
		"mov byte ptr [esp+0x70],0x2;"
		"cmp dword ptr [esp+0x64],esi;"
		"jb L0xABEL_0x0048A132;"
		"mov edx,dword ptr [esp+0x50];"
		"push edx;"
		"call 0x957A60;"
		"add esp,0x4;"
		"L0xABEL_0x0048A132:;"
		"push 0x1;"
		"push ebx;"
		"push 0x0;"
		"lea eax,dword ptr [edi+0x18];"
		"push eax;"
		"push 0x1;"
		"call dword ptr [WSAWaitForMultipleEvents];"
		"mov eax,ebp;"
		"or eax,dword ptr [esp+0x28];"
		"je L0xABEL_0x0048A1B8;"
		"mov esi,edi;"
		"call 0x489F30;"
		"mov ebx,edx;"
		"mov edi,eax;"
		"push ebx;"
		"push edi;"
		"lea esi,dword ptr [esp+0x54];"
		"call 0x485CB0;"
		"add esp,0x8;"
		"mov byte ptr [esp+0x70],0x4;"
		"cmp dword ptr [eax+0x18],0x10;"
		"jb L0xABEL_0x0048A174;"
		"mov esi,dword ptr [eax+0x4];"
		"jmp L0xABEL_0x0048A177;"
		"L0xABEL_0x0048A174:;"
		"lea esi,dword ptr [eax+0x4];"
		"L0xABEL_0x0048A177:;"
		"push 0x0;"
		"sub edi,ebp;"
		"sbb ebx,dword ptr [esp+0x2C];"
		"push 0x3E8;"
		"push ebx;"
		"push edi;"
		"call 0xA8EB30;"
		"push edx;"
		"push eax;"
		"push esi;"
		"push 0xE05B40;"
		"call 0x937C30;"
		"add esp,0x10;"
		"mov byte ptr [esp+0x70],0x2;"
		"cmp dword ptr [esp+0x64],0x10;"
		"jb L0xABEL_0x0048A1B4;"
		"mov ecx,dword ptr [esp+0x50];"
		"push ecx;"
		"call 0x957A60;"
		"add esp,0x4;"
		"L0xABEL_0x0048A1B4:;"
		"mov edi,dword ptr [esp+0x14];"
		"L0xABEL_0x0048A1B8:;"
		"mov ecx,dword ptr [esp+0x1C];"
		"call 0xAC51B0;"
		"mov edx,dword ptr [edi+0x18];"
		"push edx;"
		"mov byte ptr [esp+0x24],0x1;"
		"call dword ptr [WSAResetEvent];"
		"jmp L0xABEL_0x0048A022;"
		"L0xABEL_0x0048A1D5:;"
		"mov ecx,dword ptr [esp+0x1C];"
		"call 0xAC5200;"
		"cmp dword ptr [esp+0x48],0x10;"
		"jb L0xABEL_0x0048A1F2;"
		"mov eax,dword ptr [esp+0x34];"
		"push eax;"
		"call 0x957A60;"
		"add esp,0x4;"
		"L0xABEL_0x0048A1F2:;"
		"mov dword ptr [esp+0x70],0xFFFFFFFF;"
		"mov esi,dword ptr [esp+0x7C];"
		"test esi,esi;"
		"mov dword ptr [esp+0x48],0xF;"
		"mov dword ptr [esp+0x44],0x0;"
		"mov byte ptr [esp+0x34],0x0;"
		"je L0xABEL_0x0048A241;"
		"lea ecx,dword ptr [esi+0x4];"
		"or edx,0xFFFFFFFF;"
		"lock xadd dword ptr [ecx],edx;"
		"jne L0xABEL_0x0048A241;"
		"mov eax,dword ptr [esi];"
		"mov edx,dword ptr [eax+0x4];"
		"mov ecx,esi;"
		"call edx;"
		"lea eax,dword ptr [esi+0x8];"
		"or ecx,0xFFFFFFFF;"
		"lock xadd dword ptr [eax],ecx;"
		"jne L0xABEL_0x0048A241;"
		"mov edx,dword ptr [esi];"
		"mov eax,dword ptr [edx+0x8];"
		"mov ecx,esi;"
		"call eax;"
		"L0xABEL_0x0048A241:;"
		"mov ecx,dword ptr [esp+0x68];"
		"pop edi;"
		"pop esi;"
		"pop ebp;"
		//"mov dword ptr [0x0],ecx;"
		"pop ebx;"
		"add esp,0x64;"
		"ret 0x8;"
	);
}

void EndGame()
{
	game_ended = true;
	asm("mov dword ptr [0x011FD23F], 0xB;"); //remove block when last player remains.
	asm(
		"push esi;"
		"mov esi,eax;"
		"mov eax,dword ptr [esi];"
		"push edi;"
		"mov edi,dword ptr [0x10BA600];"
		"push eax;"
		"call 0x90C590;"
		"add esp,0x4;"
		"test eax,eax;"
		"je L0xABEL_0x0074B8DB;"
		"push eax;"
		"push 0x0;"
		"push edi;"
		"push 0xE0A220;"
		"push esi;"
		"call 0x90C1D0;"
		"add esp,0x14;"
		"L0xABEL_0x0074B8DB:;"
		"mov esi,dword ptr [esi];"
		"push esi;"
		"call 0x924050;"
		"mov edx,dword ptr [eax];"
		"mov ecx,eax;"
		"mov eax,dword ptr [edx+0x5C];"
		"add esp,0x4;"
		"call eax;"
		"pop edi;"
		"xor eax,eax;"
		"pop esi;"
		"ret;"
	);
}
#pragma GCC reset_options