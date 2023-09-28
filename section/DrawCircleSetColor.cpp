unsigned int CIRCLE_COLOR = 0xFF00D800;
float SEGMENT_RAD = 0.1308996939f;

void _SetColor()
{
    asm(
        "mov eax, %[coloraddr];"
        "mov eax, [eax];"
        "jmp 0x4561AD;"
        :
        :[coloraddr]"i"(&CIRCLE_COLOR)
        :
    );

}