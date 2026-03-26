// Address: 0x5D3140
// Function: Moho::CAiSteeringImpl::FlyToNextWaypoint
// Layer 3 - Steering: Fly to next waypoint (air units)
// Note: IDA detected positive sp value, output may have minor inaccuracies

BOOL __usercall Moho::CAiSteeringImpl::FlyToNextWaypoint@<eax>(Moho::CAiSteeringImpl *a1@<eax>)
{
  Moho::VTransform *mUnit; // eax
  float z; // xmm0_4
  unsigned int mCurWaypoint; // ebx
  float v5; // xmm0_4
  unsigned int mPathSize; // eax
  Wm3::Vector3f *v7; // eax
  Moho::CUnitMotion *mUnitMotion; // esi
  Moho::CSimConVarInstanceBase *SimVar; // eax
  float *v10; // eax
  Wm3::Vector3f a2; // [esp+10h] [ebp-38h] BYREF
  Wm3::Vector3f v13; // [esp+1Ch] [ebp-2Ch] BYREF
  Moho::VTransform a3_8; // [esp+28h] [ebp-20h] BYREF
  float a4_8; // [esp+44h] [ebp-4h]

  mUnit = (Moho::VTransform *)a1->mUnit;
  z = mUnit[5].pos.z;
  mCurWaypoint = a1->mCurWaypoint;
  mUnit = (Moho::VTransform *)((char *)mUnit + 0xA4);
  a3_8.orient.x = z;
  a3_8.orient.y = mUnit->orient.y;
  a3_8.orient.z = mUnit->orient.z;
  a3_8.orient.w = mUnit->orient.w;
  a3_8.pos.x = mUnit->pos.x;
  a3_8.pos.y = mUnit->pos.y;
  v5 = mUnit->pos.z;
  mPathSize = a1->mPathSize;
  a3_8.pos.z = v5;
  if ( mCurWaypoint < mPathSize )
  {
    v7 = func_TrySnapPosToWaypoint(mCurWaypoint, a1, &v13, &a3_8.pos, 10.0);
LABEL_5:
    mUnitMotion = a1->mUnitMotion;
    a2 = *v7;
    memset(&v13, 0, sizeof(v13));
    Moho::CUnitMotion::SetTarget(mUnitMotion, &a2, &v13, LAYER_None);
    SimVar = Moho::Sim::GetSimVar(a1->mUnit->mSim, &SimConVar_ai_SteeringAirTolerance);
    v10 = (float *)SimVar->GetPtr(SimVar);
    return (float)(*v10 * *v10) > (float)((float)((float)(v13.x - a4_8) * (float)(v13.x - a4_8))
                                        + (float)((float)(a2.y - a3_8.pos.y) * (float)(a2.y - a3_8.pos.y)));
  }
  if ( mPathSize )
  {
    v7 = func_TrySnapPosToWaypoint(mPathSize - 1, a1, &v13, &a3_8.pos, 10.0);
    goto LABEL_5;
  }
  return 1;
}
