#include "global.h"

// Streaming wave banks are opened unbuffered (FILE_FLAG_NO_BUFFERING), so every
// read must start on a logical sector of the drive. The shipped banks align
// their waves to 2048 bytes, so on a drive with 4096-byte logical sectors about
// half the waves in each streaming bank fail to play with "XACT: Invalid arg",
// and a failed cue later crashes the game while it shuts down sound, in
// IXACTWaveBank::Destroy. Through the file cache the same reads work at any
// offset. XACT reads 0x12 x 2048 bytes at a time, a multiple of 4096, so a bank
// aligned to the drive's sector size keeps the unbuffered path.

// Imports are read at each call: patch code has no dynamic initialization
#define CreateFileA_ WDecl(0xC0F540, __stdcall void *(*)(const char *, uint32_t, uint32_t, void *, uint32_t, uint32_t, void *))
#define ReadFile_ WDecl(0xC0F560, __stdcall bool (*)(void *, void *, uint32_t, uint32_t *, void *))
#define CloseHandle_ WDecl(0xC0F590, __stdcall bool (*)(void *))

namespace
{
    constexpr uint32_t kGenericRead = 0x80000000;
    constexpr uint32_t kFileShareRead = 1;
    constexpr uint32_t kOpenExisting = 3;
    constexpr uint32_t kFileAttributeNormal = 0x80;
    constexpr uint32_t kFileFlagOverlapped = 0x40000000;
    constexpr uint32_t kFileFlagNoBuffering = 0x20000000;
    void *const kInvalidHandle = (void *)-1;

    // FILE_INFO_BY_HANDLE_CLASS FileStorageInfo, FILE_STORAGE_INFO
    constexpr int kFileStorageInfo = 16;
    struct FileStorageInfo
    {
        uint32_t logicalBytesPerSector;
        uint32_t physicalBytesPerSectorForAtomicity;
        uint32_t physicalBytesPerSectorForPerformance;
        uint32_t fileSystemEffectivePhysicalBytesPerSectorForAtomicity;
        uint32_t flags;
        uint32_t byteOffsetForSectorAlignment;
        uint32_t byteOffsetForPartitionAlignment;
    };
    VALIDATE_SIZE(FileStorageInfo, 0x1C)

    using GetFileInformationByHandleExFn = __stdcall bool (*)(void *, int, void *, uint32_t);

    // 'WBND', version, header version, then the segment table; bank data
    // (segment 0) holds the wave alignment at +0x50
    constexpr uint32_t kWaveBankSignature = 0x444E4257;
    constexpr uint32_t kBankDataAlignment = 0x50;

    // The open flags retail uses, or FILE_FLAG_OVERLAPPED alone when the bank's
    // wave alignment isn't a multiple of its drive's logical sector size. Any
    // failure while checking keeps retail's flags
    uint32_t StreamingWaveBankOpenFlags(const char *path, const char *bankName)
    {
        constexpr uint32_t retailFlags = kFileFlagOverlapped | kFileFlagNoBuffering;

        auto getFileInformationByHandleEx = (GetFileInformationByHandleExFn)GetProcAddress(
            GetModuleHandleA("kernel32.dll"), "GetFileInformationByHandleEx");
        if (!getFileInformationByHandleEx)
            return retailFlags;

        void *file = CreateFileA_(path, kGenericRead, kFileShareRead, nullptr, kOpenExisting, kFileAttributeNormal, nullptr);
        if (file == kInvalidHandle)
            return retailFlags;

        uint32_t flags = retailFlags;
        FileStorageInfo storage{};
        alignas(4) uint8_t header[0x200];
        uint32_t bytesRead = 0;
        if (getFileInformationByHandleEx(file, kFileStorageInfo, &storage, sizeof(storage)) &&
            storage.logicalBytesPerSector != 0 &&
            ReadFile_(file, header, sizeof(header), &bytesRead, nullptr) && bytesRead >= 0x10 &&
            *(uint32_t *)header == kWaveBankSignature)
        {
            uint32_t bankData = ((uint32_t *)header)[3];
            if (bytesRead >= kBankDataAlignment + 4 && bankData <= bytesRead - kBankDataAlignment - 4)
            {
                uint32_t alignment = *(uint32_t *)(header + bankData + kBankDataAlignment);
                if (alignment % storage.logicalBytesPerSector != 0)
                {
                    flags = kFileFlagOverlapped;
                    LogF("SND: WaveBank '%s' is aligned to %u bytes and its drive reads %u-byte sectors; "
                         "streaming it through the file cache",
                         bankName, alignment, storage.logicalBytesPerSector);
                }
            }
        }

        CloseHandle_(file);
        return flags;
    }
}

SHARED void *OpenStreamingWaveBank(const char *path, const char *bankName)
{
    return CreateFileA_(path, kGenericRead, kFileShareRead, nullptr, kOpenExisting,
                        StreamingWaveBankOpenFlags(path, bankName), nullptr);
}
