#include "global.h"
#include "magic_classes.h"

static bool map_has_unit(uintptr_t map_base, uintptr_t key) {
  auto head = *reinterpret_cast<uintptr_t*>(map_base + 0x04);
  auto node = *reinterpret_cast<uintptr_t*>(head + 0x04);
  while (node) {
    if (*reinterpret_cast<uint8_t*>(node + 0x19))  // _Isnil (0x19)
      return false;
    auto node_key = *reinterpret_cast<uintptr_t*>(node + 0x0C);
    if (key < node_key)
      node = *reinterpret_cast<uintptr_t*>(node + 0x00);  // _Left (0x00)
    else if (key > node_key)
      node = *reinterpret_cast<uintptr_t*>(node + 0x08);  // _Right (0x08)
    else
      return true;
  }
  return false;
}

static bool isOwnUserUnit(uintptr_t userUnit) {
  auto session = *reinterpret_cast<uintptr_t*>(0x10A6470);
  auto focusArmyIndex = *reinterpret_cast<int*>(session + 0x488);
  auto unitArmy = *reinterpret_cast<uintptr_t*>(userUnit + 0x120);
  auto unitArmyIndex = *reinterpret_cast<int*>(unitArmy);
  return (unitArmyIndex == focusArmyIndex);
}

enum IntelRangeBehavior {
  kOwnUnits = 0,
  kOwnUnitsAndAlliedBuildings = 1,
  kOwnUnitsAndAlliedUnits = 2
};

int intelRangeBehavior = kOwnUnitsAndAlliedBuildings;
ConDescReg conIntelRangeBehavior{"ren_IntelRangeBehavior",
                                 "Only own units = 0; Own units and allied "
                                 "buildings = 1; Own and allied units = 2",
                                 &intelRangeBehavior};

extern "C" {

uintptr_t __thiscall RenderRange__Moho__UserUnit__UserUnit(uintptr_t this_,
                                                           uintptr_t esp) {
  if (isOwnUserUnit(this_)) return this_;

  // Compiler, I believe in you. We're in a hot path, optimize and inline the
  // lambda pls p.s. I saw the listing, mine handled it, and there’s no call
  // there
  auto equals = []<size_t N>(string& str1, const char (&str2)[N]) {
    if (str1.strLen != N - 1) return false;
    const char* l = str1.data();
    const char* r = str2;
    while (*l == *r && *l) {
      ++l;
      ++r;
    };
    return (*l - *r) == 0;
  };
  auto& rangeName = **reinterpret_cast<string**>(esp + 0x30);
  bool isIntelRange = equals(rangeName, "Radar") || equals(rangeName, "Omni") ||
                      equals(rangeName, "Sonar");
  if (isIntelRange) return this_;
  return 0;
}

extern void __fastcall Hooked_SyncVisionRange(uintptr_t*, uintptr_t*) asm("Hooked_SyncVisionRange");
void Hooked_SyncVisionRange(uintptr_t* this_, uintptr_t* edx) {
  edx[0] = this_[0]; // Vision
  edx[2] = this_[2]; // Radar
  edx[3] = this_[3]; // Sonar
  edx[4] = this_[4]; // Omni
}

bool __cdecl ShouldAddUnit(void* focusArmy, uintptr_t userUnit) {
  auto session = *reinterpret_cast<uintptr_t*>(0x10A6470);
  auto unitArmy = *reinterpret_cast<void**>(userUnit + 0x120);
  if (unitArmy == focusArmy) {
    // Optimization: To avoid double rendering,
    // do not render the global rings;
    // they will be rendered in the selection rings render
    auto selectionSize = *(size_t*)(session + 0x4A8);
    [[likely]] if (selectionSize < 67)
      return true;
    return !map_has_unit(session + 0x4A0, userUnit);
  }

  if (intelRangeBehavior == kOwnUnits) return false;

  auto focusArmyIndex = *reinterpret_cast<int*>(session + 0x488);
  if (!Army_IsAlly(focusArmyIndex, unitArmy)) return false;

  auto blueprint = *reinterpret_cast<uintptr_t*>(userUnit + 0x48);
  if (intelRangeBehavior == kOwnUnitsAndAlliedBuildings &&
      *reinterpret_cast<uint32_t*>(blueprint + 0x290) != 0)
    // MotionType != None -> let's assume this is not a building
    return false;

  auto intel = reinterpret_cast<uint32_t*>(blueprint + 0x338);
  bool hasIntel = (intel[0] | intel[1] | intel[2]) != 0;
  return hasIntel;
}

}  // extern "C"
