

asm(R"(

	Vector3f__Normalize = 0x0044F7E0
	CHeightField__Intersection = 0x00476F30
	Vector3f__IsntNaN = 0x005657F0
	CColHitResult__Calculate = 0x00577540
	CScriptObject__Callback = 0x00581AA0
	Entity__SetCurrentLayer = 0x0067AFF0
	strOnExitWater  = 0x0E29544
	strOnEnterWater = 0x0E29550
	one             = 0x0DFEC20
	two             = 0x0DFEB0C
	n10000          = 0x0E4F6E4
	onePercent      = 0x0E4F950
	NaN             = 0x10A7CB0
	pInf            = 0x10B5484
	nInf            = 0x10B5488

	IMPACT_Terrain = 1
	IMPACT_Water = 2
	LAYER_Water = 8
	LAYER_Air = 16

	mid             = 0
	pos             = 0
	distance        = 0
	x               = 0
	y               = 4
	z               = 8
	elevation       = 0xC
	norm_dir        = 0xC
	dir             = 0xC
	radius          = 0x18
	min             = 0x18
	max             = 0x1C
	impactInterp    = 0x2A4
	collideSurface  = 0x2A8
	destroyOnWater  = 0x2AF
	impactPosition  = 0x31C
	belowWater      = 0x334
	impactType      = 0x364
	hasWater        = 0x1534
	waterLevel      = 0x1538

#Moho::Projectile::CheckCollision at 0x69D1D0
	layerCallback   = esp+0x230-0x221 # repurpose `wentUnderwater`
	distMoved       = -0x220
	waterHitDist    = esp+0x230+distMoved # overlap & restore later
	curPos          = -0x21C
	curZ            = -0x218
	curX            = -0x20C
	mapData         = -0x208
	varWaterLevel   = -0x204
	curY            = -0x200
	dif             = -0x1FC
	segment         = -0x1B8
	hitPos          = -0x1F0
	waterHitRes     = -0x1F0
	hitResult       = -0x1DC
	normDif         = -0x1DC
	waterPlane      = -0x1CC
	colhitRes       = -0x19C
	this            = 8

	LAYERCALLBACK_None = 0
	LAYERCALLBACK_OnExitWater = 1
	LAYERCALLBACK_OnEnterWater = 2


# Move the `OnExitWater` / `OnEnterWater` layer change callbacks after terrain
# collisions so that they can be unscheduled when the projectile ends up never
# hitting the water.
.section h0; .set h0,0x69D258 #Moho::Projectile::CheckCollision+0x88
	# there are a few optimizations while we're at it
	mov     [esp+0x230+mapData], eax # move this up now that we can jump earlier
	# store a default value for the water level; the function needs it later for
	# a different check we aren't jumping early from
	movss   xmm1, dword ptr [n10000]
	movss   [esp+0x230+varWaterLevel], xmm1
	movss   xmm1, [esp+0x230+segment+radius]
	mulss   xmm1, dword ptr [two]
	# about to be overlapped, but we could still break
	movss   [esp+0x230+distMoved], xmm1
	# move the no collision check earlier
	cmp     byte ptr [edi+collideSurface], 0
	mov     byte ptr [layerCallback], LAYERCALLBACK_None
	jz      short CONTINUE_FUNCTION
	# skip water checks instead of using -10,000 if there's no water
	cmp     byte ptr [eax+hasWater], 0
	movss   xmm1, dword ptr [eax+waterLevel]
	jz      short TERRAIN_CHECK
	mov     dword ptr [waterHitDist], 0 # overlap
	movss   [esp+0x230+varWaterLevel], xmm1
	cmp     byte ptr [edi+belowWater], 0
	movss   xmm2, dword ptr [ebx+4]
	jz      short WAS_UNDERWATER
	comiss  xmm2, xmm1
	jbe     short TERRAIN_CHECK
	# move layer callbacks to the end, but still schedule them while we're here
	# because that's going to be easier than moving the terrain check before
	# the water check
	mov     byte ptr [layerCallback], LAYERCALLBACK_OnEnterWater
	jmp     short WATER_CHECK
WAS_UNDERWATER:
	comiss  xmm1, xmm2
	jbe     short TERRAIN_CHECK
	mov     byte ptr [layerCallback], LAYERCALLBACK_OnExitWater
WATER_CHECK:
	# move `destroyOnWater` check to get water distance so we can get torpedoes
	# to still hit the ground through the water
	# ideally, we'd only also do this when we know there's also a terrain
	# collision to adjudicate (in addition to projectiles that get destroyed on
	# the water), but we don't have that information yet 
	xorps   xmm0, xmm0
	mov     eax, [esp+0x230+curPos]
	movss   xmm2, dword ptr [one]
	movss   xmm3, dword ptr [eax+z]
	movss   [esp+0x230+waterPlane+x], xmm0
	movss   [esp+0x230+waterPlane+z], xmm0
	movss   xmm0, dword ptr [NaN]
	movss   [esp+0x230+waterPlane+elevation], xmm1
	movss   xmm1, dword ptr [eax+x]
	movss   [esp+0x230+waterHitRes+distance], xmm0
	movss   xmm0, dword ptr [ebx+x]
	movss   [esp+0x230+waterPlane+y], xmm2
	movss   xmm2, dword ptr [eax+y]
	subss   xmm0, xmm1
	movss   [esp+0x230+curX], xmm1
	movss   xmm1, dword ptr [ebx+y]
	subss   xmm1, xmm2
	movss   [esp+0x230+curY], xmm2
	movss   xmm2, dword ptr [ebx+z]
	subss   xmm2, xmm3
	lea     edi, [esp+0x230+dif]
	lea     esi, [esp+0x230+normDif]
	movss   [esp+0x230+curZ], xmm3
	movss   [esp+0x230+dif+x], xmm0
	movss   [esp+0x230+dif+y], xmm1
	movss   [esp+0x230+dif+z], xmm2
	call    Vector3f__Normalize
	movss   xmm0, [esp+0x230+curX]
	movss   xmm1, [esp+0x230+curY]
	movss   xmm2, [esp+0x230+curZ]
	movss   xmm3, dword ptr [eax+x]
	movss   xmm4, dword ptr [eax+y]
	movss   xmm5, dword ptr [eax+z]
	movss   [esp+0x230+colhitRes+pos+x], xmm0
	movss   xmm0, dword ptr [nInf]
	movss   [esp+0x230+colhitRes+min], xmm0
	movss   xmm0, dword ptr [pInf]
	lea     edi, [esp+0x230+waterHitRes]
	lea     ecx, [esp+0x230+colhitRes]
	lea     edx, [esp+0x230+waterPlane]
	lea     esi, [esp+0x230+dif]
	movss   [esp+0x230+colhitRes+pos+y], xmm1
	movss   [esp+0x230+colhitRes+pos+z], xmm2
	movss   [esp+0x230+colhitRes+dir+x], xmm3
	movss   [esp+0x230+colhitRes+dir+y], xmm4
	movss   [esp+0x230+colhitRes+dir+z], xmm5
	movss   [esp+0x230+colhitRes+max], xmm0
	call    CColHitResult__Calculate
	call    Vector3f__IsntNaN
	mov     edi, [ebp+this] # move this up so that it's guaranteed to be reset
	test    al, al
	# we can replace `eax` with `edi` up till TERRAIN_CHECK now that it
	# contains `this`
	jz      short WATER_CHECK_COLLISION
	# store water distance and restore `destroyOnWater` check
	cmp     byte ptr [edi+destroyOnWater], 0
	movss   xmm0, [esp+0x230+waterHitRes+distance]
	movss   [waterHitDist], xmm0
	jz      TERRAIN_CHECK
	movss   xmm0, [esp+0x230+dif+x]
	movss   dword ptr [edi+impactPosition+x], xmm0
	movss   xmm0, [esp+0x230+dif+y]
	movss   dword ptr [edi+impactPosition+y], xmm0
	movss   xmm0, [esp+0x230+dif+z]
	movss   dword ptr [edi+impactPosition+z], xmm0
	movss   xmm0, [esp+0x230+waterHitRes+distance]
	# inline `distMoved`
	movss   xmm1, [esp+0x230+segment+radius]
	mulss   xmm1, dword ptr [two]
	divss   xmm0, xmm1
	jmp     short SET_WATER_IMPACT
WATER_CHECK_COLLISION:
	# restore `destroyOnWater` check
	cmp     byte ptr [edi+destroyOnWater], 0
	jz      TERRAIN_CHECK
	mov     ecx, [esp+0x230+curPos]
	fld     dword ptr [ecx+x]
	xorps   xmm0, xmm0
	fstp    dword ptr [edi+impactPosition+x]
	fld     dword ptr [ecx+y]
	fstp    dword ptr [edi+impactPosition+y]
	fld     dword ptr [ecx+z]
	fstp    dword ptr [edi+impactPosition+z]
SET_WATER_IMPACT:
	movss   dword ptr [edi+impactInterp], xmm0
	mov     dword ptr [edi+impactType], IMPACT_Water
TERRAIN_CHECK:
	movss   xmm0, [esp+0x230+segment+radius] # restore radius
	movss   xmm1, dword ptr [NaN]
	movss   xmm5, [esp+0x230+segment+norm_dir+x]
	movss   xmm6, [esp+0x230+segment+norm_dir+y]
	movss   xmm4, [esp+0x230+segment+mid+x]
	movss   xmm7, [esp+0x230+segment+norm_dir+z]
	mov     ecx, [esp+0x230+mapData]
	movss   [esp+0x230+hitResult+distance], xmm1
	movss   [esp+0x230+hitResult+4], xmm1 # unknown
	movaps  xmm1, xmm5
	mulss   xmm1, xmm0
	subss   xmm4, xmm1
	movss   xmm1, [esp+0x230+segment+mid+y]
	xor     eax, eax
	movaps  xmm2, xmm6
	mulss   xmm2, xmm0
	subss   xmm1, xmm2
	movss   xmm2, [esp+0x230+segment+mid+z]
	movaps  xmm3, xmm7
	mulss   xmm3, xmm0
	mulss   xmm0, dword ptr [two]
	lea     edx, [esp+0x230+hitResult]
	mov     [esp+0x230+hitResult+8], eax # unknown
	mov     [esp+0x230+hitResult+12], eax # unknown
	mov     eax, [ecx]
	movss   [esp+0x230+colhitRes+pos+y], xmm1
	xorps   xmm1, xmm1
	subss   xmm2, xmm3
	push    edx
	lea     esi, [esp+0x234+colhitRes]
	lea     ebx, [esp+0x234+hitPos]
	movss   [esp+0x234+colhitRes+pos+x], xmm4
	movss   [esp+0x234+colhitRes+pos+z], xmm2
	movss   [esp+0x234+colhitRes+dir+x], xmm5
	movss   [esp+0x234+colhitRes+dir+y], xmm6
	movss   [esp+0x234+colhitRes+dir+z], xmm7
	movss   [esp+0x234+colhitRes+min], xmm1
	movss   [esp+0x234+colhitRes+max], xmm0
	call    CHeightField__Intersection
	mov     esi, ebx
	call    Vector3f__IsntNaN
	# I have no idea why this is called twice, but it needs to be here
	call    Vector3f__IsntNaN
	# if there's no terrain collision, proceed with the scheduled callbacks
	test    al, al
	# restore `distMoved`
	movss   xmm3, dword ptr [waterHitDist]
	movss   xmm1, [esp+0x230+segment+radius]
	mulss   xmm1, dword ptr [two]
	movss   [esp+0x230+distMoved], xmm1
	jz      short CHECK_LAYER_CHANGE
	xorps   xmm1, xmm1
	movss   xmm0, dword ptr [edi+impactInterp]
	comiss  xmm1, xmm0
	movss   xmm1, [esp+0x230+hitResult+distance]
	movss   xmm2, xmm1 # save terrain hit distance
	ja      short SET_TERRAIN_IMPACT
	mulss   xmm0, [esp+0x230+distMoved]
	comiss  xmm0, xmm1
	jbe     short CHECK_LAYER_CHANGE
SET_TERRAIN_IMPACT:
	divss   xmm1, [esp+0x230+distMoved]
	movss   xmm0, [esp+0x230+hitPos+x]
	movss   dword ptr [edi+impactInterp], xmm1
	movss   dword ptr [edi+impactPosition+x], xmm0
	movss   xmm0, [esp+0x230+hitPos+y]
	movss   dword ptr [edi+impactPosition+y], xmm0
	movss   xmm0, [esp+0x230+hitPos+z]
	movss   dword ptr [edi+impactPosition+z], xmm0
	jmp     SPLICE

# Our changes don't fit; utilize the space freed up by the shield check skip
.section h1; .set h1,0x69DB46 #Moho::Projectile::CheckCollision+0x976
SPLICE:
	mov     dword ptr [edi+impactType], IMPACT_Terrain
	# compare to actual water distance (not the last impact distance, which
	# won't be there for torpedoes); if the terrain is closer, skip the layer
	# callbacks 
	comiss  xmm2, xmm3
	jbe     short CONTINUE_FUNCTION
CHECK_LAYER_CHANGE:
	cmp     byte ptr [layerCallback], LAYERCALLBACK_None
	jz      CONTINUE_FUNCTION
	cmp     byte ptr [layerCallback], LAYERCALLBACK_OnExitWater
	mov     esi, edi
	jz      short WAS_ABOVE_WATER
	mov     edx, LAYER_Air
	call    Entity__SetCurrentLayer
	push    offset strOnExitWater
	mov     byte ptr [edi+belowWater], 0
	jmp     LAYER_DO_CALLBACK
WAS_ABOVE_WATER:
	mov     edx, LAYER_Water
	call    Entity__SetCurrentLayer
	push    offset strOnEnterWater
	# remove extraneous underwater check
	mov     byte ptr [edi+belowWater], 1
LAYER_DO_CALLBACK: # it still doesn't fit so condense this a bit
	mov     ecx, edi
	call    CScriptObject__Callback
	jmp     CONTINUE_FUNCTION

.section h2; .set h2,0x69D627 -1;nop # patcher doesn't like empty sections
CONTINUE_FUNCTION: #Moho::Projectile::CheckCollision+0x457



# This skips the check that makes entries after the first one in the collision
# results list only considered when that entry is the last entry's shield
.section h3; .set h3,0x69DB3E #Moho::Projectile::CheckCollision+0x96E
	jmp     SKIP_SHIELD_CHECK

# The splice will nestle right in here.
# Jumping to an extra section with a label (instead of an offset) will catch any
# section overlaps - though it does make it always a 4 byte jump.

.section h4; .set h4,0x69DBAB -1;nop
SKIP_SHIELD_CHECK: #Moho::Projectile::CheckCollision+0x9DB
                     


# change the 10% velocity offset for unit collision lines to 1% so the distance
# is measured closer to proper - it can't be 0% or else entity collisions will
# register as terrain collisions for some reason
.section h5; .set h5,0x69DA6A #Moho::Projectile::CheckCollision+0x89A
	movss   xmm7, dword ptr [onePercent]

)");
