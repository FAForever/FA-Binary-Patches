// Address: 0x7685A0
// Function: sub_7685A0 (A* WorkOnce - main A* loop iteration)
// Source: c:\work\rts\main\code\src\libs\gpgcore/algorithms/AStarSearch.h

int __stdcall sub_7685A0(Moho::PathQueue::ImplBase *a3, Moho::PathQueue::ImplBase *a2)
{
  int *v2; // ebp
  int v3; // eax
  int v4; // esi
  int v5; // ebx
  float v6; // xmm0_4
  std::vector *v7; // edi
  int v8; // edx
  struct _Iterator_base *Myfirstiter; // eax
  int p_capacity; // edi
  float v11; // xmm0_4
  Moho::TDatListItem_IPathTraveler *mNext; // eax
  Moho::IPathTraveler *p_mNext; // ecx
  double v14; // st7
  float v15; // xmm1_4
  Moho::SOCellPos v16; // eax
  _DWORD *v17; // ebp
  int v18; // eax
  int v19; // esi
  int v20; // eax
  unsigned int v21; // edx
  int v22; // ebx
  int *v23; // eax
  int v24; // ecx
  float v25; // xmm1_4
  float v26; // xmm0_4
  int v27; // ecx
  float v29; // [esp+5F0h] [ebp-70h]
  int v30; // [esp+5F0h] [ebp-70h]
  int v31; // [esp+5F4h] [ebp-6Ch]
  std::vector *a1; // [esp+600h] [ebp-60h] BYREF
  int v33; // [esp+604h] [ebp-5Ch]
  int v34; // [esp+608h] [ebp-58h] BYREF
  float v35; // [esp+60Ch] [ebp-54h]
  float v36; // [esp+610h] [ebp-50h]
  int v37[2]; // [esp+614h] [ebp-4Ch] BYREF
  int v38; // [esp+61Ch] [ebp-44h]
  int v39; // [esp+620h] [ebp-40h]
  int v40; // [esp+624h] [ebp-3Ch]
  int v41; // [esp+628h] [ebp-38h]
  int v42[7]; // [esp+62Ch] [ebp-34h] BYREF
  char v43[8]; // [esp+648h] [ebp-18h] BYREF
  int *v44; // [esp+650h] [ebp-10h] BYREF
  int *v45; // [esp+654h] [ebp-Ch]
  char *v46; // [esp+658h] [ebp-8h]
  char **v47; // [esp+65Ch] [ebp-4h]
  int v48; // [esp+660h] [ebp+0h] BYREF
  char v49; // [esp+CA0h] [ebp+640h] BYREF
  int v50; // [esp+CACh] [ebp+64Ch]

  v2 = &v48;
  v44 = &v48;
  v45 = &v48;
  v46 = &v49;
  v47 = (char **)&v48;
  v50 = 0;
  while ( 1 )
  {
    v3 = a3->v12;
    if ( !v3 || !((a3->v13 - v3) / 12) )
      break;
    v4 = *(_DWORD *)(a3->v12 + 4);
    v31 = v4;
    if ( v2 != (int *)v47 )
    {
      operator delete[](v2);
      v2 = (int *)v47;
      v44 = (int *)v47;
      v46 = *v47;
    }
    v45 = v2;
    v5 = sub_7661C0((Moho::SOCellPos *)v4, a2, (int)&v44);
    if ( v5 )
    {
      if ( v44 != (int *)v47 )
        operator delete[](v44);
      return v5;
    }
    *(_DWORD *)(v4 + 4) = 2;
    sub_7697E0(&a3->v11);
    v2 = v44;
    v33 = ((char *)v45 - (char *)v44) >> 3;
    if ( v33 )
    {
      do
      {
        v6 = *((float *)v2 + 1);
        sub_769560(&a1, v2, a3);
        v7 = a1;
        if ( a1 == (std::vector *)a3->mMap._Mysize )
        {
          v8 = *v2;
          v42[1] = 0;
          v42[2] = 0;
          v42[0] = v8;
          v42[3] = v38;
          v42[4] = v39;
          v42[5] = v40;
          v42[6] = v41;
          v7 = *(std::vector **)sub_7692F0(a3, (int)v43, v42);
          a1 = v7;
        }
        Myfirstiter = v7[1]._Myfirstiter;
        p_capacity = (int)&v7->_Myend;
        if ( Myfirstiter )
        {
          if ( Myfirstiter == (struct _Iterator_base *)1 )
          {
            v25 = *(float *)(v31 + 12) + v6;
            if ( *(float *)(p_capacity + 12) > v25 )
            {
              v26 = *(float *)(p_capacity + 16);
              *(_DWORD *)(p_capacity + 8) = v31;
              *(_DWORD *)p_capacity = *v2;
              v27 = *(_DWORD *)(p_capacity + 20);
              *(float *)(p_capacity + 12) = v25;
              sub_769060(&a3->v11, v27, v26 + v25);
            }
          }
          else if ( Myfirstiter != (struct _Iterator_base *)2 )
          {
            gpg::HandleAssertFailure(
              "neib->mState == CLOSED",
              253,
              "c:\\work\\rts\\main\\code\\src\\libs\\gpgcore/algorithms/AStarSearch.h");
          }
        }
        else
        {
          v11 = *(float *)(v31 + 12) + v6;
          *(_DWORD *)(p_capacity + 4) = 1;
          mNext = a2->mTraveler.mNext;
          v35 = v11;
          if ( mNext )
            p_mNext = (Moho::IPathTraveler *)&mNext[-1].mNext;
          else
            p_mNext = 0;
          v14 = p_mNext->GetSize(p_mNext, (Moho::SOCellPos *)v2);
          v29 = v14;
          if ( a2->mLargestSize > v14 )
          {
            a2->v23 = (Moho::SOCellPos)*v2;
            a2->mLargestSize = v29;
          }
          v15 = v35;
          *(float *)(p_capacity + 16) = v29;
          *(_DWORD *)(p_capacity + 8) = v31;
          v16 = (Moho::SOCellPos)*v2;
          v17 = &a3->v11;
          *(Moho::SOCellPos *)p_capacity = v16;
          *(float *)(p_capacity + 12) = v15;
          v18 = a3->v12;
          v36 = v29 + v15;
          if ( v18 )
            v19 = (a3->v13 - v18) / 12;
          else
            v19 = 0;
          v20 = a3->v20;
          v34 = v19;
          if ( v20 == -1 )
          {
            v21 = a3->v17;
            if ( v21 )
              v30 = (int)(a3->v18 - v21) >> 2;
            else
              v30 = 0;
            v22 = a3->v17;
            if ( v21 )
              v21 = (int)(a3->v18 - v21) >> 2;
            if ( v22 && v21 < (a3->v19 - v22) >> 2 )
            {
              v23 = (int *)a3->v18;
              *v23 = v19;
              a3->v18 = v23 + 1;
            }
            else
            {
              sub_4451A0((int)&a3->v16, (_BYTE *)a3->v18, 1u, (char *)&v34);
            }
          }
          else
          {
            v24 = a3->v17;
            v30 = v20;
            a3->v20 = *(_DWORD *)(v24 + 4 * v20);
            *(_DWORD *)(v24 + 4 * v20) = v19;
          }
          v38 = v30;
          *(float *)v37 = v36;
          v37[1] = p_capacity;
          sub_7698C0(v17, (int)v37);
          sub_769600(v19, (int)v17);
          *(_DWORD *)(p_capacity + 20) = v30;
        }
        v2 = (int *)v47;
        ++v33;
      }
      while ( v33 != LODWORD(v36) );
    }
  }
  if ( v2 != (int *)v47 )
    operator delete[](v2);
  return 0;
}
