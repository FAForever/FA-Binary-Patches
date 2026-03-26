// Address: 0x5D3740
// Function: sub_5D3740 (Moho::CAiSteeringImpl::CheckCollisions)
// Layer 3 - Steering: Check for collisions with nearby units

void __stdcall sub_5D3740(Moho::CAiSteeringImpl *this)
{
  Moho::CAiSteeringImpl *v1; // edi
  Moho::RUnitBlueprint *v2; // esi
  Moho::CAiPathSpline *path; // eax
  int v4; // eax
  float SizeX; // xmm4_4
  float MaxAcceleration; // xmm0_4
  float v7; // xmm1_4
  Wm3::Vector3f *v8; // eax
  Moho::Unit *unit; // ecx
  Moho::Entity **v10; // esi
  Moho::Unit *v11; // eax
  Moho::CAiSteeringImpl *steering; // ecx
  Moho::CAiPathSpline *v13; // eax
  Moho::CAiPathSpline *v14; // esi
  Moho::Unit *v15; // ecx
  Moho::Unit *v16; // eax
  gpg::fastvector_n10_Unit *v17; // ecx
  Moho::Unit **v18; // ecx
  unsigned int v19; // ebx
  Moho::Unit **start; // eax
  unsigned int v21; // ebx
  Moho::Unit *v22; // [esp+Ch] [ebp-28Ch] BYREF
  float v23; // [esp+10h] [ebp-288h]
  int v24; // [esp+10h] [ebp-288h] SPLIT
  unsigned int v25; // [esp+14h] [ebp-284h]
  gpg::fastvector_n10_Unit v26; // [esp+18h] [ebp-280h] BYREF
  struct_VecDist v27; // [esp+50h] [ebp-248h] BYREF
  Moho::Unit **v28; // [esp+60h] [ebp-238h] BYREF
  char *v29; // [esp+64h] [ebp-234h]
  int *v30; // [esp+68h] [ebp-230h]
  Moho::Unit **v31; // [esp+6Ch] [ebp-22Ch]
  char v32[40]; // [esp+70h] [ebp-228h] BYREF
  Moho::Entity **v33; // [esp+98h] [ebp-200h] BYREF
  Moho::Entity **v34; // [esp+9Ch] [ebp-1FCh]
  Moho::Entity *v35; // [esp+A0h] [ebp-1F8h]
  Moho::Entity **v36; // [esp+A4h] [ebp-1F4h]
  char v37[480]; // [esp+A8h] [ebp-1F0h] BYREF
  char v38; // [esp+288h] [ebp-10h] BYREF
  int v39; // [esp+294h] [ebp-4h]

  v1 = this;
  if ( !this->mUnit->__vftable_unit->IsDead(this->mUnit)
    && !this->mUnit->__vftable_unit->IsBeingBuilt(this->mUnit)
    && !this->mUnit->__vftable_unit->DestroyQueued(this->mUnit)
    && this->mUnit->mVarDat.mLayer != LAYER_Sub )
  {
    v28 = (Moho::Unit **)v32;
    v29 = v32;
    v30 = (int *)&v33;
    v31 = (Moho::Unit **)v32;
    v26.start = v26.vector;
    v26.finish = v26.vector;
    v26.capacity = (Moho::Unit **)&v27;
    v26.originalvec = v26.vector;
    v33 = (Moho::Entity **)v37;
    v34 = (Moho::Entity **)v37;
    v35 = (Moho::Entity *)&v38;
    v36 = (Moho::Entity **)v37;
    v39 = 2;
    v2 = this->mUnit->__vftable_unit->GetBlueprint(this->mUnit);
    path = this->mPath;
    if ( path )
      v4 = path->mNodes.end - path->mNodes.start;
    else
      v4 = 0;
    SizeX = v2->mSizeX;
    if ( v2->mSizeZ > SizeX )
      SizeX = v2->mSizeZ;
    MaxAcceleration = v2->mPhysics.mMaxAcceleration;
    if ( MaxAcceleration <= 0.0 )
      v7 = 0.0;
    else
      v7 = (float)(v2->mPhysics.mMaxSpeed * v2->mPhysics.mMaxSpeed) / (float)(MaxAcceleration * 2.0);
    v23 = (float)((float)((float)((float)v4 * v2->mPhysics.mMaxSpeed) * 0.1) + v7) + SizeX;
    sub_596560(&this->mColInfo);
    v8 = this->mUnit->__vftable_unit->GetPosition(this->mUnit);
    unit = this->mUnit;
    v27.dir.x = v8->x;
    v27.dir.y = v8->y;
    v27.dir.z = v8->z;
    v27.dist = v23;
    Moho::COGrid::ForAllEntitiesIterator(
      (gpg::fastvector_CollisionResult *)&v33,
      unit->mSim->mOGrid,
      ENTITYTYPE_Unit,
      &v27);
    v10 = v33;
    v24 = 0;
    if ( ((char *)v34 - (char *)v33) / 24 )
    {
      v25 = 0;
      while ( 1 )
      {
        v11 = v10[v25 / 4 + 1]->IsUnit(v10[v25 / 4 + 1]);
        v22 = v11;
        if ( v11 )
          break;
LABEL_29:
        v10 = v33;
        ++v24;
        v25 += 24;
        if ( v24 >= (unsigned int)(((char *)v34 - (char *)v33) / 24) )
          goto LABEL_30;
      }
      if ( func_IsSourceUnit(2, v1->mUnit, v11)
        || (steering = v22->mSteering) == 0
        || (v13 = steering->GetPath(steering), (v14 = v13) != 0) && v13->v146 == PT_2 )
      {
LABEL_28:
        v1 = this;
        goto LABEL_29;
      }
      v15 = v22;
      v16 = this->mUnit;
      if ( v22->mArmy == v16->mArmy )
      {
        if ( !Moho::Unit::IsHigherPriorityThan(v16, v22) )
        {
          v17 = &v26;
LABEL_27:
          sub_5D3E40((int)v17, &v22);
          goto LABEL_28;
        }
        v15 = v22;
      }
      if ( !v14 && !v15->__vftable_unit->GetBlueprint(v15)->mAir.mCanFly )
        goto LABEL_28;
      v17 = (gpg::fastvector_n10_Unit *)&v28;
      goto LABEL_27;
    }
LABEL_30:
    v18 = v28;
    v19 = 0;
    if ( (v29 - (char *)v28) >> 2 )
    {
      do
      {
        sub_597800(v18[v19]->mSteering, v1->mUnit, v18[v19], v1);
        v18 = v28;
        ++v19;
      }
      while ( v19 < (v29 - (char *)v28) >> 2 );
      v10 = v33;
    }
    start = v26.start;
    v21 = 0;
    if ( v26.finish - v26.start )
    {
      do
      {
        sub_597800(v1, start[v21], v1->mUnit, start[v21]->mSteering);
        start = v26.start;
        ++v21;
      }
      while ( v21 < v26.finish - v26.start );
      v18 = v28;
      v10 = v33;
    }
    if ( v10 != v36 )
    {
      operator delete[](v10);
      v10 = v36;
      v18 = v28;
      start = v26.start;
      v33 = v36;
      v35 = *v36;
    }
    v34 = v10;
    if ( start != v26.originalvec )
    {
      operator delete[](start);
      start = v26.originalvec;
      v26.start = v26.originalvec;
      v26.capacity = (Moho::Unit **)*v26.originalvec;
      v18 = v28;
    }
    v26.finish = start;
    if ( v18 != v31 )
      operator delete[](v18);
  }
}
