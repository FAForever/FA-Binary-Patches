// these are the locations of the names of fully registered commands - they're null,
// so they don't work until we fill in the name
asm(R"(
	.section h0; .set h0,0xE1DF0C; .string "ren_Steering"
	.section h1; .set h1,0xE210E8; .string "dbg_Ballistics"
	.section h2; .set h2,0xE23900; .string "dbg_EfxBeams"
	#.section h3; .set h3,0xE23EC4; .string "dbg_Emitter" # actually unused by the engine
	.section h4; .set h4,0xE26858; .string "dbg_Trail"
	.section h5; .set h5,0xE2698C; .string "dbg_CollisionBeam"
	.section h6; .set h6,0xE294E8; .string "dbg_Projectile"
)");