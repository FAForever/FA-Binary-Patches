#include "global.h"
#include "magic_classes.h"
#include "moho.h"

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

// This hook disables rendering of ally range rings, except for RADAR, SONAR,
// and OMNI. It also includes an optimization to prevent double rendering.
UserUnit *__thiscall RenderRange__Moho__UserUnit__UserUnit(UserUnit *self,
                                                           uintptr_t esp) {
  auto session = g_CWldSession;

  // Optimization: To avoid double rendering, skip rendering global rings for
  // own-army units; these will be handled by the selection rings renderer.
  if (self->mArmy->mConstDat.mIndex == session->focusArmyIndex) {
    if (session->selectedUnits.contains(self))
      return nullptr;
    else
      return self;
  }

  // Disables rendering of ally range rings, except for RADAR, SONAR, and OMNI.
  // Btw, compiler, I believe in you. We're in a hot path, so please optimize
  // and inline the lambda. P.S. I saw the listing — mine handled it, and
  // there’s no call there.
  auto equals = []<size_t N>(string &str1, const char (&str2)[N]) {
    if (str1.strLen != N - 1)
      return false;
    const char *l = str1.data();
    const char *r = str2;
    while (*l == *r && *l) {
      ++l;
      ++r;
    };
    return (*l - *r) == 0;
  };
  auto &rangeName = **reinterpret_cast<string **>(esp + 0x30);
  bool isIntelRange = equals(rangeName, "Radar") || equals(rangeName, "Omni") ||
                      equals(rangeName, "Sonar");
  if (isIntelRange)
    return self;
  return nullptr;
}

extern void __fastcall
Hooked_SyncVisionRange(ReconBlip *, Unit *) asm("Hooked_SyncVisionRange");
void Hooked_SyncVisionRange(ReconBlip *reconBlip, Unit *unit) {
  auto &reconAttrib = reconBlip->Entity::mVarDat.mAttributes;
  auto &unitAttrib = unit->mVarDat.mAttributes;
  reconAttrib.SetIntelRadius(ENTATTR_Vision,
                             unitAttrib.GetIntelRadius(ENTATTR_Vision));

  if (unit->mArmy->IsAlly(g_CWldSession->focusArmyIndex)) {
    reconAttrib.SetIntelRadius(ENTATTR_Radar,
                               unitAttrib.GetIntelRadius(ENTATTR_Radar));
    reconAttrib.SetIntelRadius(ENTATTR_Sonar,
                               unitAttrib.GetIntelRadius(ENTATTR_Sonar));
    reconAttrib.SetIntelRadius(ENTATTR_Omni,
                               unitAttrib.GetIntelRadius(ENTATTR_Omni));
  }
}

bool __cdecl ShouldAddUnit(UserArmy *focusArmy, UserUnit *userUnit) {
  auto session = g_CWldSession;
  if (userUnit->mArmy == focusArmy)
    return true;

  if (intelRangeBehavior == kOwnUnits)
    return false;

  if (!userUnit->mArmy->IsAlly(session->focusArmyIndex))
    return false;

  auto blueprint = static_cast<RUnitBlueprint *>(userUnit->mParams.mBlueprint);
  if (intelRangeBehavior == kOwnUnitsAndAlliedBuildings &&
      blueprint->mPhysics.mMotionType != RULEUMT_None)
    // let's assume this is not a building
    return false;

  auto &unitAttrib = userUnit->mVarDat.mAttributes;
  bool hasIntel = (unitAttrib.GetIntelRadius(ENTATTR_Radar) |
                   unitAttrib.GetIntelRadius(ENTATTR_Sonar) |
                   unitAttrib.GetIntelRadius(ENTATTR_Omni)) != 0;
  return hasIntel;
}

} // extern "C"
