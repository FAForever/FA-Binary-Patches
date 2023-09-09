//Adding ability to disable mesh renderer for minimap
//if DefaultMiniLOD = 0 (there is a console command for it) then Moho::Cartographic::RenderMeshes function call will be skipped
//this allows to skip an enormous amount of useless computations, as minimap works similar to main camera
//and as it's mainly zoomed out, there are thousands of objects that are always in view
//and this in turn causes mesh renderer to do thousands of useless LOD calculations per frame

void minimapMesh()
{
	asm(
        "cmp dword ptr [0x00F57A88], 0x0;"
        "je 0x007D1B02;"
        // MeshRender ON
        "push ebp;"
        "push ecx;"
        "mov ecx, dword ptr [esp+0xA8];"
        "jmp 0x007D1AF2;"
	);
}