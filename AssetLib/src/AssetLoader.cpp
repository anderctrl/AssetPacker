#include <AssetLib/AssetLoader.h>
#include <fstream>
#include <iostream>
#include <zstd.h>

namespace AssetLib {
    LoadedAsset LoadAssetFile(const std::string &assetPath) {
        LoadedAsset result;

        std::ifstream file(assetPath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Loader Error: Could not open asset file: " << assetPath << "\n";
            return result;
        }

        AssetHeader header;
        file.read(reinterpret_cast<char *>(&header), sizeof(AssetHeader));

        if (header.magic[0] != MAGIC_NUMBER[0] || header.magic[1] != MAGIC_NUMBER[1] ||
            header.magic[2] != MAGIC_NUMBER[2] || header.magic[3] != MAGIC_NUMBER[3]) {
            std::cerr << "Loader Error: Invalid magic number!\n";
            return result;
        }

        std::vector<char> compressedBuffer(header.compressedSize);
        file.read(compressedBuffer.data(), header.compressedSize);
        file.close();

        result.type = static_cast<AssetType>(header.assetType);
        result.data.resize(header.uncompressedSize);

        if (result.type == AssetType::Texture) {
            result.textureMeta = header.meta.texture;
        } else if (result.type == AssetType::Audio) {
            result.audioMeta = header.meta.audio;
        }

        if (header.compressedSize < header.uncompressedSize) {
            const size_t actualDecompressedSize = ZSTD_decompress(
                result.data.data(), header.uncompressedSize,
                compressedBuffer.data(), header.compressedSize
            );

            if (ZSTD_isError(actualDecompressedSize)) {
                std::cerr << "Loader Error: Decompression failed\n";
                result.data.clear();
                return result;
            }
        } else {
            result.data = std::move(compressedBuffer);
        }

        return result;
    }
}
