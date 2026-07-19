#pragma once

#include <cstdint>

namespace AssetLib {
    constexpr char MAGIC_NUMBER[4] = {'A', 'S', 'T', '0'};

    enum class AssetType : uint32_t {
        Unknown = 0,
        Texture,
        Audio,
        Font
    };

    enum class CompressionType : uint32_t {
        None = 0,
        ZSTD,
    };

    struct AssetHeader {
        char magic[4] = { MAGIC_NUMBER[0], MAGIC_NUMBER[1], MAGIC_NUMBER[2], MAGIC_NUMBER[3] };
        uint32_t version = 1;
        uint32_t assetType = static_cast<uint32_t>(AssetType::Unknown);
        uint32_t compressionType = static_cast<uint32_t>(CompressionType::None);

        uint64_t compressedSize = 0;
        uint64_t uncompressedSize = 0;
    };
}
