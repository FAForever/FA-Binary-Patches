#include "moho.h"


#define CURRENT_STRATEGY StrategySafe
#define BATCH_CHANGES_LEVEL 1
#define TRACE_DIFFERENCE false
static_assert(! (TRACE_DIFFERENCE && BATCH_CHANGES_LEVEL == 3), "Overflow bleed in rastering will not be removed in tracing!");

// our reference strategy; slow, but known to work
namespace StrategySafe
{
    template<bool doAdd>
    void RasterCircle(Array2D &grid, Circle2f &circle) {
        float x = circle.Center.x;
        float y = circle.Center.y;
        float radius = circle.Radius;

        int x0 = max(iceil(x - radius), 0);
        int x1 = min(ifloor(x + radius) + 1, grid.mSizeX);
        int y0 = max(iceil(y - radius), 0);
        int y1 = min(ifloor(y + radius) + 1, grid.mSizeY);

        float r2 = radius*radius;
        int index = x0 + y0 * grid.mSizeX;
        int row = grid.mSizeX - (x1 - x0);
        for (int j = y0; j < y1; ++j) {
            float dy = j - y;
            float cmp = r2 - dy*dy;
            for (int i = x0; i < x1; ++i) {
                float dx = i - x;
                if (dx*dx <= cmp) {
                    if (doAdd) {
                        ++grid.mData[index];
                    } else {
                        --grid.mData[index];
                    }
                }
                ++index;
            }
            index += row;
        }
    }

    void MoveCircle(Array2D &grid, Circle2f &prev, Circle2f &cur) {
        float px = prev.Center.x;
        float py = prev.Center.y;
        float pr = prev.Radius;
        float cx = cur.Center.x;
        float cy = cur.Center.y;
        float cr = cur.Radius;

        int x0 = max(iceil(min(px - pr, cx - cr)), 0);
        int x1 = min(ifloor(max(px + pr, cx + cr)) + 1, grid.mSizeX);
        int y0 = max(iceil(min(py - pr, cy - cr)), 0);
        int y1 = min(ifloor(max(py + pr, cy + cr)) + 1, grid.mSizeY);

        float pr2 = pr*pr;
		float cr2 = cr*cr;
        int index = x0 + y0 * grid.mSizeX;
        int row = grid.mSizeX - (x1 - x0);
		for (int j = y0; j < y1; ++j) {
			float pdy = j - py;
			float cdy = j - cy;
			float pcmp = pr2 - pdy*pdy;
			float ccmp = cr2 - cdy*cdy;
			for (int i = x0; i < x1; ++i) {
				double pdx = i - px;
				double cdx = i - cx;
                bool wasIn = pdx*pdx <= pcmp; 
				bool isIn = cdx*cdx <= ccmp;
				if (wasIn != isIn) {
					grid.mData[index] += isIn ? +1 : -1;
				}
                ++index;
			}
            index += row;
		}
    }
};

/*
Strategy: break the circle into an upper half and a lower to handle separately
starting from the diameter and working outwards, keep track of the starting and
ending x positions for the current y level and collapse the line inward to find
the circle boundary until the ends meet; update every cell in the row at once
using these boundaries.

When moving, the halves are combined into a single loop with states so that the
two circle can be interleaved to only trace the difference.

This strategy, as written, has undergone external proving consisting of tens of
billions of test cases. Do not change.
*/
namespace StrategyTrace
{
	struct Inc
	{
		static void chngByte(char &v) {
			++v;
		}
		static void chngInt(uint32_t &v) {
			v += 0x01010101;
		}
	};
	struct Dec
	{
		static void chngByte(char &v) {
			--v;
		}
		static void chngInt(uint32_t &v) {
			v -= 0x01010101;
		}
	};
	struct Upper
	{
		static constexpr bool is_upper = true;
		static void chngY(int &y) {
			--y;
		}
	};
	struct Lower
	{
		static constexpr bool is_upper = false;
		static void chngY(int &y) {
			++y;
		}
	};
	template<class D, class U>
	void RasterHalfCircle(Array2D &grid, Circle2f &circle) {
        float cx = circle.Center.x;
        float cy = circle.Center.y;
        float radius = circle.Radius;
		float r2 = radius * radius;

		int x0 = max(iceil(cx - radius), 0);
		int x1 = min(ifloor(cx + radius), grid.mSizeX - 1);
		int y0 = max(iceil(cy - radius), 0);
		int y1 = min(ifloor(cy + radius) + 1, grid.mSizeY);

		int y = ifloor(cy) + (U::is_upper ? 0 : 1);
		float dy = cy - y;
		float cmp = r2 - dy*dy;
		while (U::is_upper ? (y >= y0) : (y < y1)) {
			float dx0 = cx - x0;
			while (dx0*dx0 > cmp) {
				dx0 = cx - ++x0;
			}
			float dx1 = x1 - cx;
			while (dx1*dx1 > cmp) {
				dx1 = --x1 - cx;
			}
			
#if BATCH_CHANGES_LEVEL == 3
            if (x1 - x0 > 8) {
                int lower = x0;
                for (int align = lower & 3; align != 0; --align) {
                    D::chngByte(grid.at(lower++, y));
                }
                int upper = x1 + 1;
                for (int align = upper & 3; align != 0; --align) {
                    D::chngByte(grid.at(upper--, y));
                }
                for (int x = lower; x < upper; x += 4) {
                    // batching the 4 chars as an int will let the overflow bleed into
                    // the previous byte; since any overflow at all is already an issue,
                    // this doesn't introduce any new problems, as long as it's removed
                    // symmetrically (i.e. tracing must be disabled)
				    D::chngInt((uint32_t &) grid.at(x, y));
                }
            } else {
#elif BATCH_CHANGES_LEVEL != 0
            if (x1 - x0 >= 4 * BATCH_CHANGES_LEVEL) {
                int x = x0;
                switch (x1 - x0 & 4 * BATCH_CHANGES_LEVEL) {
                    while (x <= x1) {
                        case 0: D::chngByte(grid.at(x++, y));
                        case 1: D::chngByte(grid.at(x++, y));
                        case 2: D::chngByte(grid.at(x++, y));
                        case 3: D::chngByte(grid.at(x++, y));
#if BATCH_CHANGES_LEVEL == 2
                        case 4: D::chngByte(grid.at(x++, y));
                        case 5: D::chngByte(grid.at(x++, y));
                        case 6: D::chngByte(grid.at(x++, y));
                        case 7: D::chngByte(grid.at(x++, y));
#endif
                    }
                }
            } else {
#else
                for (int x = x0; x <= x1; ++x) {
                    D::chngByte(grid.at(x, y));
                }
#endif
#if BATCH_CHANGES_LEVEL != 0
            }
#endif

			U::chngY(y);
			dy = cy - y;
			cmp = r2 - dy*dy;
			if (cmp < 1) {
				if (cmp < 0) {
					break;
				}
				while (dx1 > 0) {
					if (dx1*dx1 <= cmp) {
						break;
					}
					dx1 = --x1 - cx;
				}
				if (dx1*dx1 <= cmp) {
					continue;
				}
				break;
			}
		}
	}

    template<bool doAdd>
    void RasterCircle(Array2D &grid, Circle2f &circle) {
        if (doAdd) {
            RasterHalfCircle<Inc, Upper>(grid, circle);
            RasterHalfCircle<Inc, Lower>(grid, circle);
        } else {
            RasterHalfCircle<Dec, Upper>(grid, circle);
            RasterHalfCircle<Dec, Lower>(grid, circle);
        }
    }

    void MoveCircle(Array2D &grid, Circle2f &prev, Circle2f &cur) {
        enum LineState { Early, In, Out, Done };

        float cx = cur.Center.x;
        float cy = cur.Center.y;
        float cr = cur.Radius;
        float cr2 = cr*cr;

		int cx0 = max(iceil(cx - cr), 0);
		int cx1 = min(ifloor(cx + cr), grid.mSizeX - 1);
		int cy0 = max(iceil(cy - cr), 0);
		int cy1 = min(ifloor(cy + cr) + 1, grid.mSizeY);

		int cleft = ifloor(cx);
        int cright = iceil(cx);
        int cline = cy0;
		LineState cstate = cy > cy + 0.5 ? Out : In;
		float cdx = cx - cleft;
		float cdy = cline - cy;
		float ccmp = cr2 - cdy*cdy;
		if (cdx*cdx > ccmp) {
			cdx = cx - cright;
			if (cdx*cdx > ccmp) {
				cdy = cy - ++cline;
				ccmp = cr2 - cdy*cdy;
			} else {
				cleft = cright;
			}
		} else {
			cdx = cx - cright;
			if (cdx*cdx > ccmp) {
				cright = cleft;
			}
		}
		if (ccmp < 0) {
			cstate = Done;
		}
		if (cstate == In) {
			cdx = cx - (cleft - 1);
			while (cdx*cdx <= ccmp) {
				cdx = cx - (--cleft - 1);
			}
			cdx = cright + 1 - cx;
			while (cdx*cdx <= ccmp) {
				cdx = ++cright + 1 - cx;
			}
		}


        float px = prev.Center.x;
        float py = prev.Center.y;
        float pr = prev.Radius;
        float pr2 = pr*pr;

		int px0 = max(iceil(px - pr), 0);
		int px1 = min(ifloor(px + pr), grid.mSizeX - 1);
		int py0 = max(iceil(py - pr), 0);
		int py1 = min(ifloor(py + pr) + 1, grid.mSizeY);

		int pleft = ifloor(px);
        int pright = iceil(px);
        int pline = py0;
		LineState pstate = py > py + 0.5 ? Out : In;
		float pdx = px - pleft;
		float pdy = py - pline;
		float pcmp = pr2 - pdy*pdy;
		if (pdx*pdx > pcmp) {
			pdx = px - pright;
			if (pdx*pdx > pcmp) {
				pdy = py - ++pline;
				pcmp = pr2 - pdy*pdy;
			} else {
				pleft = pright;
			}
		} else {
			pdx = px - pright;
			if (pdx*pdx > pcmp) {
				pright = pleft;
			}
		}
		if (pcmp < 0.0) {
			pstate = Done;
		}
		if (pstate == In) {
			pdx = px - (pleft - 1);
			while (pdx*pdx <= pcmp) {
				pdx = px - (--pleft - 1);
			}
			pdx = pright + 1 - px;
			while (pdx*pdx <= pcmp) {
				pdx = ++pright + 1 - px;
			}
		}
		if (cline < pline) {
			if (pstate != Done) {
				pstate = Early;
			}
		} else if (cline > pline) {
			if (cstate != Done) {
				cstate = Early;
			}
		}
	
		while (cstate != Done || pstate != Done) {
			if (cstate == Out) {
				cdx = cx - (cleft - 1);
				while (cdx*cdx <= ccmp) {
					cdx = cx - (--cleft - 1);
				}
				cdx = cright + 1 - cx;
				while (cdx*cdx <= ccmp) {
					cdx = ++cright + 1 - cx;
				}
			} else if (cstate == In) {
				cdx = cx - cleft;
				while (cdx*cdx > ccmp) {
					cdx = cx - ++cleft;
				}
				cdx = cright - cx;
				while (cdx*cdx > ccmp) {
					cdx = --cright - cx;
				}
			}
			if (pstate == Out) {
				pdx = px - (pleft - 1);
				while (pdx*pdx <= pcmp) {
					pdx = px - (--pleft - 1);
				}
				pdx = pright + 1 - px;
				while (pdx*pdx <= pcmp) {
					pdx = ++pright + 1 - px;
				}
			} else if (pstate == In) {
				pdx = px - pleft;
				while (pdx*pdx > pcmp) {
					pdx = px - ++pleft;
				}
				pdx = pright - px;
				while (pdx*pdx > pcmp) {
					pdx = --pright - px;
				}
			}
			
			if ((cline >= grid.mSizeY || cstate == Done) && (pline >= grid.mSizeY || pstate == Done)) {
				return;
			}
			
			if (cstate == Early || cstate == Done) {
				for (int i = max(pleft, 0); i <= min(pright, grid.mSizeX - 1); ++i) {
					--grid.at(i, pline);
				}
			} else if (pstate == Early || pstate == Done) {
				for (int i = max(cleft, 0); i <= min(cright, grid.mSizeX - 1); ++i) {
					++grid.at(i, cline);
				}
			} else {
				if (cleft < pleft) {
					for (int i = max(cleft, 0); i < min(pleft, grid.mSizeX); ++i) {
						++grid.at(i, cline);
					}
				} else {
					for (int i = max(pleft, 0); i < min(cleft, grid.mSizeX); ++i) {
						--grid.at(i, cline);
					}
				}
				if (cright < pright) {
					for (int i = max(cright + 1, 0); i <= min(pright, grid.mSizeX - 1); ++i) {
						--grid.at(i, cline);
					}
				} else {
					for (int i = max(pright + 1, 0); i <= min(cright, grid.mSizeX - 1); ++i) {
						++grid.at(i, cline);
					}
				}
			}
			if (cstate == Early) {
				if (pline + 1 == cline) {
					cstate = Out;
				}
			} else if (cstate != Done) {
				cdy = cy - ++cline;
				ccmp = cr2 - cdy*cdy;
				if (cstate == Out) {
					if (cdy < 0 && cdx*cdx > ccmp) {
						cdx = cx - cleft + 1;
						if (cdx*cdx > ccmp) {
							cstate = In;
						}
					}
				} else if (ccmp < 1) {
					if (ccmp < 0) {
						cstate = Done;
					} else {
						bool found = false;
						while (cdx > 0) {
							if (cdx*cdx <= ccmp) {
								found = true;
								break;
							}
							cdx = --cright - cx;
						}
						if (cdx*cdx <= ccmp) {
							found = true;
						}
						if (! found) {
							cstate = Done;
						}
					}
				}
			}

			if (pstate == Early) {
				if (pline == cline) {
					pstate = Out;
				}
			} else if (pstate != Done) {
				pdy = py - ++pline;
				pcmp = pr2 - pdy*pdy;
				if (pstate == Out) {
					if (pdy < 0 && pdx*pdx > pcmp) {
						pdx = px - pleft + 1;
						if (pdx*pdx > pcmp) {
							pstate = In;
						}
					}
				} else if (pcmp < 1) {
					if (pcmp < 0) {
						pstate = Done;
					} else {
						bool found = false;
						while (pdx > 0) {
							if (pdx*pdx <= pcmp) {
								found = true;
								break;
							}
							pdx = --pright - px;
						}
						if (pdx*pdx <= pcmp) {
							found = true;
						}
						if (! found) {
							pstate = Done;
						}
					}
				}
			}
        }
    }
};



Circle2f CIntelGrid::ToGridCircle(Vector3f *pos, float radius) {
    return Circle2f(
        pos->x / this->mGridSize,
        pos->z / this->mGridSize,
        // todo: the curvature of a circle makes the rasterer underestimate the
        // area; consider increasing radius to compensate
        radius / this->mGridSize
    );
}

// this templated raster function would have been too cumbersome to include as
// a templated method in `moho.h`; `CIntelCounterHandle_RasterCircle` would have
// been impossible as it relies on `CAiReconDBImpl` being a complete type

// replaces 0x00507540; `radius` is now floating-point
template<bool doAdd>
void CIntelGrid_Raster(CIntelGrid *self, Circle2f &circle) {
    using namespace CURRENT_STRATEGY;

    if (circle.Radius < 2.0) {
        StrategySafe::RasterCircle<doAdd>(self->mGrid, circle);
        return;
    }

    RasterCircle<doAdd>(self->mGrid, circle);
}

// we accommodate moving circles with different radii, but `SetIntelRadius` acts
// instantly, so we only ever see circles with the same radii
void CIntelGrid::Trace(Circle2f &prev, Circle2f &cur) {
    using namespace CURRENT_STRATEGY;

    bool useDef = false;
    if (prev.Radius < 2.0) {
        if (cur.Radius < 2.0) {
            StrategySafe::MoveCircle(this->mGrid, prev, cur);
        } else {
            StrategySafe::RasterCircle<false>(this->mGrid, prev);
            RasterCircle<true>(this->mGrid, cur);
        }
        return;
    } else if (cur.Radius < 2.0) {
        RasterCircle<false>(this->mGrid, prev);
        StrategySafe::RasterCircle<true>(this->mGrid, cur);
        return;
    }
    float dx = prev.Center.x - cur.Center.x;
    float dy = prev.Center.y - cur.Center.y;
    float dr = prev.Radius + cur.Radius;
    if (dx*dx + dy*dy > (0.5 * dr)*(0.5 * dr)) {
        RasterCircle<false>(this->mGrid, prev);
        RasterCircle<true>(this->mGrid, cur);
        return;
    }

    MoveCircle(this->mGrid, prev, cur);
}



// overrides 0x0076E4C0
void CIntel::Update(Vector3f *newPos, int curTick) {
    for (int i = 0; i < 9; ++i) {
        CIntelPosHandle *handle = (&this->mExplorationHandle)[i];
        if (handle != nullptr) {
            handle->UpdatePos(newPos, curTick);
            //handle->mLastTickUpdated = curTick; // no longer used
        }
    }
}



// overrides 0x0076F1E0
void CIntelPosHandle::UpdatePos(Vector3f *newPos, int curTick) {
    if (this->mEnabled) {
        if (newPos->x != this->mLastPos.x || 
            newPos->z != this->mLastPos.z
        ) {
            //this->mLastTickUpdated = curTick; // no longer used
            this->Update(newPos);
        }
    } else {
        // this branch is now a bug
        //this->mLastPos = *newPos;
    }
}

// overrides 0x0076EFC0
void CIntelPosHandle::Update(Vector3f *newPos) {
    // when needed, we will change our approach to a difference-based tracer
    // rather than the shotgun approach here
#if TRACE_DIFFERENCE
    // I don't feel like replacing the vftable to add a new virtual method is worth it
    if (this->vtable == &CIntelCounterHandle::vftable) {
        reinterpret_cast<CIntelCounterHandle *>(this)->TraceCircle(newPos);
    } else {
        this->TraceCircle(newPos);
    }
#else
    this->vtable->SubViz(this);
    // `mLastPos` is now the last position rastered, not the last position checked,
    // so we can't always update it like it originally does
    if (this->mEnabled && this->mRadius != 0.0) {
        this->mLastPos = *newPos;
    }
    this->vtable->AddViz(this);
#endif
}

// overrides 0x0076F180
void CIntelPosHandle::AddViz() {
    if (this->mEnabled && this->mRadius != 0.0 && this->mGrid) {
        Circle2f circle = this->mGrid->ToGridCircle(&this->mLastPos, this->mRadius);
        CIntelGrid_Raster<true>(this->mGrid.get(), circle);
    }
}

// overrides 0x0076F1B0
void CIntelPosHandle::SubViz() {
    if (this->mEnabled && this->mRadius != 0.0 && this->mGrid) {
        Circle2f circle = this->mGrid->ToGridCircle(&this->mLastPos, this->mRadius);
        CIntelGrid_Raster<false>(this->mGrid.get(), circle);
    }
}

void CIntelPosHandle::TraceViz(Vector3f *newPos) {
    if (this->mEnabled && this->mRadius != 0.0 && this->mGrid) {
        Circle2f pCircle = this->mGrid->ToGridCircle(&this->mLastPos, this->mRadius);
        Circle2f cCircle = this->mGrid->ToGridCircle(newPos, this->mRadius);
        this->mGrid->Trace(pCircle, cCircle);
        this->mLastPos = *newPos;
    }
}



template<bool doAdd>
void CIntelCounterHandle_RasterCircle(CIntelCounterHandle *self) {
    if (! self->mEnabled || self->mRadius == 0.0) {
        return;
    }
    size_t gridOffset;
    switch (self->mType) {
        case INTELCOUNTER_RadarStealthField: {
            gridOffset = offsetof(CAiReconDBImpl, mRCIGrid);
            break;
        }
        case INTELCOUNTER_SonarStealthField: {
            gridOffset = offsetof(CAiReconDBImpl, mSCIGrid);
            break;
        }
        case INTELCOUNTER_CloakField: {
            gridOffset = offsetof(CAiReconDBImpl, mVCIGrid);
            break;
        }
        default: {
            WarningF(doAdd ? s_addVizError : s_subVizError, self->mType);
            return; // this return makes it suboptimal to wrap the offset code
            // in an inlined function with our compiler
        }
    }
    // recall that `mGrid` is null for counter-intel handles
    auto myGrid = (CIntelGrid *) ((char *) self->mReconDB + gridOffset);
    Circle2f circle = myGrid->ToGridCircle(&self->mLastPos, self->mRadius);
    auto &armies = self->mSim->mArmiesList;
    size_t end = armies.size();
    for (int i = 0; i < end; ++i) {
        CAiReconDBImpl *reconDB = armies[i]->mReconDB;
        if (reconDB != self->mReconDB) {
            auto grid = (CIntelGrid *) ((char *) reconDB + gridOffset);
            CIntelGrid_Raster<doAdd>(grid, circle);
        }
    }
}

// same as above, except for four lines
void CIntelCounterHandle::TraceViz(Vector3f *newPos) {
    if (! this->mEnabled || this->mRadius == 0.0) {
        return;
    }
    size_t gridOffset;
    switch (this->mType) {
        case INTELCOUNTER_RadarStealthField: {
            gridOffset = offsetof(CAiReconDBImpl, mRCIGrid);
            break;
        }
        case INTELCOUNTER_SonarStealthField: {
            gridOffset = offsetof(CAiReconDBImpl, mSCIGrid);
            break;
        }
        case INTELCOUNTER_CloakField: {
            gridOffset = offsetof(CAiReconDBImpl, mVCIGrid);
            break;
        }
        default: {
            WarningF("Moho::CIntelCounterHandle::TraceViz: unsupported counter intel type %i", this->mType); // line diff 1 (changed)
            return;
        }
    }
    auto myGrid = (CIntelGrid *) ((char *) this->mReconDB + gridOffset);
    Circle2f pCircle = myGrid->ToGridCircle(&this->mLastPos, this->mRadius);
    Circle2f cCircle = myGrid->ToGridCircle(newPos, this->mRadius); // line diff 2 (added)
    auto &armies = this->mSim->mArmiesList;
    size_t end = armies.size();
    for (int i = 0; i < end; ++i) {
        CAiReconDBImpl *reconDB = armies[i]->mReconDB;
        if (reconDB != this->mReconDB) {
            auto grid = (CIntelGrid *) ((char *) reconDB + gridOffset);
            grid->Trace(pCircle, cCircle); // line diff 3 (changed)
        }
    }
    this->mLastPos = *newPos; // line diff 4 (added)
}

// overrides 0x0076F5D0
void CIntelCounterHandle::AddViz() {
    CIntelCounterHandle_RasterCircle<true>(this);
}

// overrides 0x0076F720
void CIntelCounterHandle::SubViz() {
    CIntelCounterHandle_RasterCircle<false>(this);
}



// overrides 0x0068E728 (patch didn't fit)
void cfunc_EntitySetIntelRadius_patch() {
    asm(R"(
        fstp    dword ptr [esp+0x8]
        movss   xmm0, dword ptr [esp+0x8]
        xorps   xmm1, xmm1
        comiss  xmm0, xmm1
        jg      SKIP_SET_ZERO
        movss   xmm0, xmm1
    SKIP_SET_ZERO:
        movd    ebx, xmm0
        jmp     0x0068E734
    )");
}

// easier to rewrite than to patch:

// overrides 0x005BE9E8
void __stdcall ReconBlip_ctr_patch(ReconBlip *self) {
    auto unit = self->mCreator;
    auto bp = unit->vtable->GetBlueprint(unit);
    auto rand = self->mSim->mRandom;
    float min = bp->mIntel.mJamRadius.mMin;
    float max = bp->mIntel.mJamRadius.mMax;
    float scale = rand->FRand(min, max); // this is the affected line
    Vector3f dir(rand->FRandGaussian(), 0.0, rand->FRandGaussian());
    dir = dir.normalized();
    self->mJamOffset = dir * scale;
}

// overrides 0x008BFD70
bool UserUnit::GetWaterIntel(
    /*out*/ float &sonarRange,
    /*out*/ float &waterRange,
    /*out*/ float &radarRange
) {
    if ((this->GetToggleCaps() & RULEUTC_IntelToggle) != 0
        && (this->mUnitVarDat.mScriptbits & RULEUTC_IntelToggle) != 0)
    {
        return false;
    }
    auto &attr = this->mVarDat.mAttributes;
    // GetRange now returns a float
    sonarRange = attr.GetRange(ENTATTR_OmniRange);
    waterRange = attr.GetRange(ENTATTR_RadarRange);
    radarRange = attr.GetRange(ENTATTR_SonarRange);
    if (sonarRange <= 0.0 && waterRange <= 0.0 && radarRange <= 0.0) {
        return false;
    }
    return true;
}

// overrides 0x008BFE50
bool UserUnit::GetMaxCounterIntel(/*out*/ float &range) {
    auto tcaps = this->GetToggleCaps();
    if ((tcaps & RULEUTC_JammingToggle) != 0 && (this->mUnitVarDat.mScriptbits & RULEUTC_JammingToggle) != 0
        || (tcaps & RULEUTC_StealthToggle) != 0 && (this->mUnitVarDat.mScriptbits & RULEUTC_StealthToggle) != 0)
    {
        return false;
    }
    auto &attr = this->mVarDat.mAttributes;
    auto bp = this->IUnit::vtable->GetBlueprint(this);
    // GetRange now returns a float
    float cloakRange = attr.GetRange(ENTATTR_CloakRange);
    float sonarRange = attr.GetRange(ENTATTR_SonarStealthRange);
    float radarRange = attr.GetRange(ENTATTR_RadarStealthRange);
    float maxRange = max(bp->mIntel.mJamRadius.mMax, bp->mIntel.mSpoofRadius.mMax);
    if (maxRange < cloakRange)
        maxRange = cloakRange;
    if (maxRange < sonarRange)
        maxRange = sonarRange;
    if (maxRange < radarRange)
        maxRange = radarRange;
    range = maxRange;
    return maxRange > 0.0;
}
