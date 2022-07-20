#include <cmath>

float MAX_HEIGHT = 256.0;
float MIN_HEIGHT = -256.0;

int __thiscall HeightCylinders(void *this_)
{
    int result = reinterpret_cast<int(__thiscall *)(void *)>(0x7433B0)(this_); // call start_game

    if (auto SIM = *reinterpret_cast<uintptr_t *>(0x10A63F0); SIM != NULL)
    {
        if (auto STIMap = *reinterpret_cast<uintptr_t *>(SIM + 0x8CC); STIMap != NULL)
        {
            auto mapSizeX = *reinterpret_cast<int *>(STIMap + 0x142C);
            auto mapSizeY = *reinterpret_cast<int *>(STIMap + 0x1430);
            if (auto MapData = *reinterpret_cast<uintptr_t *>(STIMap + 0x0); MapData != NULL && mapSizeX > 0 && mapSizeY > 0)
            {
                if (auto TerrainHeights = *reinterpret_cast<uintptr_t *>(MapData + 0x0); TerrainHeights != NULL)
                {

                    float maxCalculated = -256.0;
                    float minCalculated = 256.0;
                    for (auto x = 0; x < mapSizeX; x++)
                    {
                        for (auto y = 0; y < mapSizeY; y++)
                        {
                            float height = static_cast<float>(*reinterpret_cast<unsigned short *>(TerrainHeights + (y * mapSizeX + x) * 2)) * static_cast<float>(0.0078125);
                            if (height > maxCalculated)
                                maxCalculated = height;
                            if (height < minCalculated)
                                minCalculated = height;
                        }
                    }
                    MIN_HEIGHT = std::round(minCalculated - 1);
                    MAX_HEIGHT = std::ceil(maxCalculated + 1);
                    auto GetModuleHandle = *reinterpret_cast<void *(__stdcall **)(const char *)>(0xC0F378);
                    auto GetProcAddress = *reinterpret_cast<void *(__stdcall **)(void *, const char *)>(0xC0F48C);
                    auto VirtualProtect = reinterpret_cast<int(__stdcall *)(void *, int, int, int *)>(GetProcAddress(GetModuleHandle("KERNEL32"), "VirtualProtect"));
                    long oldProt;
                    VirtualProtect(reinterpret_cast<void *>(0x7EE2B8 + 0x4), sizeof(float), 0x40, &oldProt);
                    // F30F 1015 6CF8E400                 movss   xmm2, ds: *pointer*
                    *reinterpret_cast<float **>(0x7EE2B8 + 0x4) = &MIN_HEIGHT;
                    VirtualProtect(reinterpret_cast<void *>(0x7EE2B8 + 0x4), sizeof(float), oldProt, &oldProt);
                    VirtualProtect(reinterpret_cast<void *>(0x7EE336 + 0x4), sizeof(float), 0x40, &oldProt);
                    *reinterpret_cast<float **>(0x7EE336 + 0x4) = &MAX_HEIGHT;
                    VirtualProtect(reinterpret_cast<void *>(0x7EE336 + 0x4), sizeof(float), oldProt, &oldProt);

                    auto this__ = *reinterpret_cast<uintptr_t *>(0x10C7C28) + 0x37C; // 0x10C7C28 -> main wxWindow
                    reinterpret_cast<void(__thiscall *)(uintptr_t)>(0x7EDFE0)(this__);    // call generate_ring_cylinders(this)
                }
            }
        }
    }
    return result;
}
