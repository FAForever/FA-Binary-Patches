// Address: 0x597800
// Function: sub_597800 (collision resolve between two units)
// Layer 5 - Collision Resolution: Predict and record future collisions

void __usercall sub_597800(
        Moho::CAiSteeringImpl *a1@<eax>,
        Moho::Unit *a2@<edx>,
        Moho::Unit *a3@<ecx>,
        Moho::CAiSteeringImpl *a4)
{
  Wm3::Vector3f *v7; // eax
  Wm3::Vector3f *(__thiscall *GetPosition)(Moho::IUnit *); // edx
  Wm3::Vector3f *v9; // eax
  Wm3::Vector3f *(__thiscall *v10)(Moho::IUnit *); // edx
  Wm3::Vector3f *v11; // eax
  Wm3::Vector3f *(__thiscall *v12)(Moho::IUnit *); // edx
  Moho::CAiPathSpline *v13; // eax
  Moho::CAiPathSpline *v14; // eax
  Moho::IAiSteering_vtbl *v15; // edx
  int v16; // ebp
  Moho::Unit *v17; // eax
  Moho::CFormationInstance *mFormationLayer; // eax
  bool v19; // zf
  int v20; // ebx
  Wm3::Vector3f *v21; // eax
  float v22; // xmm3_4
  float v23; // xmm4_4
  float v24; // xmm5_4
  Wm3::Vector3f *v25; // eax
  Wm3::Vector3f *v26; // eax
  float v27; // xmm0_4
  float v28; // xmm1_4
  float v29; // xmm2_4
  Wm3::Vector3f *v30; // eax
  float v31; // xmm3_4
  float v32; // xmm6_4
  float v33; // xmm5_4
  float v34; // xmm4_4
  Wm3::Vector3f v35; // [esp+10h] [ebp-98h] BYREF
  Moho::CAiPathSpline *v36; // [esp+1Ch] [ebp-8Ch]
  Moho::CAiPathSpline *v37; // [esp+20h] [ebp-88h]
  Wm3::Vector3f v38; // [esp+24h] [ebp-84h] BYREF
  int v39; // [esp+30h] [ebp-78h]
  int v40; // [esp+34h] [ebp-74h]
  float x; // [esp+38h] [ebp-70h]
  float y; // [esp+3Ch] [ebp-6Ch]
  float z; // [esp+40h] [ebp-68h]
  uint v44; // [esp+44h] [ebp-64h]
  unsigned int mCurTick; // [esp+48h] [ebp-60h]
  float v46; // [esp+4Ch] [ebp-5Ch]
  float v47; // [esp+50h] [ebp-58h]
  float v48; // [esp+54h] [ebp-54h]
  uint mPathNode; // [esp+58h] [ebp-50h]
  Wm3::Vector3f v50; // [esp+5Ch] [ebp-4Ch] BYREF
  Wm3::Vector3f v51; // [esp+68h] [ebp-40h] BYREF
  int v52; // [esp+74h] [ebp-34h]
  Wm3::Vector3f v53; // [esp+78h] [ebp-30h] BYREF
  Wm3::Vector3f v54; // [esp+84h] [ebp-24h] BYREF
  char v55[12]; // [esp+90h] [ebp-18h] BYREF
  Wm3::Vector3f v56; // [esp+9Ch] [ebp-Ch] BYREF

  v7 = a2->__vftable_unit->GetPosition(a2);
  x = v7->x;
  y = v7->y;
  GetPosition = a2->__vftable_unit->GetPosition;
  z = v7->z;
  v9 = GetPosition(a2);
  v35.x = v9->x;
  v35.y = v9->y;
  v10 = a3->__vftable_unit->GetPosition;
  v35.z = v9->z;
  v11 = v10(a3);
  v46 = v11->x;
  v47 = v11->y;
  v12 = a3->__vftable_unit->GetPosition;
  v48 = v11->z;
  v38 = *v12(a3);
  mPathNode = 0;
  v44 = 0;
  v39 = 0;
  v37 = 0;
  if ( a4 )
  {
    v13 = a4->GetPath(a4);
    v37 = v13;
    if ( v13 )
      mPathNode = v13->mPathNode;
  }
  v36 = 0;
  if ( a1 )
  {
    v14 = a1->GetPath(a1);
    v36 = v14;
    if ( v14 )
      v44 = v14->mPathNode;
  }
  if ( v37 || v36 )
  {
    v15 = a4->__vftable;
    mCurTick = a2->mSim->mCurTick;
    v16 = ((int (__thiscall *)(Moho::CAiSteeringImpl *))v15->GetColInfo)(a4);
    if ( *(_DWORD *)v16 )
      v17 = (Moho::Unit *)(*(_DWORD *)v16 - 4);
    else
      v17 = 0;
    if ( v17 == a3 )
      sub_596560((Moho::SCollisionInfo *)v16);
    if ( a2->__vftable_unit->IsUnitState(a2, UNITSTATE_Attacking)
      || a3->__vftable_unit->IsUnitState(a3, UNITSTATE_Attacking)
      || (mFormationLayer = a2->mInfo.mFormationLayer) == 0
      || (v19 = mFormationLayer == a3->mInfo.mFormationLayer, LOBYTE(v40) = 1, !v19) )
    {
      LOBYTE(v40) = 0;
    }
    memset(&v51, 0, sizeof(v51));
    memset(&v50, 0, sizeof(v50));
    while ( 1 )
    {
      v20 = v39;
      if ( *(_DWORD *)(v16 + 20) == 1 && *(_DWORD *)(v16 + 24) < v39 + mCurTick )
        break;
      if ( v37 )
      {
        if ( v39 + mPathNode >= v37->mNumItems )
          return;
        v21 = &v37->mNodes.start[v39 + mPathNode].v0;
        if ( !v21 )
          return;
        v22 = v21->x;
        v23 = v21->y;
        v24 = v21->z;
        v35.x = v21->x;
        v35.y = v23;
        v35.z = v24;
        if ( v39 )
        {
          v53.x = v22 - x;
          v53.y = v23 - y;
          v53.z = v24 - z;
          v25 = &v53;
        }
        else
        {
          v25 = a2->GetVelocity(&a2->Moho::Entity, v55);
          v22 = v35.x;
          v23 = v35.y;
          v24 = v35.z;
        }
        v51 = *v25;
        x = v22;
        y = v23;
        z = v24;
      }
      if ( v36 )
      {
        if ( v20 + v44 >= v36->mNumItems )
          return;
        v26 = &v36->mNodes.start[v20 + v44].v0;
        if ( !v26 )
          return;
        v27 = v26->x;
        v28 = v26->y;
        v29 = v26->z;
        v38.x = v26->x;
        v38.y = v28;
        v38.z = v29;
        if ( v20 )
        {
          v54.x = v27 - v46;
          v54.y = v28 - v47;
          v54.z = v29 - v48;
          v30 = &v54;
        }
        else
        {
          v30 = a3->GetVelocity(&a3->Moho::Entity, &v56);
          v27 = v38.x;
          v28 = v38.y;
          v29 = v38.z;
        }
        v50 = *v30;
        v46 = v27;
        v47 = v28;
        v48 = v29;
      }
      if ( func_UnitsWillCollide(&v51, a3, a2, &v35, &v38, &v50, v40) )
      {
        *(float *)&v52 = 9999.0;
        v31 = sub_57D540((int *)v16)
            ? (float)((float)((float)(v35.z - *(float *)(v16 + 16)) * (float)(v35.z - *(float *)(v16 + 16)))
                    + (float)((float)(v35.y - *(float *)(v16 + 12)) * (float)(v35.y - *(float *)(v16 + 12))))
            + (float)((float)(v35.x - *(float *)(v16 + 8)) * (float)(v35.x - *(float *)(v16 + 8)))
            : *(float *)&v52;
        v32 = v38.z;
        v33 = v38.y;
        v34 = v38.x;
        if ( v31 > (float)((float)((float)((float)(v35.z - v38.z) * (float)(v35.z - v38.z))
                                 + (float)((float)(v35.y - v38.y) * (float)(v35.y - v38.y)))
                         + (float)((float)(v35.x - v38.x) * (float)(v35.x - v38.x))) )
        {
          *(_DWORD *)(v16 + 20) = 1;
          *(float *)(v16 + 8) = v34;
          *(float *)(v16 + 12) = v33;
          *(float *)(v16 + 16) = v32;
          Moho::WeakPtr_Unit::Set((Moho::WeakPtr_Unit *)v16, a3);
          *(_DWORD *)(v16 + 24) = v20 + mCurTick;
          return;
        }
      }
      v39 = v20 + 3;
    }
  }
}
