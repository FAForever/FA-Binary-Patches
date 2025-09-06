#pragma once

#define SHARED extern "C"

#define GPtr(addr, type) \
  (*(type*)addr)

#define CSTR(name, addr) \
extern const char name[] asm(#addr);


#define GDecl(name, addr, type) \
  extern type name asm(#addr);

#define WDecl(addr, type) \
  ((type)*(uintptr_t*)addr)

#define VALIDATE_SIZE(struc, size) \
  static_assert(sizeof(struc) == size, "Invalid structure size of " #struc);