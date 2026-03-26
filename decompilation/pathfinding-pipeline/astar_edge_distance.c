// Address: 0x7658D0
// Function: sub_7658D0 (A* Edge Distance - computes octile distance between cluster nodes)
// Used by HAStar cluster-based pathfinding for edge cost calculation

double __thiscall sub_7658D0(_DWORD *this, int a2, int a3)
{
  long double v3; // st7
  long double v4; // st6
  float v6; // [esp+8h] [ebp-8h]
  float v8; // [esp+Ch] [ebp-4h]

  v3 = fabs((double)(*(unsigned __int8 *)(*this + 2 * a3 + 13) - *(unsigned __int8 *)(*this + 2 * a2 + 13)));
  v8 = v3;
  v4 = fabs((double)(*(unsigned __int8 *)(*this + 2 * a3 + 14) - *(unsigned __int8 *)(*this + 2 * a2 + 14)));
  v6 = v4;
  if ( v4 <= v3 )
    return (float)((float)(v6 * 0.41421354) + v8);
  else
    return (float)((float)(v8 * 0.41421354) + v6);
}
