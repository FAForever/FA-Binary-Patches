// Address: 0x5D32CC (function starts at 0x5D32B0)
// Function: Moho::CAiSteeringImpl::OnTick
// Layer 3 - Steering: Task tick handler

int __thiscall Moho::CAiSteeringImpl::OnTick(Moho::CAiSteeringImpl_CTask *this)
{
  Moho::Unit *unit; // eax
  Moho::Sim *sim; // esi
  Moho::ELayer layer; // eax
  char Waypoint; // al
  unsigned int curwaypoint; // eax
  Moho::CDebugCanvas *DebugCanvas; // esi
  Moho::CAiPathSpline *path; // eax
  unsigned int v9; // ebp
  unsigned int v10; // edi
  Moho::CAiPathSpline *v11; // edx
  int *v12; // eax
  int v13; // ecx
  Moho::CAiPathSpline *v14; // ecx
  unsigned int pathNode; // eax
  Wm3::Vector3f *v16; // eax
  Moho::Unit *v17; // eax
  _DWORD *v18; // edi
  int v19; // ebp
  Wm3::Vector3f *v20; // ebx
  float *v21; // eax
  int (__thiscall *v22)(int); // eax
  Wm3::Vector3f *v23; // eax
  int v25; // [esp+20h] [ebp-54h]
  float v26; // [esp+20h] [ebp-54h]
  Wm3::Vector3f v27; // [esp+24h] [ebp-50h] BYREF
  Wm3::Vector3f v28; // [esp+30h] [ebp-44h] BYREF
  Wm3::Vector3f v29; // [esp+3Ch] [ebp-38h] BYREF
  Wm3::Vector3f v30; // [esp+48h] [ebp-2Ch] BYREF
  Moho::DebugLine v31; // [esp+54h] [ebp-20h] BYREF

  unit = this->mUnit;
  sim = unit->mSim;
  Moho::Sim::Logf(sim, "0x%08x's steering tick.\n", unit->mConstDat.mId);
  layer = this->mLayer;
  if ( layer == LAYER_Air || layer == LAYER_Orbit )
    Waypoint = Moho::CAiSteeringImpl::FlyToNextWaypoint((Moho::CAiSteeringImpl *)((char *)this - 4));
  else
    Waypoint = Moho::CAiSteeringImpl::DriveToNextWaypoint((Moho::CAiSteeringImpl *)((char *)this - 4));
  if ( Waypoint )
  {
    curwaypoint = this->mCurWaypoint;
    if ( curwaypoint < this->mPathSize )
    {
      this->mCurWaypoint = curwaypoint + 1;
      this->mNewWaypoint = 1;
    }
  }
  if ( Moho::ren_Steering )
  {
    DebugCanvas = Moho::Sim::GetDebugCanvas(sim);
    this->mUnit->__vftable_unit->GetBlueprint(this->mUnit);
    path = this->mPath;
    if ( path )
    {
      v9 = path->mNodes.end - path->mNodes.start;
      v10 = 0;
      if ( v9 )
      {
        v25 = 0;
        do
        {
          v11 = this->mPath;
          if ( v10 < v11->mNumItems )
          {
            v12 = (int *)&v11->mNodes.start[v25];
            if ( v12 )
            {
              v13 = v12[6];
              if ( v13 == 5 || v13 == 6 || v13 == 2 )
              {
                v27.x = 0.0;
                v27.y = 1.0;
                v27.z = 0.0;
                Moho::CDebugCanvas::AddWireCircle(&v27, DebugCanvas, (Wm3::Vector3f *)v12, 0.1, -1.7146522e38, 6u);
              }
              else if ( v13 == 1 || v13 == 3 || v13 == 4 )
              {
                v28.x = 0.0;
                v28.y = 1.0;
                v28.z = 0.0;
                Moho::CDebugCanvas::AddWireCircle(&v28, DebugCanvas, (Wm3::Vector3f *)v12, 0.1, NAN, 6u);
              }
              else if ( v11->v146 == PT_2 )
              {
                v29.x = 0.0;
                v29.y = 1.0;
                v29.z = 0.0;
                Moho::CDebugCanvas::AddWireCircle(&v29, DebugCanvas, (Wm3::Vector3f *)v12, 0.1, NAN, 6u);
              }
              else
              {
                v30.x = 0.0;
                v30.y = 1.0;
                v30.z = 0.0;
                Moho::CDebugCanvas::AddWireCircle(&v30, DebugCanvas, (Wm3::Vector3f *)v12, 0.1, NAN, 6u);
              }
            }
          }
          v25 += 3;
          v10 += 3;
        }
        while ( v10 < v9 );
      }
      v14 = this->mPath;
      pathNode = v14->mPathNode;
      if ( pathNode < v14->mNumItems )
      {
        v16 = &v14->mNodes.start[pathNode].v0;
        if ( v16 )
        {
          v30.x = 0.0;
          v30.y = 1.0;
          v30.z = 0.0;
          Moho::CDebugCanvas::AddWireCircle(&v30, DebugCanvas, v16, 0.30000001, NAN, 8u);
        }
      }
    }
    v17 = this->mColInfo.Moho::CAiSteeringImpl_base::mUnit.Moho::CAiSteeringImpl_base::mUnit;
    if ( v17 )
    {
      v18 = &v17[-1].v207;
      if ( v17 != (Moho::Unit *)4 )
      {
        v19 = (*(int (__thiscall **)(_DWORD *))(*v18 + 28))(v18);
        v20 = this->mUnit->__vftable_unit->GetPosition(this->mUnit);
        v21 = (float *)(*(int (__thiscall **)(_DWORD *))(*v18 + 20))(v18);
        v31.mP0.x = *v21;
        v31.mP0.y = v21[1];
        v31.mP0.z = v21[2];
        v31.mP1.x = v20->x;
        v31.mP1.y = v20->y;
        v31.mP1.z = v20->z;
        v31.mColor0 = -65281;
        v31.mColor1 = -65281;
        Moho::CDebugCanvas::DebugDrawLine(&v31, DebugCanvas);
        v26 = *(float *)(v19 + 172);
        if ( *(float *)(v19 + 180) > v26 )
          v26 = *(float *)(v19 + 180);
        v22 = *(int (__thiscall **)(int))(*v18 + 20);
        v30.x = 0.0;
        v30.y = 1.0;
        v30.z = 0.0;
        v23 = (Wm3::Vector3f *)v22((int)v18);
        Moho::CDebugCanvas::AddWireCircle(&v30, DebugCanvas, v23, v26, NAN, 8u);
      }
    }
  }
  return 1;
}
