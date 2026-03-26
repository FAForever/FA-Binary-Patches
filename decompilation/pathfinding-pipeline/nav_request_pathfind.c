// Address: 0x5ADFE0
// Function: sub_5ADFE0 (Navigator Request Pathfind - initiates a new pathfinding request)
// Calls nav_set_goal and nav_enqueue_path, sets up path state

char __stdcall sub_5ADFE0(Moho::CAiPathNavigator *a2, Moho::COGrid *eax0)
{
  Moho::CAiPathNavigator *v2; // ebp
  __int16 z; // cx
  Moho::CAiPathFinder *mPathFinder; // eax
  bool v5; // zf
  __int16 z0; // cx
  Moho::Unit *mUnit; // eax
  int mUnitStates_high; // edx
  Moho::CAiPathFinder *v9; // ecx
  __int16 v10; // dx
  Moho::CAiPathFinder *v11; // eax
  Moho::Sim *mSim; // ecx
  __int16 x; // si
  Moho::Unit *v14; // eax
  Moho::COGrid *mOGrid; // edx
  Moho::SFootprint *Footprint; // eax
  int v17; // eax
  Moho::SOCellPos v19; // [esp-4h] [ebp-18h]

  v2 = a2;
  sub_5ADC70(a2);
  z = v2->mCurrentPos.z;
  mPathFinder = v2->mPathFinder;
  mPathFinder->mCurrentPos.x = v2->mCurrentPos.x;
  mPathFinder->mCurrentPos.z = z;
  sub_5AA120(&v2->mGoal, v2->mPathFinder);
  v2->mPathFinder->mOGrid2 = (Moho::ESearchType)eax0;
  v2->v34 = vec0;
  v5 = v2->v37b == 0;
  a2 = 0;
  v2->v12 = 0;
  v2->v26 = 0;
  v2->v38a = 0;
  if ( v5 )
  {
    mUnit = v2->mPathFinder->mUnit;
    mUnitStates_high = HIDWORD(mUnit->mUnitVarDat.mUnitStates);
    LODWORD(mUnit->mUnitVarDat.mUnitStates) |= (unsigned int)&loc_7FFFFD + 3;
    HIDWORD(mUnit->mUnitVarDat.mUnitStates) = mUnitStates_high;
    sub_5AA310(v2->mPathFinder);
    v9 = v2->mPathFinder;
    v2->mNext = (Moho::TDatListItem_Listener *)3;
    v2->__vftable_listener[1].OnEvent = (void (__thiscall *)(Moho::Listener *, int))v2->mPrev;
    v2->mPrev->mPrev = (Moho::TDatListItem_Listener *)v2->__vftable_listener;
    v9 = (Moho::CAiPathFinder *)((char *)v9 + 12);
    v2->__vftable_listener = (Moho::Listener_vtbl *)&v2->Moho::Listener_NavPath;
    v2->mPrev = (Moho::TDatListItem_Listener *)&v2->Moho::Listener_NavPath;
    v2->__vftable_listener = (Moho::Listener_vtbl *)v9->__vftable;
    v2->mPrev = (Moho::TDatListItem_Listener *)v9;
    v9->__vftable = (Moho::IPathTraveler_vtbl *)&v2->Moho::Listener_NavPath;
    v2->__vftable_listener[1].OnEvent = (void (__thiscall *)(Moho::Listener *, int))&v2->Moho::Listener_NavPath;
  }
  else
  {
    z0 = v2->mGoal.mPos1.z0;
    LOWORD(a2) = v2->mGoal.mPos1.x0;
    HIWORD(a2) = z0;
    sub_5AFEC0();
    v2->mNext = (Moho::TDatListItem_Listener *)6;
  }
  v10 = v2->mCurrentPos.z;
  v11 = v2->mPathFinder;
  mSim = v2->mSim;
  x = v2->mCurrentPos.x;
  v2->v38c = 0;
  v14 = v11->mUnit;
  HIWORD(a2) = v10;
  mOGrid = mSim->mOGrid;
  LOWORD(a2) = x;
  eax0 = mOGrid;
  Footprint = Moho::Entity::GetFootprint(&v14->Moho::Entity);
  LOBYTE(v17) = Moho::OCCUPY_FootprintFits(eax0, (Moho::SOCellPos *)&a2, Footprint, OC_ANY);
  v2->v37d = v17 && (v19.x = x, v19.z = HIWORD(a2), LOBYTE(v17) = sub_5AF5B0((Moho::SOCellPos)v2, v19), (_BYTE)v17);
  return v17;
}
