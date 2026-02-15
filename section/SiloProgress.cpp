#include "moho.h"
#include <algorithm>

// overrides `Moho::CAiSiloBuildImpl::SiloTick` switch case 2 at 0x005CF42A
void __stdcall CheckSiloProgress(CAiSiloBuildImpl *self, SEconValue *res)
{
    CEconRequest *cons_data = self->mRequest;

    float energy_ratio = cons_data->mAddWhenSetOff.ENERGY / self->mSegmentCost.ENERGY;
    float mass_ratio = cons_data->mAddWhenSetOff.MASS / self->mSegmentCost.MASS;
    float available_ratio = (mass_ratio > energy_ratio ? energy_ratio : mass_ratio);
    *res = cons_data->mAddWhenSetOff;
    cons_data->mAddWhenSetOff = {0, 0};

    auto *unit = self->mUnit;
    unit->mUnitVarDat.mMaintainenceCost = self->mSegmentCost;
    unit->mUnitVarDat.mResourcesSpent += *res;

    self->mCurSegments += available_ratio;
    unit->mUnitVarDat.mWorkProgress = self->mCurSegments / self->mSegments;

    if (self->mCurSegments >= self->mSegments)
        self->mState = 3;
}

// overrides `Moho::CAiSiloBuildImpl::SiloAssistWithResource` at 0x005CF030
void __thiscall AddSiloEco(CAiSiloBuildImpl *self, SEconValue *econ)
{
    if (self->mState == 0)
        return;

    self->mSegmentSpent += *econ;

    float energy_ratio = self->mSegmentSpent.ENERGY / self->mSegmentCost.ENERGY;
    float mass_ratio = self->mSegmentSpent.MASS / self->mSegmentCost.MASS;
    float available_ratio = std::min(mass_ratio, energy_ratio);

    float needed_segments = self->mSegments - self->mCurSegments;
    float used_ratio = std::min(needed_segments, available_ratio);

    SEconValue spent = self->mSegmentCost * used_ratio;
    self->mCurSegments += used_ratio;
    self->mUnit->mUnitVarDat.mWorkProgress = self->mCurSegments / self->mSegments;
    self->mUnit->mUnitVarDat.mResourcesSpent += spent;

    self->mSegmentSpent-= spent;

    if (self->mCurSegments >= self->mSegments)
        self->mState = 3;
}