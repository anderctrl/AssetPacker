#pragma once

#include <cstdint>

namespace AssetLib {
    constexpr char MAGIC_NUMBER[4] = {'A', 'S', 'T', '0'};
    constexpr char PAK_MAGIC[4] = {'P', 'A', 'K', '0'};

    enum class AssetType : uint32_t {
        Raw = 0,
        Texture,
        Audio,
        Font
    };

    enum class CompressionType : uint32_t {
        None = 0,
        ZSTD,
    };

#pragma pack(push, 1)

    struct TextureMetadata {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t channels = 4;
    };

    struct AudioMetadata {
        uint32_t sampleRate = 44100;
        uint16_t bitsPerSample = 16;
        uint16_t channels = 2;
    };

    struct AssetHeader {
        char magic[4] = {MAGIC_NUMBER[0], MAGIC_NUMBER[1], MAGIC_NUMBER[2], MAGIC_NUMBER[3]};
        uint32_t version = 1;
        uint32_t assetType = static_cast<uint32_t>(AssetType::Raw);
        uint32_t compressionType = static_cast<uint32_t>(CompressionType::None);

        uint64_t compressedSize = 0;
        uint64_t uncompressedSize = 0;

        union Metadata {
            TextureMetadata texture;
            AudioMetadata audio;

            Metadata() : texture{} {}
        } meta{};

        AssetHeader() = default;
    };

    struct PakEntry {
        char filePath[128] = {0};
        uint64_t offset = 0;
        uint64_t size = 0;
    };

    struct PakHeader {
        char magic[4] = {PAK_MAGIC[0], PAK_MAGIC[1], PAK_MAGIC[2], PAK_MAGIC[3]};
        uint32_t version = 1;
        uint32_t entryCount = 0;
    };

#pragma pack(pop)
}