// Address: 0x766350
// Function: sub_766350 (A* Neighbor Expansion - expands neighbors for A* search)
// Source: AStarSearch / HAStar cluster-based pathfinding

char __stdcall sub_766350(Moho::PathQueue::ImplBase *a1, Moho::SOCellPos *a2, int a3, _DWORD *a4)
{
  Moho::TDatListItem_IPathTraveler *mNext; // ebx
  int v5; // ebp
  int p_mNext; // ebx
  int x; // edi
  int z; // eax
  int v9; // esi
  int v10; // esi
  __int16 v11; // di
  float v12; // xmm0_4
  unsigned __int8 (__thiscall *v13)(int, unsigned int *); // edx
  float *v14; // eax
  bool v15; // zf
  gpg::HaStar::ClusterMap *mClusterMap; // ecx
  int v18; // ebp
  int v19; // edx
  int v20; // edi
  int v21; // ebx
  gpg::HaStar::ClusterMap *v22; // ecx
  gpg::HaStar::Cluster::Data *mData; // esi
  unsigned int v24; // edi
  int mNodeCount; // ebp
  gpg::HaStar::Cluster::Node *mNodes; // eax
  unsigned int v27; // ebx
  gpg::HaStar::Cluster::Node *v28; // edx
  unsigned int v29; // eax
  int v30; // ecx
  int v31; // esi
  int v32; // ebp
  int (__thiscall **v33)(int, int); // edi
  int v34; // eax
  char v35; // al
  unsigned int v36; // eax
  int v37; // ecx
  double v38; // st7
  unsigned __int8 (__thiscall *v39)(int, Moho::SOCellPos *, _WORD *, float *); // eax
  float *v40; // ecx
  unsigned __int8 v41; // [esp+15h] [ebp-79h]
  char v42; // [esp+15h] [ebp-79h]
  gpg::HaStar::Cluster::Data *v43; // [esp+16h] [ebp-78h] BYREF
  int v44; // [esp+1Ah] [ebp-74h]
  unsigned int v45; // [esp+1Eh] [ebp-70h] BYREF
  int v46; // [esp+22h] [ebp-6Ch]
  float v47; // [esp+26h] [ebp-68h] BYREF
  int v48; // [esp+2Ah] [ebp-64h]
  int v49; // [esp+2Eh] [ebp-60h]
  int v50; // [esp+32h] [ebp-5Ch]
  int v51; // [esp+36h] [ebp-58h]
  int v52; // [esp+3Ah] [ebp-54h]
  _WORD v53[2]; // [esp+3Eh] [ebp-50h] BYREF
  int v54; // [esp+42h] [ebp-4Ch]
  float v55; // [esp+46h] [ebp-48h] BYREF
  gpg::HaStar::Cluster::Node *v56; // [esp+4Ah] [ebp-44h]
  int v57; // [esp+4Eh] [ebp-40h]
  int mNumlevels; // [esp+52h] [ebp-3Ch] BYREF
  float v59; // [esp+56h] [ebp-38h]
  int v60; // [esp+5Ah] [ebp-34h] BYREF
  float v61; // [esp+5Eh] [ebp-30h]
  int v62; // [esp+62h] [ebp-2Ch] BYREF
  int v63; // [esp+66h] [ebp-28h]
  int v64; // [esp+6Ah] [ebp-24h]
  int v65; // [esp+6Eh] [ebp-20h]
  char v66[16]; // [esp+72h] [ebp-1Ch] BYREF
  int v67; // [esp+8Ah] [ebp-4h]

  mNext = a1->mTraveler.mNext;
  v5 = 0;
  if ( mNext )
  {
    p_mNext = (int)&mNext[-1].mNext;
    v44 = p_mNext;
  }
  else
  {
    v44 = 0;
    p_mNext = 0;
  }
  x = (unsigned __int16)a2->x;
  z = (unsigned __int16)a2->z;
  v9 = a3;
  v50 = x;
  v51 = z;
  if ( !a3 )
  {
    v41 = 0;
    while ( 1 )
    {
      if ( (v41 & byte_E35E94[v5]) == byte_E35E94[v5] )
      {
        v10 = x + byte_E35E84[v5];
        v11 = v51 + byte_E35E8C[v5];
        sub_8E33B0(&a1->mClusterMap->mNumlevels, (int)v66, v10, v51 + byte_E35E8C[v5], 1);
        if ( (*(unsigned __int8 (__thiscall **)(int, char *))(*(_DWORD *)p_mNext + 28))(p_mNext, v66) )
        {
          v12 = flt_E35E9C[v5];
          v13 = *(unsigned __int8 (__thiscall **)(int, unsigned int *))(*(_DWORD *)p_mNext + 4);
          LOWORD(v45) = v10;
          HIWORD(v45) = v11;
          v47 = v12;
          if ( v13(p_mNext, &v45) )
          {
            if ( (*(unsigned __int8 (__thiscall **)(int, Moho::SOCellPos *, unsigned int *, float *))(*(_DWORD *)p_mNext + 8))(
                   p_mNext,
                   a2,
                   &v45,
                   &v47) )
            {
              HIWORD(v48) = v11;
              v14 = (float *)a4[1];
              v15 = v14 == (float *)a4[2];
              LOWORD(v48) = v10;
              mNumlevels = v48;
              v59 = v47;
              if ( v15 )
              {
                sub_767370(a4, (int)v14, (int)&mNumlevels, (int)&v60);
              }
              else
              {
                if ( v14 )
                {
                  *(_DWORD *)v14 = v48;
                  v14[1] = v59;
                }
                a4[1] += 8;
              }
              v41 |= 1 << v5;
            }
          }
        }
      }
      if ( ++v5 >= 8 )
        break;
      x = v50;
    }
    return 1;
  }
  mClusterMap = a1->mClusterMap;
  mNumlevels = mClusterMap->mNumlevels;
  v49 = gpg::HaStar::sClusterSize_Log2[a3];
  gpg::HaStar::ClusterMap::ClusterIndexRect(mClusterMap, (int)&v62, x, z, a3);
  v18 = v62;
  v48 = v62;
  if ( v62 >= v64 )
    return 1;
  v19 = v65;
  v20 = v63;
  while ( 1 )
  {
    v47 = *(float *)&v20;
    v52 = v18 << v49;
    if ( v20 < v19 )
      break;
LABEL_73:
    v48 = ++v18;
    if ( v18 >= v64 )
      return 1;
  }
  while ( 1 )
  {
    v21 = v20 << v49;
    v22 = a1->mClusterMap;
    v57 = v20 << v49;
    if ( !gpg::HaStar::ClusterMap::WorkOnCluster(v22, v18, v20, v9, &a1->mBudget) )
      return 0;
    mData = a1->mClusterMap->mLevels[v9].mArray[v18 + v20 * a1->mClusterMap->mLevels[v9].mWidth].mData;
    v24 = 0;
    v43 = mData;
    if ( mData )
      ++mData->mRefs;
    v67 = 0;
    if ( mData )
      mNodeCount = mData->mNodeCount;
    else
      mNodeCount = 0;
    if ( mNodeCount )
    {
      mNodes = (gpg::HaStar::Cluster::Node *)&mData->mNodes[0].z;
      while ( mNodes[-1].z != (_BYTE)v50 - (_BYTE)v52 || mNodes->x != (_BYTE)v51 - (_BYTE)v21 )
      {
        ++v24;
        ++mNodes;
        if ( v24 == mNodeCount )
          goto LABEL_66;
      }
      v45 = v24;
      if ( v24 != -1 )
      {
        v27 = 0;
        v46 = mData ? mData->mNodeCount : 0;
        if ( v46 > 0 )
        {
          v28 = (gpg::HaStar::Cluster::Node *)&mData->mNodes[0].z;
          v56 = (gpg::HaStar::Cluster::Node *)&mData->mNodes[0].z;
          do
          {
            if ( v24 != v27 )
            {
              v29 = v24 >= v27 ? v27 + ((v24 * (v24 - 1)) >> 1) : v24 + ((v27 * (v27 - 1)) >> 1);
              v30 = mData ? mData->mNodeCount : 0;
              if ( *((char *)&mData->mNodes[v30].x + v29) >= 0 )
              {
                v31 = v52 + v28[-1].z;
                v32 = v57 + v28->x;
                if ( a3 == mNumlevels
                  || (v33 = (int (__thiscall **)(int, int))(*(_DWORD *)v44 + 28),
                      v34 = sub_8E33B0(&a1->mClusterMap->mNumlevels, (int)v66, v31, v32, a3 + 1),
                      v35 = (*v33)(v44, v34),
                      v24 = v45,
                      v35) )
                {
                  v36 = v24 >= v27 ? v27 + ((v24 * (v24 - 1)) >> 1) : v24 + ((v27 * (v27 - 1)) >> 1);
                  v37 = v43 ? v43->mNodeCount : 0;
                  v42 = *(&v43->mNodes[v37].x + v36);
                  v38 = sub_7658D0(&v43, v24, v27);
                  v39 = *(unsigned __int8 (__thiscall **)(int, Moho::SOCellPos *, _WORD *, float *))(*(_DWORD *)v44 + 8);
                  v55 = v38 * `gpg::HaStar::Cluster::DequantizeEdgeCost'::`2'::lut[v42];
                  v53[0] = v31;
                  v53[1] = v32;
                  if ( v39(v44, a2, v53, &v55) )
                  {
                    v40 = (float *)a4[1];
                    v15 = v40 == (float *)a4[2];
                    LOWORD(v54) = v31;
                    HIWORD(v54) = v32;
                    v60 = v54;
                    v61 = v55;
                    if ( v15 )
                    {
                      sub_767370(a4, (int)v40, (int)&v60, (int)&v62);
                      v24 = v45;
                    }
                    else
                    {
                      if ( v40 )
                      {
                        *(_DWORD *)v40 = v54;
                        v40[1] = v61;
                      }
                      a4[1] += 8;
                    }
                  }
                }
                mData = v43;
              }
            }
            ++v27;
            v28 = ++v56;
          }
          while ( (int)v27 < v46 );
        }
      }
    }
LABEL_66:
    v67 = -1;
    if ( mData )
    {
      v15 = mData->mRefs-- == 1;
      if ( v15 )
      {
        if ( mData->mPtr )
          (**(void (__thiscall ***)(dtr_vtbl **, _DWORD))mData->mPtr)(mData->mPtr, mData->v2);
        operator delete[](mData);
      }
    }
    v19 = v65;
    v18 = v48;
    v9 = a3;
    ++LODWORD(v47);
    if ( SLODWORD(v47) >= v65 )
    {
      v20 = v63;
      goto LABEL_73;
    }
    v20 = LODWORD(v47);
  }
}
