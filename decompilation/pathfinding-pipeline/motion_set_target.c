// Address: 0x6B85E0
// Function: Moho::CUnitMotion::SetTarget
// Layer 4 - Physics/Motion: Set movement target position

void __userpurge Moho::CUnitMotion::SetTarget(
        Moho::CUnitMotion *a1@<esi>,
        Wm3::Vector3f *a2,
        Wm3::Vector3f *a3,
        Moho::ELayer a4)
{
  char v4; // al
  Moho::Unit *mUnit; // ecx
  Moho::CHeightField *field; // edx
  int x0; // edi
  int z0; // ebx
  int x1; // eax
  int z1; // edx
  Moho::STIMap *mMapData; // edx
  float x; // xmm0_4
  float z; // xmm1_4
  float v14; // xmm0_4
  Moho::CUnitCommandQueue *mCommandQueue; // eax
  Moho::WeakPtr_CUnitCommand *start; // edx
  Moho::Unit *v17; // eax
  int mUnitStates_high; // ecx
  Moho::Unit *v19; // eax
  int v20; // edx
  Wm3::Vector3f *v21; // eax
  float v22; // xmm1_4
  float v23; // xmm3_4
  float v24; // [esp+Ch] [ebp-10h]
  Wm3::Vector3f v25; // [esp+10h] [ebp-Ch] BYREF

  a1->v4 = 0;
  a1->mTargetPosition = *a2;
  v4 = a1->mUnit->mArmy->UseWholeMap(a1->mUnit->mArmy);
  mUnit = a1->mUnit;
  if ( v4 )
  {
    field = mUnit->mSim->mMapData->mHeightField.field;
    x0 = 0;
    z0 = 0;
    x1 = field->width - 1;
    z1 = field->height - 1;
  }
  else
  {
    mMapData = mUnit->mSim->mMapData;
    x0 = mMapData->mPlayableRect.x0;
    z0 = mMapData->mPlayableRect.z0;
    x1 = mMapData->mPlayableRect.x1;
    z1 = mMapData->mPlayableRect.z1;
  }
  x = (float)x0;
  if ( a1->mTargetPosition.x > (float)x0 )
    x = a1->mTargetPosition.x;
  if ( (float)x1 <= x )
    x = (float)x1;
  z = a1->mTargetPosition.z;
  a1->mTargetPosition.x = x;
  v14 = (float)z0;
  if ( z > (float)z0 )
    v14 = z;
  if ( (float)z1 <= v14 )
    v14 = (float)z1;
  a1->mTargetPosition.z = v14;
  if ( a4 )
    a1->mLayer = a4;
  mCommandQueue = mUnit->mCommandQueue;
  if ( !mCommandQueue
    || (start = mCommandQueue->mCommands._Myfirst) == 0
    || !(mCommandQueue->mCommands._Mylast - start)
    || !&ADJ(start->mComm)->Moho::CScriptObject_base
    || &ADJ(start->mComm)->Moho::CScriptObject_base == (Moho::CUnitCommand_as_WeakObject)4 )
  {
    a1->mPreparationTick = mUnit->mSim->mCurTick;
  }
  if ( mUnit->__vftable_unit->GetBlueprint(mUnit)->mAir.mCanFly )
  {
    v17 = a1->mUnit;
    mUnitStates_high = HIDWORD(a1->mUnit->mUnitVarDat.mUnitStates);
    LODWORD(v17->mUnitVarDat.mUnitStates) &= ~0x800u;
    HIDWORD(v17->mUnitVarDat.mUnitStates) = mUnitStates_high;
    v19 = a1->mUnit;
    v20 = HIDWORD(a1->mUnit->mUnitVarDat.mUnitStates);
    LODWORD(v19->mUnitVarDat.mUnitStates) &= ~0x400u;
    HIDWORD(v19->mUnitVarDat.mUnitStates) = v20;
  }
  v25 = *a3;
  if ( !Wm3::Vector3::Compare(&v25, &vec0) || Wm3::Vector3f::Normalize(&v25) == 0.0 )
  {
    if ( a1->mUnit->__vftable_unit->GetBlueprint(a1->mUnit)->mAir.mCanFly )
    {
      v21 = a1->mUnit->__vftable_unit->GetPosition(a1->mUnit);
      v22 = a2->z - v21->z;
      v25.x = a2->x - v21->x;
      v25.z = v22;
      v24 = sqrtf_0((float)(v25.x * v25.x) + (float)(v22 * v22));
      if ( v24 > (double)a1->mUnit->__vftable_unit->GetBlueprint(a1->mUnit)->mAir.mStartTurnDistance )
      {
        if ( v24 == 0.0 )
        {
          a1->mFormationVec.x = 3.4028235e38;
          a1->mFormationVec.y = 3.4028235e38;
          a1->mFormationVec.z = 3.4028235e38;
        }
        else
        {
          v23 = v25.z * (float)(1.0 / v24);
          a1->mFormationVec.x = (float)(1.0 / v24) * v25.x;
          a1->mFormationVec.y = (float)(1.0 / v24) * 0.0;
          a1->mFormationVec.z = v23;
        }
      }
    }
    else
    {
      a1->mFormationVec = *a3;
    }
  }
  else
  {
    a1->mFormationVec = v25;
  }
  if ( a1->mUnit->mOccupation.x0 == a1->mReservation.x0
    && a1->mUnit->mOccupation.z0 == a1->mReservation.z0
    && a1->mUnit->mOccupation.x1 == a1->mReservation.x1
    && a1->mUnit->mOccupation.z1 == a1->mReservation.z1 )
  {
    Moho::Unit::FreeOgridRect(a1->mUnit);
  }
  a1->mReservation.x0 = 0;
  a1->mReservation.z0 = 0;
  a1->mReservation.x1 = 0;
  a1->mReservation.z1 = 0;
}
