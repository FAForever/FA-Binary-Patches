#define CSTRINGCAST(addr) reinterpret_cast<const char *>(addr)

#define s_ExpectedButGot CSTRINGCAST(0x00E0A220)           // "%s\n  expected %d args, but got %d"
#define s_ExpectedBetweenButGot CSTRINGCAST(0xE0A270)      // "%s\n  expected between %d and %d args, but got %d"
#define s_Global CSTRINGCAST(0x00E00D90)                   // "<global>"
#define s_CMauiBitmap CSTRINGCAST(0xE37438)                // "CMauiBitmap"
#define s_UserUnit CSTRINGCAST(0xE4D090)                   // "UserUnit"
#define s_ExpectedAGameObject CSTRINGCAST(0xE09860)        // "Expected a game object. (Did you call with '.' instead of ':'?)"
#define s_GameObjectHasBeenDestroyed CSTRINGCAST(0xE098A0) // "Game object has been destroyed"
#define s_IncorrectTypeOfGameObject CSTRINGCAST(0xE098C0)  // "Incorrect type of game object.  (Did you call with '.' instead of ':'?)"
#define s_UnknownColor CSTRINGCAST(0x4B2D54)               // "Unknown color: %s"
#define s_c_object CSTRINGCAST(0xE016DC)                   // "_c_object"
