// Address: 0x6A9041 (within function at 0x6A9010)
// Function: Moho::Unit::MotionTick
// Layer 4 - Physics/Motion: Unit motion tick
// Note: 0x6A9041 is mid-function; IDA resolved to containing function at 0x6A9010

Moho::Entity **__fastcall Moho::Unit::MotionTick(Moho::Unit_as_Entity this)
{
  struct _EXCEPTION_REGISTRATION_RECORD *ExceptionList; // eax
  Moho::CAiSiloBuildImpl *mSiloBuild; // ecx
  Moho::Unit *v4; // esi
  int mStunTicks; // eax
  Moho::CEconomyEvent_as_ListItem i; // ebx
  Moho::CEconomyEvent *v7; // eax
  Moho::Entity_base *v8; // eax
  Moho::Motor **p_mMotor; // eax
  Moho::Sim *mSim; // edx
  bool v11; // zf
  Moho::RUnitBlueprint *v12; // eax
  Moho::RUnitBlueprint *(__thiscall *GetBlueprint)(Moho::IUnit *); // edx
  Moho::RUnitBlueprint *v14; // eax
  Moho::RUnitBlueprint *(__thiscall *v15)(Moho::IUnit *); // edx
  Moho::RUnitBlueprint *v16; // eax
  float mBuildTime; // xmm0_4
  Moho::Entity_vtbl *vtable; // ebp
  Moho::UnitAttributes *v19; // eax
  Moho::CUnitCommandQueue *mCommandQueue; // eax
  Moho::WeakPtr_CUnitCommand *start; // ecx
  Moho::SSTICommandIssueData *v22; // eax
  struct_UnitSearchRes **v23; // eax
  Moho::CAiBuilderImpl *mBuilder; // edi
  std::map_uint_Entity a1; // [esp+18h] [ebp-DCh]
  std::map_uint_Entity v27; // [esp+18h] [ebp-DCh] SPLIT
  Moho::EntitySetTemplate_Entity v28; // [esp+1Ch] [ebp-D8h] SPLIT BYREF
  struct_UnitSearchRes **a1_12; // [esp+24h] [ebp-D0h]
  struct_UnitSearchRes **a1_16; // [esp+28h] [ebp-CCh]
  struct_UnitSearchRes *a1_20; // [esp+2Ch] [ebp-C8h]
  struct_UnitSearchRes **a1_24; // [esp+30h] [ebp-C4h]
  _BYTE a1_28[16]; // [esp+34h] [ebp-C0h] BYREF
  struct_UnitSearchRes a2; // [esp+44h] [ebp-B0h] BYREF
  float mBuildCostEnergy; // [esp+50h] [ebp-A4h]
  Moho::SSTICommandIssueData v36; // [esp+54h] [ebp-A0h] BYREF
  void *v37; // [esp+ECh] [ebp-8h]
  int v38; // [esp+F0h] [ebp-4h]

  v38 = -1;
  ExceptionList = NtCurrentTeb()->NtTib.ExceptionList;
  v37 = &loc_BB9A18;
  v36.mQueue = (Moho::CUnitCommandQueue *)ExceptionList;
  Moho::Sim::Logf(ADJ(this)->mSim, "0x%08x's motion tick.\n", ADJ(this)->mConstDat.mId);
  mSiloBuild = ADJ(this)->mSiloBuild;
  if ( mSiloBuild )
    mSiloBuild->SiloTick(mSiloBuild);
  v4 = ADJ(this);
  Moho::Unit::UpdateGuardFormation(ADJ(this));
  Moho::Unit::UpdateInfoCache(ADJ(this));
  mStunTicks = ADJ(this)->mUnitVarDat.mStunTicks;
  if ( mStunTicks > 0 )
    ADJ(this)->mUnitVarDat.mStunTicks = mStunTicks - 1;
  if ( ADJ(this)->mUnitMotion )
    a1._Myfirstiter = (struct _Iterator_base *)Moho::CUnitMotion::MotionTick(ADJ(this)->mUnitMotion);
  else
    a1._Myfirstiter = (struct _Iterator_base *)1;
  for ( i = &ADJ(ADJ(this)->mEconEvents.mNext)->mList; i != &ADJ(this)->mEconEvents; i = &ADJ(ADJ(i)->mList.mNext)->mList )
  {
    if ( i )
      v7 = ADJ(i);
    else
      v7 = 0;
    Moho::CEconomyEvent::OnTick(v7);
  }
  Moho::CAniActor::UpdateManipulators(ADJ(this)->mAniActor, &ADJ(this)->mLastTrans);
  v8 = &ADJ(ADJ(this)->mAttachInfo.mEnt.mEntity)->Moho::Entity_base;
  if ( v8 )
    p_mMotor = &v8[-1].mMotor;
  else
    p_mMotor = 0;
  mSim = ADJ(this)->mSim;
  v11 = !ADJ(this)->mVarDat.mIsBeingBuilt;
  ADJ(this)->mUnitVarDat.mIsBusy = p_mMotor != 0;
  if ( v11 )
  {
    if ( ADJ(this)->mVarDat.mMaxHealth > ADJ(this)->mVarDat.mHealth
      && v4->__vftable_unit->GetAttributes1(ADJ(this))->mRegenRate > 0.0 )
    {
      vtable = ADJ(this)->__vftable;
      v19 = v4->__vftable_unit->GetAttributes1(ADJ(this));
      vtable->AdjustHealth(this, this != (Moho::Unit_as_Entity)8 ? this : 0, v19->mRegenRate * 0.1);
    }
  }
  else if ( (signed int)(mSim->mCurTick - ADJ(this)->mCreationTick) > 1 )
  {
    v12 = v4->__vftable_unit->GetBlueprint(ADJ(this));
    GetBlueprint = v4->__vftable_unit->GetBlueprint;
    mBuildCostEnergy = v12->mEconomy.mBuildCostEnergy;
    v14 = GetBlueprint(ADJ(this));
    v15 = v4->__vftable_unit->GetBlueprint;
    *(float *)&a2.mFound = v14->mEconomy.mBuildCostMass;
    v16 = v15(ADJ(this));
    mBuildTime = mBuildCostEnergy;
    if ( *(float *)&a2.mFound > mBuildCostEnergy )
      mBuildTime = *(float *)&a2.mFound;
    if ( v16->mEconomy.mBuildTime > mBuildTime )
      mBuildTime = v16->mEconomy.mBuildTime;
    if ( mBuildTime > 0.0 )
    {
      ((void (__thiscall *)(Moho::Unit_as_Entity, _DWORD))ADJ(this)->Materialize)(this, -0.1 / mBuildTime);
      if ( ADJ(this)->mVarDat.mHealth <= 0.0 )
        Moho::CScriptObject::RunScript((Moho::CScriptObject *)&ADJ(this)->Moho::Entity, "OnDecayed");
    }
  }
  Moho::Unit::HandleResourceManagement(ADJ(this));
  if ( ADJ(this)->mIsNotPod )
  {
    mCommandQueue = ADJ(this)->mCommandQueue;
    if ( mCommandQueue )
    {
      start = mCommandQueue->mCommands._Myfirst;
      if ( !start || !(mCommandQueue->mCommands._Mylast - start) )
      {
        v28.mNext = &v28;
        v28.mPrev = &v28;
        a1_12 = (struct_UnitSearchRes **)a1_28;
        a1_16 = (struct_UnitSearchRes **)a1_28;
        a1_20 = &a2;
        a1_24 = (struct_UnitSearchRes **)a1_28;
        v38 = 0;
        Moho::EntitySetTemplate_Unit::Add(&v28, (struct_UnitSearchRes *)&a2.mFound, ADJ(this));
        v22 = Moho::SSTICommandIssueData::SSTICommandIssueData(UNITCOMMAND_AssistCommander, &v36);
        LOBYTE(v38) = 1;
        Moho::UNIT_IssueCommand(&v28, ADJ(this)->mSim, v22, 0);
        LOBYTE(v38) = 0;
        Moho::SSTICommandIssueData::~SSTICommandIssueData(&v36);
        v38 = -1;
        v23 = a1_12;
        if ( a1_12 != a1_24 )
        {
          operator delete[](a1_12);
          v23 = a1_24;
          a1_12 = a1_24;
          a1_20 = *a1_24;
        }
        a1_16 = v23;
        v28.mPrev->mNext = v28.mNext;
        v28.mNext->Moho::TDatListItem_EntitySetTemplate_Entity::mPrev = v28.mPrev;
        v28.mNext = &v28;
        v28.mPrev = &v28;
      }
    }
  }
  mBuilder = ADJ(this)->mBuilder;
  if ( mBuilder )
    mBuilder->OnTick(mBuilder);
  return (Moho::Entity **)v27._Myfirstiter;
}
