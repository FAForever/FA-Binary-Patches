#include "global.h"

// The engine sizes the 3D DSP buffers from the DirectSound speaker config,
// which maps every mode except 5.1 to 2 output channels, but initializes
// X3DAudio with XACT's real final mix channel mask. On a device with an LFE
// channel and a different channel count (7.1, 5.1 surround), X3DAudioCalculate
// writes the LFE coefficient past the end of the matrix every 3D cue, which
// corrupts the neighbouring heap block, and XACT rejects the matrix with
// "SND: XACT3DApply failed". Size the buffers from the same mask instead.

namespace Moho
{
    struct Audio3DDspSettings
    {
        float *matrixCoefficients; // +0x00
        float *delayTimes;         // +0x04
        unsigned srcChannelCount;  // +0x08
        unsigned dstChannelCount;  // +0x0C
    };
}

// The operator new and delete the engine allocates these buffers with
const auto AudioOperatorNew = (void *(*)(size_t))0xA82130;
const auto AudioOperatorDelete = (void (*)(void *))0xA82542;

// AudioEngineImpl fields
constexpr int kImplXactEngine = 0x34;
constexpr int kImplDspSettings = 0x7C;
constexpr int kImplX3DAudioHandle = 0x110;

// IXACTEngine::GetFinalMixFormat writes a WAVEFORMATEXTENSIBLE (0x28 bytes)
constexpr int kGetFinalMixFormatSlot = 5;
constexpr int kWaveFormatChannelMask = 0x14;

SHARED int X3DAudioInitializeSized(char *impl)
{
    void *engine = *(void **)(impl + kImplXactEngine);
    void *handle = impl + kImplX3DAudioHandle;

    // func_AudioInitialize(engine in eax, handle on the stack)
    int result;
    asm volatile(
        "push %[handle];"
        "call 0x4D8170;"
        "add esp, 4;"
        : "=a"(result)
        : "a"(engine), [handle] "r"(handle)
        : "ecx", "edx", "memory");
    if (result < 0)
        return result;

    alignas(4) char format[0x28];
    using GetFinalMixFormatFn = __stdcall int (*)(void *, void *);
    auto getFinalMixFormat = (*(GetFinalMixFormatFn **)engine)[kGetFinalMixFormatSlot];
    int formatResult = getFinalMixFormat(engine, format);
    if (formatResult < 0)
    {
        WarningF("SND: GetFinalMixFormat failed (0x%08X), 3D sound may not work", formatResult);
        return result;
    }

    unsigned mask = *(unsigned *)(format + kWaveFormatChannelMask);
    unsigned channels = 0;
    for (; mask; mask &= mask - 1)
        ++channels;

    // X3DAudio writes one coefficient for an empty mask, which still fits
    if (channels == 0)
    {
        WarningF("SND: output device reported no speaker channel mask, 3D sound may not work");
        return result;
    }

    auto settings = (Moho::Audio3DDspSettings *)(impl + kImplDspSettings);
    if (channels == settings->dstChannelCount)
        return result;

    AudioOperatorDelete(settings->delayTimes);
    AudioOperatorDelete(settings->matrixCoefficients);
    settings->dstChannelCount = channels;
    settings->delayTimes = (float *)AudioOperatorNew(channels * sizeof(float));
    settings->matrixCoefficients =
        (float *)AudioOperatorNew(channels * settings->srcChannelCount * sizeof(float));
    memset(settings->delayTimes, 0, channels * sizeof(float));
    memset(settings->matrixCoefficients, 0, channels * settings->srcChannelCount * sizeof(float));
    return result;
}
