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

void *__thiscall RenderRange__Moho__UserUnit__UserUnit(UserUnit *this_,
                                                       uintptr_t esp) {
  if (this_->mArmy->mConstDat.mIndex == g_CWldSession->focusArmyIndex)
    return this_;

  // Compiler, I believe in you. We're in a hot path, optimize and inline the
  // lambda pls p.s. I saw the listing, mine handled it, and there’s no call
  // there
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
    return this_;
  return nullptr;
}

extern void __fastcall
Hooked_SyncVisionRange(ReconBlip *, Unit *) asm("Hooked_SyncVisionRange");
void Hooked_SyncVisionRange(ReconBlip *recon, Unit *unit) {
  recon->Entity::mVarDat.mAttributes.mVisionRange =
      unit->mVarDat.mAttributes.mVisionRange;
  recon->Entity::mVarDat.mAttributes.mRadarRange =
      unit->mVarDat.mAttributes.mRadarRange;
  recon->Entity::mVarDat.mAttributes.mSonarRange =
      unit->mVarDat.mAttributes.mSonarRange;
  recon->Entity::mVarDat.mAttributes.mOmniRange =
      unit->mVarDat.mAttributes.mOmniRange;
}

bool __cdecl ShouldAddUnit(UserArmy *focusArmy, UserUnit *userUnit) {
  auto session = g_CWldSession;
  // auto session = *reinterpret_cast<uintptr_t*>(0x10A6470);
  if (userUnit->mArmy == focusArmy) {
    // Optimization: To avoid double rendering,
    // do not render the global rings;
    // they will be rendered in the selection rings render
    [[likely]] if (session->selectedUnits.size() < 67)
      return true;
    return !session->selectedUnits.contains(userUnit);
  }

  if (intelRangeBehavior == kOwnUnits)
    return false;

  if (!userUnit->mArmy->isAlly(session->focusArmyIndex))
    return false;

  auto blueprint = static_cast<RUnitBlueprint *>(userUnit->mParams.mBlueprint);
  if (intelRangeBehavior == kOwnUnitsAndAlliedBuildings &&
      blueprint->mPhysics.mMotionType != RULEUMT_None)
    // let's assume this is not a building
    return false;

  auto &intel = blueprint->mIntel;
  bool hasIntel =
      (intel.mRadarRadius | intel.mSonarRadius | intel.mOmniRadius) != 0;
  return hasIntel;
}

} // extern "C"
