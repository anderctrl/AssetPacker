#pragma once

#include <cstdint>
#include <cstring>

namespace AssetLib {
    constexpr char MAGIC_NUMBER[4] = {'A', 'S', 'T', '0'};

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

        uint64_t compressedSize = 0;
        uint64_t uncompressedSize = 0;

        union Metadata {
            TextureMetadata texture;
            AudioMetadata audio;

            Metadata() { std::memset(this, 0, sizeof(Metadata)); }
        } meta;

        AssetHeader() = default;
    };
}
