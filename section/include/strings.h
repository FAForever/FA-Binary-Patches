#define CSTRINGCAST(addr) reinterpret_cast<const char*>(addr) 

#define s_ExpectedButGot CSTRINGCAST(0x00E0A220) // "%s\n  expected %d args, but got %d"
#define s_ExpectedBetweenButGot CSTRINGCAST(0xE0A270) //"%s\n  expected between %d and %d args, but got %d"
#define s_Global CSTRINGCAST(0x00E00D90) // "<global>"
#define s_CMauiBitmap CSTRINGCAST(0xE37438) // "CMauiBitmap"
#define s_UserUnit CSTRINGCAST(0xE4D090) // "UserUnit"