#include "moho.h"

struct struct_Resources // sizeof=0x8
{
    float ENERGY;
    float MASS;

    struct_Resources(float e = 0, float m = 0) : ENERGY{e}, MASS{m}
    {
    }

    struct_Resources operator+(const struct_Resources &other)
    {
        return struct_Resources{ENERGY + other.ENERGY, MASS + other.MASS};
    }

    struct_Resources &operator+=(const struct_Resources &other)
    {
        ENERGY += other.ENERGY;
        MASS += other.MASS;
        return *this;
    }
    struct_Resources &operator-=(const struct_Resources &other)
    {
        ENERGY -= other.ENERGY;
        MASS -= other.MASS;
        return *this;
    }

    struct_Resources operator*(float mul)
    {
        return struct_Resources{ENERGY * mul, MASS * mul};
    }

    struct_Resources &operator=(const struct_Resources &other)
    {
        ENERGY = other.ENERGY;
        MASS = other.MASS;
        return *this;
    }
};

struct struct_ConsumptionData
{
    struct_ConsumptionData *next;
    struct_ConsumptionData *prev;
    struct_Resources perSecond;
    struct_Resources addWhenSetOff;
};

struct struct_UnitData
{
    int dword0;
    int dword4;
    bool autoMode;
    bool autoSurfaceMode;
    bool isBusy;
    bool byteB;
    float fuelRatio;
    float shieldRatio;
    int stunTicks;
    bool isPaused;
    bool isValidTarget;
    bool byte1A;
    bool byte1B;
    float dword1C;
    /*Moho::EFireState*/ int fireState;
    float workProgress;
    int tacticalSiloBuildCount;
    int nukeSiloBuildCount;
    int tacticalSiloStorageCount;
    int nukeSiloStorageCount;
    int tacticalSiloMaxStorageCount;
    int nukeSiloMaxStorageCount;
    int dword40;
    string customName;
    float energyProduced;
    float massProduced;
    struct_Resources spent;
    struct_Resources MaintainenceCost;
    int focusUnit;
    int dword7C;
    int dword80;
    int dword84;
    int dword88;
    int dword8C;
    int dword90;
    Vector3f vec3;
    int dwordA0;
    int dwordA4;
    int gapA8;
    bool v0;
    bool v1;
    bool v2;
    bool v3;
    bool gapB0[24];
    int dwordC8;
    int dwordCC;
    int dwordD0;
    int dwordD4;
    int gapD8;
    int gapDC;
    int gapE0;
    int gapE4;
    int gapE8;
    float gapEC;
    int gapF0;
    int gapF4;
    int gapF8;
    bool gapFC[164];
    /*Moho::UnitAttributes*/ char attributes[0x70];
    unsigned int scriptbits;
    bool gap214[4];
    int unitStates1;
    int unitStates2;
    /*Moho::CUnitMotion*/ void *unitMotion;
};
VALIDATE_SIZE(struct_UnitData, 0x224);

struct Moho_Unit
{
    void *vtable;
    void *chain;
    char entity[0x270];
    /*struct_StatItemRRef*/ char statRref[0x10];
    struct_UnitData unitData;
};

struct CAiSiloBuildImpl // sizeof=0x4C
{
    void *vtable;
    Moho_Unit *unit;
    int vec[3];
    int v5;
    int v6;
    int v7;
    int v8;
    int *v9;
    int v10;
    struct_ConsumptionData *cons_data;
    signed __int32 state;
    struct_Resources segment_cost;
    struct_Resources segment_spent;
    float segments;
    float curSegments;
};

void __stdcall CheckSiloProgress(CAiSiloBuildImpl *silo_build, struct_Resources *res)
{
    struct_ConsumptionData *cons_data = silo_build->cons_data;

    float energy_ratio = cons_data->addWhenSetOff.ENERGY / silo_build->segment_cost.ENERGY;
    float mass_ratio = cons_data->addWhenSetOff.MASS / silo_build->segment_cost.MASS;
    float available_ratio = (mass_ratio > energy_ratio ? energy_ratio : mass_ratio);
    *res = cons_data->addWhenSetOff;
    cons_data->addWhenSetOff = {0, 0};

    auto *unit = silo_build->unit;
    unit->unitData.MaintainenceCost = silo_build->segment_cost;
    unit->unitData.spent += *res;

    silo_build->curSegments += available_ratio;
    silo_build->unit->unitData.workProgress = silo_build->curSegments / silo_build->segments;

    if (silo_build->curSegments >= silo_build->segments)
    {
        silo_build->state = 3;
    }
}

void __thiscall AddSiloEco(CAiSiloBuildImpl *silo_build, struct_Resources *econ)
{
    if (!silo_build->state)
        return;

    silo_build->segment_spent += *econ;

    float energy_ratio = silo_build->segment_spent.ENERGY / silo_build->segment_cost.ENERGY;
    float mass_ratio = silo_build->segment_spent.MASS / silo_build->segment_cost.MASS;
    float available_ratio = (mass_ratio > energy_ratio ? energy_ratio : mass_ratio);

    float needed_segments = silo_build->segments - silo_build->curSegments;

    auto *unit = silo_build->unit;

    struct_Resources spent;
    if (needed_segments > available_ratio)
    {
        spent = silo_build->segment_cost * available_ratio;
        silo_build->curSegments += available_ratio;
        silo_build->unit->unitData.workProgress = silo_build->curSegments / silo_build->segments;
    }
    else
    {
        spent = silo_build->segment_cost * needed_segments;
        silo_build->curSegments += needed_segments;
        silo_build->unit->unitData.workProgress = silo_build->curSegments / silo_build->segments;
    }

    unit->unitData.spent += spent;

    silo_build->segment_spent -= spent;

    if (silo_build->curSegments >= silo_build->segments)
        silo_build->state = 3;
}