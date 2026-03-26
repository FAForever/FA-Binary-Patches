// Address: 0x596560
// Function: sub_596560 (collision info clear/reset)
// Layer 5 - Collision Resolution: Clear collision info structure

void sub_596560(Moho::SCollisionInfo *result)
{
  Moho::Unit *unit; // ecx

  unit = result->mUnit.mUnit;
  if ( result->mUnit.mUnit )
  {
    for ( ; (Moho::SCollisionInfo *)unit->__vftable_unit != result; unit = (Moho::Unit *)&unit->__vftable_unit->IsUnit2 )
      ;
    unit->__vftable_unit = (Moho::Unit_vtbl *)result->mUnit.mNext.pi_;
    result->mUnit.mUnit = 0;
    result->mUnit.mNext.pi_ = 0;
  }
  result->mPos = vec0;
  result->mType = 0;
  result->mVal = -1;
}
