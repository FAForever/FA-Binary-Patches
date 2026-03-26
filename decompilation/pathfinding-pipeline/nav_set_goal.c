// Address: 0x5AA120
// Function: sub_5AA120 (Navigator Set Goal)
// Copies the navigation goal to the pathfinder and validates goal cells for reachability

char sub_5AA120(Moho::SNavGoal *eax0, Moho::CAiPathFinder *a2)
{
  Moho::SNavGoal *p_mGoal; // ebp
  Moho::SFootprint *Footprint; // eax
  bool v4; // zf
  int v5; // ecx
  float mMinWaterDepth; // edx
  float mMaxWaterDepth; // eax
  int x0; // eax
  bool v9; // cc
  int j; // esi
  Moho::COGrid *mOccupation; // ebp
  Moho::STIMap *mMapData; // edx
  Moho::EOccupancyCaps v13; // al
  Moho::SOCellPos v1; // [esp+10h] [ebp-1Ch] BYREF
  int i; // [esp+14h] [ebp-18h]
  Moho::SFootprint a1; // [esp+18h] [ebp-14h] BYREF

  p_mGoal = &a2->mGoal;
  qmemcpy(&a2->mGoal, eax0, sizeof(a2->mGoal));
  Footprint = Moho::Entity::GetFootprint(&a2->mUnit->Moho::Entity);
  v4 = !a2->v5c;
  v5 = *(_DWORD *)&Footprint->mSizeX;
  a1.mMaxSlope = Footprint->mMaxSlope;
  mMinWaterDepth = Footprint->mMinWaterDepth;
  mMaxWaterDepth = Footprint->mMaxWaterDepth;
  *(_DWORD *)&a1.mSizeX = v5;
  a1.mMinWaterDepth = mMinWaterDepth;
  a1.mMaxWaterDepth = mMaxWaterDepth;
  if ( !v4 )
    a1.mFlags = HIBYTE(v5) | 1;
  x0 = p_mGoal->mPos1.x0;
  v9 = p_mGoal->mPos1.x0 <= a2->mGoal.mPos1.x1;
  for ( i = p_mGoal->mPos1.x0; v9; i = x0 )
  {
    for ( j = a2->mGoal.mPos1.z0; j <= a2->mGoal.mPos1.z1; ++j )
    {
      if ( x0 == a2->mGoal.mPos1.x0 || x0 == a2->mGoal.mPos1.x1 || j == a2->mGoal.mPos1.z0 || j == a2->mGoal.mPos1.z1 )
      {
        mOccupation = a2->mOccupation;
        mMapData = mOccupation->mSim->mMapData;
        v1.x = x0;
        v1.z = j;
        v13 = Moho::OCCUPY_MobileCheck(&a1, mMapData, &v1);
        if ( a2->mUnit->mVarDat.mLayer == LAYER_Water )
          v13 &= ~4u;
        LOBYTE(x0) = Moho::OCCUPY_FootprintFits(mOccupation, &v1, &a1, v13);
        if ( x0 )
        {
          a2->mHasPath = 1;
          return x0;
        }
        x0 = i;
      }
    }
    v9 = ++x0 <= a2->mGoal.mPos1.x1;
  }
  a2->mHasPath = 0;
  return x0;
}
