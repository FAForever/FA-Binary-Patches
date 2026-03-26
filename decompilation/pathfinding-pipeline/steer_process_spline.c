// Address: 0x5D2C00
// Function: Moho::CAiSteeringImpl::ProcessSplineMovement
// Layer 3 - Steering: Process spline movement

char __usercall Moho::CAiSteeringImpl::ProcessSplineMovement@<al>(Moho::CAiSteeringImpl *this@<eax>)
{
  Moho::Unit *mUnit; // eax
  Moho::Sim *mSim; // ebp
  Moho::RUnitBlueprintPhysics *p_mPhysics; // esi
  Wm3::Vector3f *v5; // eax
  Moho::CUnitMotion *mUnitMotion; // eax
  Moho::IAiNavigator *mNavigator; // ecx
  Wm3::Vector3f *v8; // eax
  Wm3::Vector3f *p_destination; // esi
  int v10; // ebx
  void (__thiscall *Stop)(Moho::CAiSteeringImpl *, int); // eax
  Moho::CAiPathSpline *mPath; // eax
  Wm3::Vector3f *v14; // eax
  Moho::Unit *v15; // ecx
  int v16; // ebx
  Moho::ECollisionType mType; // eax
  Moho::IAiNavigator *v18; // ecx
  Wm3::Vector3f *v19; // eax
  Moho::CAiPathSpline *v20; // eax
  int v21; // eax
  Moho::CAiPathSpline *v22; // eax
  unsigned int pathNode; // ecx
  int v24; // edx
  unsigned int v25; // ecx
  int v26; // ecx
  Moho::CUnitMotion *v27; // eax
  char val; // [esp+8h] [ebp-20h]
  float v29[3]; // [esp+1Ch] [ebp-Ch] BYREF

  mUnit = this->mUnit;
  mSim = mUnit->mSim;
  Moho::Sim::Logf(mSim, "  0x%08x->ProcessSplineMovement()\n", mUnit->mConstDat.mId);
  if ( this->mUnitMotion->mPushed )
  {
    Moho::Sim::Logf(mSim, "    pushed!\n");
    if ( this->mPath )
      this->Stop(this, 0);
    p_mPhysics = &this->mUnit->__vftable_unit->GetBlueprint(this->mUnit)->mPhysics;
    v5 = this->mUnit->GetVelocity(&this->mUnit->Moho::Entity, v29);
    if ( p_mPhysics->mMaxSpeed * 0.0099999998 <= sqrtf(
                                                    (float)((float)(v5->x * v5->x) + (float)(v5->y * v5->y))
                                                  + (float)(v5->z * v5->z)) )
      goto LABEL_11;
    this->mUnitMotion->mPushed = 0;
    mUnitMotion = this->mUnitMotion;
    if ( !mUnitMotion->v35c )
      goto LABEL_11;
    mUnitMotion->v35c = 0;
    mNavigator = this->mUnit->mNavigator;
    if ( mNavigator )
      mNavigator->Func1(mNavigator);
    v8 = Moho::Invalid<Wm3::Vector3<float>>();
    p_destination = &this->destination;
    if ( !Wm3::Vector3::Compare(&this->destination, v8) )
      goto LABEL_11;
    val = 1;
  }
  else
  {
    mPath = this->mPath;
    if ( !mPath || (unsigned int)(mPath->mPathNode + 1) < mPath->mNumItems )
      goto LABEL_11;
    v14 = Moho::Invalid<Wm3::Vector3<float>>();
    p_destination = &this->destination;
    if ( !Wm3::Vector3::Compare(&this->destination, v14)
      || (this->mUnit->__vftable_unit->GetPosition(this->mUnit),
          Moho::Unit::IsAtPosition(this->mUnit, &this->destination)) )
    {
      this->Stop(this, 0);
      return 1;
    }
    val = 0;
  }
  v10 = Moho::CAiSteeringImpl::GetVal(this);
  Moho::CAiSteeringImpl::UpdatePath(this, v10, p_destination, val);
  Moho::CAiSteeringImpl::CheckCollisions(this);
LABEL_11:
  if ( !this->v40 )
  {
    if ( this->mUnit->__vftable_unit->IsUnitState(this->mUnit, UNITSTATE_Immobile)
      || this->mUnit->mUnitVarDat.mStunTicks )
    {
      Stop = this->Stop;
      this->v40 = 1;
      Stop(this, 0);
      return 0;
    }
    if ( !this->v40 )
      goto LABEL_30;
  }
  if ( !this->mUnit->__vftable_unit->IsUnitState(this->mUnit, UNITSTATE_Immobile) )
  {
    v15 = this->mUnit;
    if ( !v15->mUnitVarDat.mStunTicks )
    {
      if ( v15->__vftable_unit->IsUnitState(v15, UNITSTATE_Moving)
        || this->mUnit->__vftable_unit->IsUnitState(this->mUnit, UNITSTATE_Patrolling)
        || this->mUnit->__vftable_unit->IsUnitState(this->mUnit, UNITSTATE_Attacking) )
      {
        v16 = Moho::CAiSteeringImpl::GetVal(this);
        Moho::CAiSteeringImpl::UpdatePath(this, v16, &this->destination, 1);
        Moho::CAiSteeringImpl::CheckCollisions(this);
      }
      this->v40 = 0;
      goto LABEL_30;
    }
  }
  if ( !this->v40 )
  {
LABEL_30:
    mType = this->mColInfo.mType;
    if ( mType == (COLTYPE_4|COLTYPE_1) )
    {
LABEL_31:
      Moho::CAiSteeringImpl::UpdatePath(this, 4, &this->destination, 1);
    }
    else if ( mType == COLTYPE_1 && mSim->mCurTick >= this->mColInfo.mVal )
    {
      func_ResolvePossibleCollision(
        &this->mColInfo.Moho::CAiSteeringImpl_CTask::Moho::CAiSteeringImpl_base::mUnit.Moho::CAiSteeringImpl_CTask::Moho::CAiSteeringImpl_base::mUnit,
        this->mUnit);
      switch ( this->mColInfo.mType )
      {
        case COLTYPE_0:
          v19 = Moho::Invalid<Wm3::Vector3<float>>();
          if ( Wm3::Vector3::Compare(&this->destination, v19) )
          {
            v20 = this->mPath;
            if ( !v20 || v20->mPathNode >= (unsigned int)v20->mNumItems )
            {
              v21 = Moho::CAiSteeringImpl::GetVal(this);
              Moho::CAiSteeringImpl::UpdatePath(this, v21, &this->destination, 1);
              Moho::CUnitMotion::SetTarget(&this->destination, this->mUnitMotion);
            }
            Moho::CAiSteeringImpl::CheckCollisions(this);
          }
          break;
        case COLTYPE_2:
          Moho::CUnitMotion::SetTarget(&this->v33, this->mUnitMotion);
          Moho::CAiSteeringImpl::UpdatePath(this, 2, &this->v33, 1);
          this->v33 = vec0;
          break;
        case COLTYPE_3:
          v18 = this->mUnit->mNavigator;
          if ( v18 )
            v18->Func1(v18);
          break;
        case COLTYPE_4:
          goto LABEL_31;
        default:
          break;
      }
    }
    v22 = this->mPath;
    if ( v22 )
    {
      pathNode = v22->mPathNode;
      if ( pathNode >= v22->mNumItems )
        v24 = 0;
      else
        v24 = (int)&v22->mNodes.start[pathNode];
      v25 = v22->mPathNode + 1;
      if ( v25 >= v22->mNumItems )
        v26 = 0;
      else
        v26 = (int)&v22->mNodes.start[v25];
      v27 = this->mUnitMotion;
      v27->mNextWaypoint = (Moho::CPathPoint *)v24;
      v27->v2 = (Moho::CPathPoint *)v26;
      if ( v24 )
        ++this->mPath->mPathNode;
    }
  }
  return 0;
}
