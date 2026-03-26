// Address: 0x596F30
// Function: func_ResolvePossibleCollision
// Layer 5 - Collision Resolution: Resolve a detected possible collision

char __usercall func_ResolvePossibleCollision@<al>(_DWORD *eax0@<eax>, Moho::Unit *a2)
{
  Moho::Unit *v2; // edi
  int *v3; // eax
  Moho::Unit_vtbl *iunit_vtable; // edx
  float v5; // xmm0_4
  Wm3::Vector3f *(__thiscall *GetPosition)(Moho::Unit *); // eax
  int v7; // eax
  Wm3::Vector3f *(__thiscall *GetVelocity)(Moho::Entity *, Wm3::Vector3f *); // edx
  deprecated_Moho::Unit_base_Entity *v9; // esi
  float *v10; // eax
  int v11; // eax
  Moho::CUnitMotion *unitMotion; // esi
  Wm3::Vector3f *v13; // eax
  Moho::CAiSteeringImpl *steering; // edi
  void **v15; // esi
  Wm3::Vector3f *v16; // eax
  Moho::CAiPathSpline *v18; // eax
  int *v19; // eax
  Moho::CAiSteeringImpl *v20; // ecx
  Moho::CAiPathSpline *v21; // eax
  int v22; // eax
  float *v23; // eax
  int v24; // eax
  Moho::RUnitBlueprint *v25; // esi
  int v26; // eax
  float SizeX; // xmm1_4
  float v28; // xmm0_4
  Moho::VTransform *v29; // eax
  float y; // xmm1_4
  float z; // xmm2_4
  float w; // xmm0_4
  float x; // xmm3_4
  int (__thiscall *v34)(_DWORD); // eax
  float *v35; // esi
  Wm3::Vector3f *v36; // eax
  Wm3::Vector3f *v37; // edi
  _DWORD *v38; // edi
  void (__thiscall **v39)(_DWORD *, int, int); // esi
  int v40; // eax
  _DWORD *v41; // edi
  void (__thiscall **v42)(_DWORD *, int, int); // esi
  int v43; // eax
  Wm3::Vector3f *v44; // edi
  float *v45; // eax
  float v46; // xmm2_4
  float v47; // xmm0_4
  float v48; // xmm5_4
  float v49; // xmm4_4
  float v50; // xmm1_4
  float v51; // xmm0_4
  float v52; // xmm2_4
  float v53; // xmm0_4
  float v54; // xmm1_4
  float v55; // xmm3_4
  Wm3::Vector3f *v56; // eax
  int *v57; // eax
  float v58; // xmm1_4
  float v59; // xmm2_4
  float v60; // xmm0_4
  Wm3::Vector3f *p_z; // ebx
  Wm3::Quaternionf *v62; // ecx
  Moho::RUnitBlueprint *(__thiscall *GetBlueprint)(Moho::IUnit *); // eax
  Moho::RUnitBlueprint *v64; // edi
  Moho::CAiSteeringImpl *v65; // edi
  void **v66; // esi
  Wm3::Vector3f *v67; // eax
  bool v68; // [esp+4Fh] [ebp-59h]
  Moho::Sim *sim; // [esp+50h] [ebp-58h]
  Moho::SCoordsVec2 a1; // [esp+58h] [ebp-50h] BYREF
  int v71; // [esp+60h] [ebp-48h] BYREF
  float v72; // [esp+64h] [ebp-44h]
  float v73; // [esp+68h] [ebp-40h]
  Wm3::Vector3f v74; // [esp+6Ch] [ebp-3Ch] BYREF
  float v75; // [esp+78h] [ebp-30h]
  Wm3::Vector3f v76; // [esp+7Ch] [ebp-2Ch] BYREF
  float v77; // [esp+88h] [ebp-20h]
  float v78; // [esp+8Ch] [ebp-1Ch]
  Wm3::Vector3f v79; // [esp+90h] [ebp-18h] BYREF
  float v80; // [esp+9Ch] [ebp-Ch] BYREF
  float v81; // [esp+A0h] [ebp-8h] BYREF
  float v82; // [esp+A4h] [ebp-4h]

  if ( *eax0 )
    v2 = (Moho::Unit *)(*eax0 - 4);
  else
    v2 = 0;
  if ( !a2 || !v2 || !a2->mSteering )
    return 0;
  v3 = (int *)a2->__vftable_unit->GetPosition(a2);
  iunit_vtable = v2->__vftable_unit;
  v71 = *v3;
  v72 = *((float *)v3 + 1);
  v5 = *((float *)v3 + 2);
  GetPosition = (Wm3::Vector3f *(__thiscall *)(Moho::Unit *))iunit_vtable->GetPosition;
  v73 = v5;
  v7 = (int)GetPosition(v2);
  GetVelocity = a2->GetVelocity;
  v75 = *(float *)v7;
  v76.x = *(float *)(v7 + 4);
  v9 = (deprecated_Moho::Unit_base_Entity *)&a2->Moho::Entity;
  v76.y = *(float *)(v7 + 8);
  v10 = (float *)GetVelocity(&a2->Moho::Entity, (Wm3::Vector3f *)&v80);
  v68 = (float)((float)((float)(*v10 * *v10) + (float)(v10[1] * v10[1])) + (float)(v10[2] * v10[2])) > 0.0;
  if ( v2->__vftable_unit->GetBlueprint(v2)->mAir.mCanFly && v2->mUnitMotion )
  {
    if ( !v2->mTransport )
    {
      v2->GetVelocity(&v2->Moho::Entity, (Wm3::Vector3f *)&v80);
      v11 = ((int (__thiscall *)(Moho::Entity *))v9->GetVelocity)(&a2->Moho::Entity);
      if ( (unsigned __int8)sub_596E00((int)&v71, v11, (int)&v79.y) )
      {
        unitMotion = v2->mUnitMotion;
        v13 = v2->__vftable_unit->GetPosition(v2);
        Moho::CUnitMotion::SetTarget(unitMotion, v13, &vec0, LAYER_Air);
      }
    }
LABEL_12:
    steering = a2->mSteering;
    v15 = &steering->SetCol;
    v16 = a2->__vftable_unit->GetPosition(a2);
    ((void (__thiscall *)(Moho::CAiSteeringImpl *, _DWORD, Wm3::Vector3f *))*v15)(steering, 0, v16);
    return 0;
  }
  v18 = a2->mSteering->GetPath(a2->mSteering);
  if ( v18 )
  {
    v19 = (int *)sub_5965E0(v18->mPathNode, v18);
    if ( v19 )
    {
      v71 = *v19;
      v72 = *((float *)v19 + 1);
      v73 = *((float *)v19 + 2);
    }
  }
  v20 = v2->mSteering;
  if ( v20 )
  {
    v21 = v20->GetPath(v20);
    if ( v21 )
    {
      v22 = sub_5965E0(v21->mPathNode, v21);
      if ( v22 )
      {
        v75 = *(float *)v22;
        v76.x = *(float *)(v22 + 4);
        v76.y = *(float *)(v22 + 8);
      }
    }
  }
  sim = a2->mSim;
  Moho::Sim::Logf(
    sim,
    "  ResolvePossibleCollision(0x%08x @ <%.2f,%.2f,%2.f>, 0x%08x @ <%.2f,%.2f,%2.f>)\n",
    a2->mConstDat.mId,
    *(float *)&v71,
    v72,
    v73,
    v2->mConstDat.mId,
    v75,
    v76.x,
    v76.y);
  v23 = (float *)v2->GetVelocity(&v2->Moho::Entity, &v80);
  if ( sqrtf((float)((float)(*v23 * *v23) + (float)(v23[1] * v23[1])) + (float)(v23[2] * v23[2])) <= 0.0 )
    goto LABEL_12;
  v2->GetVelocity(&v2->Moho::Entity, (Wm3::Vector3f *)&v80);
  v24 = ((int (__thiscall *)(Moho::Entity *))v9->GetVelocity)(&a2->Moho::Entity);
  if ( !(unsigned __int8)sub_596E00((int)&v71, v24, (int)&v79.y) )
    goto LABEL_12;
  Moho::Sim::Logf(sim, "    collide.\n");
  v25 = a2->__vftable_unit->GetBlueprint(a2);
  v26 = (int)v2->__vftable_unit->GetBlueprint(v2);
  SizeX = v25->mSizeX;
  if ( v25->mSizeZ > SizeX )
    SizeX = v25->mSizeZ;
  v28 = *(float *)(v26 + 172);
  if ( *(float *)(v26 + 180) > v28 )
    v28 = *(float *)(v26 + 180);
  a1.x = (float)(v28 + SizeX) + 0.5;
  Moho::Unit::IsSameFormationLayerWith(a2, v2);
  v29 = a2->__vftable_unit->GetTransform(a2);
  y = v29->orient.y;
  z = v29->orient.z;
  w = v29->orient.w;
  x = v29->orient.x;
  v34 = *(int (__thiscall **)(_DWORD))(*(_DWORD *)LODWORD(a1.x) + 20);
  v78 = (float)((float)(w * z) - (float)(x * y)) * 2.0;
  v77 = (float)((float)(x * z) + (float)(w * y)) * 2.0;
  v79.x = 1.0 - (float)((float)((float)(z * z) + (float)(y * y)) * 2.0);
  v35 = (float *)v34(LODWORD(a1.x));
  v36 = a2->__vftable_unit->GetPosition(a2);
  v79.y = v36->x - *v35;
  v79.z = v36->y - v35[1];
  v80 = v36->z - v35[2];
  Wm3::Vector3::Normalize((Wm3::Vector3f *)&v79.y, &v76);
  v37 = (Wm3::Vector3f *)(*(int (__thiscall **)(int, float *))(*(_DWORD *)(LODWORD(a1.x) + 8) + 60))(
                           LODWORD(a1.x) + 8,
                           &v81);
  Wm3::Vector3::Normalize(v37, &v74);
  if ( (float)((float)((float)(v76.x * v74.y) + (float)(v76.y * v74.z)) + (float)(v75 * v74.x)) <= 0.70700002 )
  {
    Moho::Sim::Logf(sim, "    not within cone.\n");
    v65 = a2->mSteering;
    v66 = &v65->SetCol;
    v67 = a2->__vftable_unit->GetPosition(a2);
    ((void (__thiscall *)(Moho::CAiSteeringImpl *, int, Wm3::Vector3f *))*v66)(v65, 4, v67);
    return 1;
  }
  Moho::Sim::Logf(sim, "    within cone.\n");
  v38 = *(_DWORD **)(LODWORD(a1.x) + 1360);
  if ( v38 && !(_BYTE)a2 )
  {
    if ( *(_BYTE *)(LODWORD(a1.x) + 1674) )
    {
      if ( a2->mIsNaval )
      {
        v39 = (void (__thiscall **)(_DWORD *, int, int))(*v38 + 24);
        v40 = (*(int (__thiscall **)(_DWORD))(*(_DWORD *)LODWORD(a1.x) + 20))(LODWORD(a1.x));
        (*v39)(v38, 5, v40);
      }
    }
    else if ( (Moho::Entity::GetFootprint((Moho::Entity *)(LODWORD(a1.x) + 8))->mFlags & 1) == 0
           || (Moho::Entity::GetFootprint(&a2->Moho::Entity)->mFlags & 1) != 0 )
    {
      v41 = *(_DWORD **)(LODWORD(a1.x) + 1360);
      v42 = (void (__thiscall **)(_DWORD *, int, int))(*v41 + 24);
      v43 = (*(int (__thiscall **)(_DWORD))(*(_DWORD *)LODWORD(a1.x) + 20))(LODWORD(a1.x));
      (*v42)(v41, 5, v43);
    }
  }
  if ( !v68 || (float)((float)((float)(v76.x * v77) + (float)(v76.y * v78)) + (float)(v75 * v76.z)) >= 0.0 )
  {
    if ( (float)((float)(v79.z * v74.x) - (float)(v74.z * v79.x)) <= 0.0 )
    {
      if ( (_BYTE)a2 )
      {
        p_z = &v79;
        v62 = &stru_10AE288;
        goto LABEL_56;
      }
      v62 = &stru_10AE278;
    }
    else
    {
      if ( (_BYTE)a2 )
      {
        p_z = &v79;
        v62 = &stru_10AE1DC;
LABEL_56:
        v56 = Moho::MultQuadVec(p_z, &v74, v62);
        goto LABEL_57;
      }
      v62 = &stru_10AE1CC;
    }
    p_z = (Wm3::Vector3f *)&v76.z;
    goto LABEL_56;
  }
  v44 = a2->GetVelocity(&a2->Moho::Entity, &v79);
  Wm3::Vector3::Normalize(v44, (Wm3::Vector3f *)&v76.z);
  v45 = (float *)(*(int (__thiscall **)(_DWORD))(*(_DWORD *)LODWORD(a1.x) + 24))(LODWORD(a1.x));
  v46 = v45[2];
  v47 = v45[3];
  v48 = v45[1];
  v49 = 1.0 - (float)((float)((float)(v47 * v47) + (float)(v46 * v46)) * 2.0);
  v50 = *v45 * v47;
  v51 = (float)(v47 * v48) - (float)(*v45 * v46);
  v52 = (float)((float)(v77 * v74.y) + (float)(v78 * v74.z)) + (float)(v76.z * v74.x);
  v53 = v51 * 2.0;
  v54 = (float)(v50 + (float)(v45[2] * v48)) * 2.0;
  v55 = (float)((float)(v54 * v76.x) + (float)(v53 * v76.y)) + (float)(v49 * v75);
  if ( v52 <= 0.70700002 )
  {
    if ( v52 < -0.70700002 )
    {
      if ( v55 <= 0.0 )
      {
        v75 = -0.0 - v49;
        v76.x = -0.0 - v54;
        v76.y = -0.0 - v53;
      }
      else
      {
        v75 = v49;
        v76.x = v54;
        v76.y = v53;
      }
      v57 = (int *)(*(int (__thiscall **)(_DWORD))(*(_DWORD *)LODWORD(a1.x) + 20))(LODWORD(a1.x));
      v58 = v76.x;
      v59 = v76.y;
      v71 = *v57;
      v72 = *((float *)v57 + 1);
      v73 = *((float *)v57 + 2);
      v60 = v75;
      goto LABEL_58;
    }
    v79.x = v76.z + v75;
    v79.y = v77 + v76.x;
    v79.z = v78 + v76.y;
    v56 = Wm3::Vector3::Normalize(&v79, (Wm3::Vector3f *)&v76.z);
  }
  else
  {
    if ( v55 <= 0.0 )
    {
      v53 = -0.0 - v53;
      v49 = -0.0 - v49;
      v54 = -0.0 - v54;
    }
    v79.x = v49 + v75;
    v79.y = v76.x + v54;
    v79.z = v76.y + v53;
    v56 = Wm3::Vector3::Normalize(&v79, (Wm3::Vector3f *)&v76.z);
  }
LABEL_57:
  v59 = v56->z;
  v58 = v56->y;
  v60 = v56->x;
LABEL_58:
  GetBlueprint = a2->__vftable_unit->GetBlueprint;
  v80 = (float)(v60 * a1.x) + *(float *)&v71;
  v81 = (float)(v58 * a1.x) + v72;
  v82 = (float)(v59 * a1.x) + v73;
  a1.x = v80;
  a1.z = v82;
  v64 = GetBlueprint(a2);
  if ( !func_TryBuildStructureAt(&a1, v64, sim, 1, 0, 0, 0) )
    return 0;
  ((void (__thiscall *)(Moho::CAiSteeringImpl *, int, float *))a2->mSteering->SetCol)(a2->mSteering, 2, &v80);
  return 1;
}
