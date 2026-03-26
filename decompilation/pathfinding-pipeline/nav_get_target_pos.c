// Address: 0x5AD8B0
// Function: Moho::CAiPathNavigator::GetTargetPos
// Returns the world-space target position for the navigator (either formation leader pos or cell-based target)

Wm3::Vector3f *__usercall Moho::CAiPathNavigator::GetTargetPos@<eax>(
        Wm3::Vector3f *eax0@<eax>,
        Moho::CAiPathNavigator *a2@<ecx>)
{
  Wm3::Vector3f *v4; // ebx
  Moho::Unit *mUnit; // ebx
  __int16 z; // cx
  __int16 x; // ax
  Moho::STIMap *mMapData; // edi
  Moho::SFootprint *Footprint; // eax
  Moho::SOCellPos a1; // [esp+54h] [ebp-4h] BYREF

  if ( a2->v37c && (v4 = &a2->v34, Wm3::Vector3::Compare(&a2->v34, &vec0)) )
  {
    Moho::Sim::Logf(
      a2->mSim,
      "CAiPathNavigator::GetTargetPos() for 0x%08x: following leader: <%.5f,%.5f,%.5f>\n",
      a2->mPathFinder->mUnit->mConstDat.mId,
      v4->x,
      a2->v34.y,
      a2->v34.z);
    gpg::MD5Context::Update(&a2->mSim->mContext, &a2->v34, 0xCu);
    eax0->x = v4->x;
    eax0->y = a2->v34.y;
    eax0->z = a2->v34.z;
    return eax0;
  }
  else
  {
    mUnit = a2->mPathFinder->mUnit;
    Moho::Sim::Logf(
      a2->mSim,
      "CAiPathNavigator::GetTargetPos() for 0x%08x:\n  mTargetPos=<%d,%d>\n",
      mUnit->mConstDat.mId,
      (unsigned __int16)a2->mTargetPos.x,
      (unsigned __int16)a2->mTargetPos.z);
    gpg::MD5Context::Update(&a2->mSim->mContext, &a2->mTargetPos, 4u);
    z = a2->mTargetPos.z;
    x = a2->mTargetPos.x;
    mMapData = mUnit->mSim->mMapData;
    a1.z = z;
    a1.x = x;
    Footprint = Moho::Entity::GetFootprint(&mUnit->Moho::Entity);
    Moho::COORDS_ToWorldPos(
      &a1,
      mMapData,
      eax0,
      (Moho::ELayer)(unsigned __int8)Footprint->mOccupancyCaps,
      Footprint->mSizeX,
      Footprint->mSizeZ);
    return eax0;
  }
}
