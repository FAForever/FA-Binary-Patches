// Address: 0x5AE2D0
// Function: Moho::CAiPathNavigator::UpdateCurrentPosition
// Navigation update loop - handles path following, formation following, repath triggers

void __userpurge Moho::CAiPathNavigator::UpdateCurrentPosition(
        Moho::CAiPathNavigator *ebx0@<ebx>,
        Moho::COGrid *xmm0_4_0@<xmm0>,
        Wm3::Vector3f *arg0)
{
  unsigned int mCurTick; // ecx
  unsigned int v4; // edi
  int v5; // eax
  int v6; // eax
  Moho::SOCellPos *Myfirst; // ecx
  unsigned __int16 z; // dx
  Moho::COGrid *v9; // eax
  unsigned __int16 x; // dx
  int v11; // eax
  unsigned __int16 v12; // dx
  long double v13; // st7
  Moho::TDatListItem_Listener *mNext; // eax
  Moho::HPathCell mCurrentPos; // eax
  Moho::RUnitBlueprintPhysics *p_mPhysics; // eax
  bool v17; // zf
  Moho::ELayer mLayer; // eax
  Moho::Unit *v19; // ecx
  int *v20; // eax
  int v21; // eax
  __int16 v22; // cx
  Moho::Unit *mUnit; // esi
  Moho::Sim *mSim; // eax
  Moho::SFootprint *Footprint; // eax
  int v26; // eax
  int v27; // esi
  Moho::Unit *v28; // edi
  Moho::Unit *v29; // esi
  Moho::WeakPtr_Unit *v30; // ecx
  Moho::WeakObject_IUnit *v31; // eax
  Moho::WeakObject_IUnit *v32; // edi
  Moho::WeakObject_IUnit *v33; // eax
  Moho::WeakPtr_Unit *v34; // edi
  Moho::Unit *v35; // eax
  int v36; // eax
  Moho::SOCellPos v37; // edx
  Moho::IAiNavigator *mNavigator; // ecx
  Moho::SFootprint *v39; // eax
  double v40; // st7
  float v41; // xmm0_4
  Moho::SOCellPos v42; // esi
  Moho::STIMap *v43; // esi
  char v44; // al
  double v45; // st7
  float v46; // xmm0_4
  __int16 z0; // ax
  int v48; // eax
  Moho::WeakObject_IUnit *p_mNext; // eax
  Moho::Unit *v50; // esi
  Moho::HPathCell mTargetPos; // edi
  double v52; // st7
  Moho::HPathCell v53; // edx
  Moho::Unit *v54; // esi
  bool v55; // [esp+16h] [ebp-2Ah]
  bool v56; // [esp+17h] [ebp-29h]
  bool v57; // [esp+17h] [ebp-29h]
  int v58; // [esp+18h] [ebp-28h]
  Moho::COGrid *v59; // [esp+18h] [ebp-28h]
  Moho::SOCellPos a2; // [esp+1Ch] [ebp-24h] BYREF
  unsigned int v61; // [esp+20h] [ebp-20h]
  Moho::COGrid *eax0; // [esp+24h] [ebp-1Ch] BYREF
  unsigned int v63; // [esp+28h] [ebp-18h]
  Moho::EntId mId; // [esp+2Ch] [ebp-14h] BYREF
  float v65; // [esp+30h] [ebp-10h]
  Wm3::Vector3f a1; // [esp+34h] [ebp-Ch] BYREF

  Moho::Sim::Logf(ebx0->mSim, "  in UpdateCurrentPosition() for 0x%08x\n", ebx0->mPathFinder->mUnit->mConstDat.mId);
  mId = ebx0->mPathFinder->mUnit->mConstDat.mId;
  gpg::MD5Context::Update(&ebx0->mSim->mContext, &mId, 4u);
  Moho::CAiPathNavigator::SetCurrentPosition(arg0, ebx0);
  mCurTick = ebx0->mSim->mCurTick;
  v4 = mCurTick % 7;
  v5 = ebx0->v40;
  v61 = mCurTick;
  v63 = mCurTick % 0xD;
  if ( v5 > 0 )
  {
    ebx0->v40 = v5 - 1;
    if ( v5 == 1 )
      sub_5ADFE0(ebx0, (Moho::COGrid *)ebx0->mSearchType);
    ebx0->mTargetPos = ebx0->mCurrentPos;
    return;
  }
  v6 = ebx0->v26;
  if ( v6 > 0 )
  {
    ebx0->v26 = v6 - 1;
    if ( v6 == 1 )
      sub_5AEC70((Moho::SOCellPos)ebx0, 2);
    return;
  }
  while ( 1 )
  {
    Myfirst = (Moho::SOCellPos *)ebx0->mPath.path._Myfirst;
    if ( !Myfirst )
      break;
    if ( (unsigned int)(((char *)ebx0->mPath.path._Mylast - (char *)Myfirst) >> 2) < 2 )
      break;
    z = ebx0->mCurrentPos.z;
    mId = (unsigned __int16)Myfirst->x - (unsigned __int16)ebx0->mCurrentPos.x;
    v9 = (Moho::COGrid *)((unsigned __int16)Myfirst->z - z);
    x = ebx0->mCurrentPos.x;
    eax0 = v9;
    v11 = (unsigned __int16)Myfirst[1].x - x;
    v12 = ebx0->mCurrentPos.z;
    v13 = sqrt((double)mId * (double)mId + (double)(int)eax0 * (double)(int)eax0);
    mId = v11;
    eax0 = (Moho::COGrid *)((unsigned __int16)Myfirst[1].z - v12);
    if ( v13 < sqrt((double)v11 * (double)v11 + (double)(int)eax0 * (double)(int)eax0)
      || !sub_5AF5B0((Moho::SOCellPos)ebx0, Myfirst[1]) )
    {
      break;
    }
    sub_5AD830(1, (Moho::CAiNavigatorLand *)ebx0);
  }
  mNext = ebx0->mNext;
  if ( mNext != (Moho::TDatListItem_Listener *)5 && mNext != (Moho::TDatListItem_Listener *)6 )
  {
    ebx0->mTargetPos = ebx0->mCurrentPos;
    return;
  }
  mCurrentPos = ebx0->mCurrentPos;
  if ( mCurrentPos == *(_DWORD *)&ebx0->mPath.path._Mylast[-1] )
  {
    ebx0->mNext = 0;
    ebx0->mTargetPos = mCurrentPos;
    ebx0->v26 = 0;
    return;
  }
  p_mPhysics = &ebx0->mPathFinder->mUnit->__vftable_unit->GetBlueprint(ebx0->mPathFinder->mUnit)->mPhysics;
  v17 = p_mPhysics->mMotionType == RULEUMT_SurfacingSub;
  v65 = *(float *)&p_mPhysics;
  if ( v17 )
  {
    mLayer = ebx0->mPathFinder->mUnit->mVarDat.mLayer;
    if ( ebx0->mLayer != mLayer )
    {
      ebx0->mLayer = mLayer;
LABEL_21:
      sub_5AEC70((Moho::SOCellPos)ebx0, 2);
      return;
    }
  }
  v55 = ebx0->v38a != 0;
  v56 = ebx0->v38a == 0;
  *(float *)&mId = sub_5AD370(&ebx0->mCurrentPos);
  v21 = *v20;
  if ( *(_DWORD *)&ebx0->mCurrentPos != v21 && !ebx0->v37b && !ebx0->v38c && !v55 )
  {
    LOBYTE(v19) = ebx0->mIsInFormation;
    if ( (_BYTE)v19 )
    {
      if ( ebx0->mEntityIdMod14 != v63 )
        goto LABEL_42;
    }
    else if ( ebx0->mEntityIdMod7 != v4 )
    {
      goto LABEL_42;
    }
    v22 = ebx0->mCurrentPos.x;
    v58 = v21;
    mUnit = ebx0->mPathFinder->mUnit;
    a2.z = ebx0->mCurrentPos.z;
    mSim = ebx0->mSim;
    a2.x = v22;
    eax0 = mSim->mOGrid;
    Footprint = Moho::Entity::GetFootprint(&mUnit->Moho::Entity);
    LOBYTE(v26) = Moho::OCCUPY_FootprintFits(eax0, &a2, Footprint, OC_ANY);
    if ( v26 )
    {
      sub_62A990((int)mUnit);
      eax0 = xmm0_4_0;
      if ( *(float *)&mId <= *(float *)&xmm0_4_0 )
        v27 = v58;
      else
        v27 = *(_DWORD *)sub_5AE170(*(float *)&eax0);
      if ( !(unsigned __int8)sub_5AF4E0(ebx0, *(_DWORD *)&ebx0->mCurrentPos, v27)
        || !(unsigned __int8)sub_5AF670(*(_DWORD *)&ebx0->mCurrentPos, v27) )
      {
        v56 = 0;
LABEL_41:
        v55 = 1;
        goto LABEL_42;
      }
      if ( *(float *)(LODWORD(v65) + 80) > *(float *)(LODWORD(v65) + 84) && v61 > ebx0->v29 + 10 )
        goto LABEL_41;
    }
  }
LABEL_42:
  if ( !ebx0->mIsInFormation || !v56 || v61 <= ebx0->v31 + 100 )
    goto LABEL_96;
  v28 = ebx0->mPathFinder->mUnit;
  a2 = (Moho::SOCellPos)v28;
  v29 = 0;
  if ( Moho::Unit::GetFormation(v19) )
  {
    v32 = &ADJ(v28->mInfo.v1.mUnit)->Moho::WeakObject_IUnit;
    if ( v32 && v32 != (Moho::WeakObject_IUnit *)4 )
      v29 = (Moho::Unit *)((int (__thiscall *)(Moho::WeakObject_IUnit *))v32[-1].mNextUse->mNext)(&v32[-1]);
    v33 = &ADJ(ebx0->v32.mUnit)->Moho::WeakObject_IUnit;
    v34 = &ebx0->v32;
    if ( v33 )
      v35 = (Moho::Unit *)&v33[-1];
    else
      v35 = 0;
    v57 = v29 != v35;
    Moho::WeakPtr_Unit::Set(&ebx0->v32, v29);
    if ( !&ADJ(v34->mUnit)->Moho::WeakObject_IUnit
      || &ADJ(v34->mUnit)->Moho::WeakObject_IUnit == (Moho::Unit_as_WeakObject)4 )
    {
      v37 = a2;
    }
    else
    {
      if ( &ADJ(v34->mUnit)->Moho::WeakObject_IUnit )
        v36 = (int)(&ADJ(v34->mUnit)->Moho::WeakObject_IUnit - 1);
      else
        v36 = 0;
      v37 = a2;
      if ( v36 != a2 )
      {
        mNavigator = v29->mNavigator;
        if ( mNavigator )
        {
          if ( mNavigator->GetStatus(mNavigator) == AINAVSTATUS_Thinking )
          {
            ebx0->mTargetPos = ebx0->mCurrentPos;
            ebx0->v37b = 1;
            goto LABEL_96;
          }
          v37 = a2;
        }
        ebx0->v37b = 0;
        a1 = *(Wm3::Vector3f *)(*(_DWORD *)&v37 + 1436);
        v39 = Moho::Entity::GetFootprint((Moho::Entity *)(*(_DWORD *)&v37 + 8));
        Moho::SFootprint::ToCellPos((Moho::SOCellPos *)&eax0, &a1, v39);
        v59 = eax0;
        if ( ebx0->mEntityIdMod14 != v63 && Wm3::Vector3::Compare(&ebx0->v34, &vec0) )
          return;
        if ( (Moho::COGrid *)ebx0->mTargetPos == v59 )
        {
          ebx0->v34 = a1;
          return;
        }
        v40 = sub_5AD370(&ebx0->mCurrentPos);
        v65 = v40;
        if ( v40 >= 10.0 )
          v41 = 10.0;
        else
          v41 = v65;
        v63 = LODWORD(v41);
        sub_5AE170(v41);
        v42 = (Moho::SOCellPos)LODWORD(v65);
        if ( (unsigned __int8)sub_5AF4E0(ebx0, *(_DWORD *)&ebx0->mCurrentPos, LODWORD(v65)) )
        {
          if ( sub_5AF5B0((Moho::SOCellPos)ebx0, v42) )
          {
            v43 = *(Moho::STIMap **)(*(_DWORD *)(*(_DWORD *)&a2 + 336) + 2252);
            v44 = (*(int (__thiscall **)(_DWORD))(**(_DWORD **)(*(_DWORD *)&a2 + 340) + 184))(*(_DWORD *)(*(_DWORD *)&a2 + 340));
            if ( Moho::STIMap::IsWithin(v43, &a1, 1.0, v44) )
            {
              v45 = sub_5AD370(&ebx0->mCurrentPos);
              v46 = a1.x;
              ebx0->v28 = v45 * 0.5;
              ebx0->mTargetPos = (Moho::HPathCell)v59;
              ebx0->v37c = 1;
              ebx0->v34.x = v46;
              ebx0->v34.y = a1.y;
              ebx0->v34.z = a1.z;
              ebx0->mNext = (Moho::TDatListItem_Listener *)6;
              if ( (unsigned int)sub_5A9CF0() > 1 )
              {
                sub_5AD130();
                z0 = ebx0->mGoal.mPos1.z0;
                LOWORD(v63) = ebx0->mGoal.mPos1.x0;
                HIWORD(v63) = z0;
                sub_5AFEC0();
              }
              return;
            }
          }
        }
        v17 = ebx0->v37c == 0;
        ebx0->v31 = v61;
        if ( !v17 || sub_5A9CF0() == 1 )
        {
          ebx0->v34 = vec0;
          ebx0->v37c = 0;
          sub_5AD130();
          sub_5ADFE0(ebx0, 0);
          return;
        }
        goto LABEL_96;
      }
    }
    if ( v57 )
    {
      v48 = &ADJ(v34->mUnit)->Moho::WeakObject_IUnit ? (int)(&ADJ(v34->mUnit)->Moho::WeakObject_IUnit - 1) : 0;
      if ( v48 == v37 )
      {
        for ( ; sub_5A9CF0(); ebx0->v26 = 0 )
          sub_5AD130();
        ebx0->v34 = vec0;
        ebx0->v37c = 0;
        ebx0->v37b = 0;
        sub_5ADFE0(ebx0, 0);
        return;
      }
    }
    ebx0->v34 = vec0;
    ebx0->v37c = 0;
    p_mNext = &ADJ(v34->mUnit)->Moho::WeakObject_IUnit;
    if ( &ADJ(v34->mUnit)->Moho::WeakObject_IUnit )
    {
      for ( ; p_mNext->mNextUse != v34; p_mNext = (Moho::WeakObject_IUnit *)&p_mNext->mNextUse->mNext )
        ;
      p_mNext->mNextUse = ebx0->v32.mNext;
      v34->mUnit = 0;
      ebx0->v32.mNext = 0;
    }
    ebx0->mIsInFormation = 0;
  }
  else
  {
    v30 = &ebx0->v32;
    ebx0->mIsInFormation = 0;
    v31 = &ADJ(ebx0->v32.mUnit)->Moho::WeakObject_IUnit;
    if ( v31 )
    {
      for ( ; v31->mNextUse != v30; v31 = (Moho::WeakObject_IUnit *)&v31->mNextUse->mNext )
        ;
      v31->mNextUse = ebx0->v32.mNext;
      v30->mUnit = 0;
      ebx0->v32.mNext = 0;
    }
    ebx0->v37c = 0;
    ebx0->v34 = vec0;
  }
LABEL_96:
  if ( !ebx0->v37b )
  {
    v50 = ebx0->mPathFinder->mUnit;
    if ( Wm3::Vector3::Compare(&v50->mVarDat.mCurTransform.pos, &v50->mVarDat.mLastTransform.pos)
      || v50->__vftable_unit->IsUnitState(v50, UNITSTATE_Immobile) )
    {
      ebx0->v30 = 0;
    }
    else
    {
      ++ebx0->v30;
    }
    if ( ebx0->v28 >= *(float *)&mId || v55 || ebx0->v30 > 30 )
    {
      mTargetPos = ebx0->mTargetPos;
      ebx0->v29 = v61;
      v65 = *(float *)&mTargetPos;
      if ( (unsigned __int8)sub_5AF7E0() )
      {
        v52 = sub_5AD370(&ebx0->mCurrentPos);
        ebx0->v37c = 0;
        ebx0->v28 = v52 * 0.5;
        ebx0->v34 = vec0;
        ebx0->v38a = 0;
        return;
      }
      if ( ebx0->v30 > 30 )
      {
        v53 = ebx0->mCurrentPos;
        ebx0->mNext = 0;
        ebx0->mTargetPos = v53;
        ebx0->v26 = 0;
        return;
      }
      if ( ebx0->v38a )
      {
        ebx0->v38a = 0;
        sub_5AEC70((Moho::SOCellPos)ebx0, 3);
        return;
      }
      if ( *(_DWORD *)&ebx0->mPath.path._Mylast[-1] == mTargetPos
        && !ebx0->mPathFinder->mUnit->__vftable_unit->IsUnitState(
              ebx0->mPathFinder->mUnit,
              UNITSTATE_WaitingForTransport) )
      {
        v54 = ebx0->mPathFinder->mUnit;
        a1.x = (float)(unsigned __int16)mTargetPos.x;
        a1.y = 0.0;
        a1.z = (float)HIWORD(v65);
        if ( sub_62ABA0(&a1, v54, 1) )
        {
          ebx0->v26 = 10;
          return;
        }
      }
      goto LABEL_21;
    }
  }
}
