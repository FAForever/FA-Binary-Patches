// Address: 0x5D3000
// Function: Moho::CAiSteeringImpl::DriveToNextWaypoint
// Layer 3 - Steering: Drive to next waypoint (ground units)

char __usercall Moho::CAiSteeringImpl::DriveToNextWaypoint@<al>(Moho::CAiSteeringImpl *a1@<eax>)
{
  char result; // al
  bool mNewWaypoint; // cl
  Wm3::Vector3f *v4; // eax
  int mCurWaypoint; // ebx
  Wm3::Vector3f *v6; // eax
  float z; // xmm0_4
  Moho::Unit *mUnit; // eax
  int Val; // eax
  Wm3::Vector3f a2; // [esp+10h] [ebp-18h] BYREF
  Wm3::Vector3f a3; // [esp+1Ch] [ebp-Ch] BYREF

  Moho::Sim::Logf(a1->mUnit->mSim, "  0x%08x->DriveToNextWaypoint()\n", a1->mUnit->mConstDat.mId);
  result = Moho::CAiSteeringImpl::ProcessSplineMovement(a1);
  mNewWaypoint = a1->mNewWaypoint;
  if ( !mNewWaypoint )
  {
    if ( result )
      return 1;
    if ( a1->mPath )
      return 0;
  }
  if ( a1->mCurWaypoint == a1->mPathSize )
    return 1;
  if ( mNewWaypoint )
  {
    v4 = a1->mUnit->__vftable_unit->GetPosition(a1->mUnit);
    mCurWaypoint = a1->mCurWaypoint;
    a2 = *v4;
    a1->destination = *func_TrySnapPosToWaypoint(mCurWaypoint, a1, &a3, &a2, 8.0);
    v6 = &a1->waypoints[a1->mCurWaypoint];
    a1->mNewWaypoint = 0;
    a2.x = v6->x;
    a2.y = v6->y;
    z = v6->z;
    mUnit = a1->mUnit;
    a2.z = z;
    if ( !Moho::Unit::IsAtPosition(mUnit, &a2) )
    {
      Moho::CUnitMotion::SetTarget(&a1->destination, a1->mUnitMotion);
      Val = Moho::CAiSteeringImpl::GetVal(a1);
      Moho::CAiSteeringImpl::UpdatePath(a1, Val, &a1->destination, 1);
      Moho::CAiSteeringImpl::CheckCollisions(a1);
      return Moho::CAiSteeringImpl::ProcessSplineMovement(a1);
    }
    return 1;
  }
  return result;
}
