// Address: 0x5AA310
// Function: sub_5AA310 (Navigator Enqueue Path)
// Enqueues a pathfinding request into the PathQueue, sets up search bounds and occupancy checks

_DWORD *__usercall sub_5AA310@<eax>(Moho::CAiPathFinder *a1@<eax>)
{
  Moho::PathQueue::Impl *mQueue; // ecx
  Moho::TDatListItem_IPathTraveler *mPrev; // edx
  Moho::TDatListItem_IPathTraveler *v4; // eax
  Moho::TDatListItem_IPathTraveler *mHeight; // edx
  Moho::Unit *mUnit; // edx
  char v7; // al
  Moho::Unit *v8; // ecx
  float mSize; // xmm0_4
  Moho::STIMap *mMapData; // edi
  Wm3::Vector3f *(__thiscall *GetPosition)(Moho::IUnit *); // eax
  float *v12; // eax
  int x0; // ecx
  int z0; // edx
  int x1; // ebx
  int z1; // edi
  bool v17; // al
  Moho::Entity *v18; // ecx
  Moho::SFootprint *Footprint; // edi
  Wm3::Vector3f *v20; // eax
  int v21; // ebp
  __int16 x; // bx
  Moho::Entity *v23; // ecx
  Moho::SFootprint *v24; // eax
  int v25; // eax
  bool v26; // zf
  Moho::CHeightField *field; // ecx
  int v28; // edx
  int v29; // edi
  int v30; // eax
  int v31; // ebx
  int v32; // ecx
  int v33; // edx
  int v34; // edx
  _List_nod_Rect2i::_Node *Myhead; // ecx
  _List_nod_Rect2i::_Node *Prev; // eax
  int *v37; // eax
  int v38; // edi
  std::list_Rect2i *p_mList; // ebx
  int *v40; // esi
  _DWORD *result; // eax
  Moho::SOCellPos a2; // [esp+18h] [ebp-1Ch] BYREF
  float v43; // [esp+1Ch] [ebp-18h]
  gpg::Rect2i a1a; // [esp+20h] [ebp-14h] BYREF

  mQueue = a1->mPathQueue->mQueue;
  mPrev = a1->v1.mPrev;
  v4 = &a1->v1;
  mPrev->mNext = v4->mNext;
  v4->mNext->mPrev = v4->mPrev;
  v4->mPrev = v4;
  v4->mNext = v4;
  mHeight = (Moho::TDatListItem_IPathTraveler *)mQueue->mHeight;
  mQueue = (Moho::PathQueue::Impl *)((char *)mQueue + 4);
  v4->mPrev = mHeight;
  v4->mNext = (Moho::TDatListItem_IPathTraveler *)mQueue;
  mQueue->mSize = (int)v4;
  v4->mPrev->mNext = v4;
  mUnit = a1->mUnit;
  a1->v5b = 1;
  v7 = mUnit->mArmy->UseWholeMap(mUnit->mArmy);
  v8 = a1->mUnit;
  mSize = (float)(int)a1->mSize;
  a1->mUseWholeMap = v7;
  mMapData = a1->mSim->mMapData;
  GetPosition = v8->__vftable_unit->GetPosition;
  *(float *)&a2 = mSize;
  v12 = (float *)GetPosition(v8);
  x0 = mMapData->mPlayableRect.x0;
  z0 = mMapData->mPlayableRect.z0;
  x1 = mMapData->mPlayableRect.x1;
  z1 = mMapData->mPlayableRect.z1;
  v17 = (float)x0 <= (float)(*v12 - mSize)
     && (float)z0 <= (float)(v12[2] - mSize)
     && (float)(*v12 + mSize) <= (float)x1
     && (float)(v12[2] + mSize) <= (float)z1;
  v18 = &a1->mUnit->Moho::Entity;
  a1->mIsInMap = v17;
  a1->v25b = 0;
  Footprint = Moho::Entity::GetFootprint(v18);
  v20 = a1->mUnit->__vftable_unit->GetPosition(a1->mUnit);
  *(float *)&a2 = v20->z - (float)((float)Footprint->mSizeZ * 0.5);
  v21 = (int)a2;
  v43 = v20->x - (float)((float)Footprint->mSizeX * 0.5);
  a2 = (Moho::SOCellPos)(int)v43;
  x = a2.x;
  v23 = &a1->mUnit->Moho::Entity;
  a2.z = v21;
  v24 = Moho::Entity::GetFootprint(v23);
  LOBYTE(v25) = Moho::OCCUPY_FootprintFits(a1->mOccupation, &a2, v24, OC_ANY);
  v26 = a1->mOGrid2 == 0;
  a1->v25a = v25 != 0;
  if ( v26 )
    return sub_5AAF60((int)&a1->mList);
  field = a1->mSim->mMapData->mHeightField.field;
  v28 = x;
  v29 = x - 8;
  v30 = field->width - 1;
  v31 = 0;
  v32 = field->height - 1;
  if ( v29 > 0 )
    v31 = v29;
  v33 = v28 + 8;
  a1a.x0 = v31;
  a1a.x1 = v30 - 1;
  if ( v30 - 1 >= v33 )
    a1a.x1 = v33;
  v34 = 0;
  if ( (__int16)v21 - 8 > 0 )
    v34 = (__int16)v21 - 8;
  a1a.z0 = v34;
  a1a.z1 = v32 - 1;
  if ( v32 - 1 >= (__int16)v21 + 8 )
    a1a.z1 = (__int16)v21 + 8;
  if ( a1->mList._Mysize > 2 )
  {
    Myhead = a1->mList._Myhead;
    Prev = Myhead->_Prev;
    if ( Prev != Myhead )
    {
      Prev->_Prev->_Next = Prev->_Next;
      Prev->_Next->_Prev = Prev->_Prev;
      operator delete(Prev);
      --a1->mList._Mysize;
    }
  }
  v37 = (int *)a1->mList._Myhead;
  v38 = *v37;
  p_mList = &a1->mList;
  v40 = sub_5AB710(&a1a, *v37, *(_DWORD *)(*v37 + 4));
  result = (_DWORD *)sub_5AB760(p_mList);
  *(_DWORD *)(v38 + 4) = v40;
  *(_DWORD *)v40[1] = v40;
  return result;
}
