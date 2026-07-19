#include <AssetLib/AssetPakLoader.h>
#include <iostream>
#include <cstring>
#include <zstd.h>

namespace AssetLib {
    PakArchive::~PakArchive() {
        if (m_PakFileStream.is_open()) {
            m_PakFileStream.close();
        }
    }

    bool PakArchive::Mount(const std::string &pakPath) {
        m_PakFileStream.open(pakPath, std::ios::binary);
        if (!m_PakFileStream.is_open()) {
            std::cerr << "PakLoader Error: Could not open pak file: " << pakPath << "\n";
            return false;
        }

        PakHeader header;
        m_PakFileStream.read(reinterpret_cast<char *>(&header), sizeof(PakHeader));

        if (std::memcmp(header.magic, PAK_MAGIC, 4) != 0) {
            std::cerr << "PakLoader Error: Invalid pack file format!\n";
            m_PakFileStream.close();
            return false;
        }

        std::vector<PakEntry> entries(header.entryCount);
        m_PakFileStream.read(reinterpret_cast<char *>(entries.data()), sizeof(PakEntry) * header.entryCount);

        m_TOC.clear();
        for (const auto &entry: entries) {
            m_TOC[std::string(entry.filePath)] = entry;
        }

        return true;
    }

    bool PakArchive::HasFile(const std::string &virtualPath) const {
        std::string fullKey = virtualPath;

        if (!fullKey.ends_with(".asset")) {
            fullKey += ".asset";
        }

        return m_TOC.contains(fullKey);
    }

    LoadedAsset PakArchive::LoadAsset(const std::string &virtualPath) {
        LoadedAsset result;

        std::string fullKey = virtualPath;
        if (!fullKey.ends_with(".asset")) {
            fullKey += ".asset";
        }

        const auto it = m_TOC.find(fullKey);
        if (it == m_TOC.end()) {
            std::cerr << "PakLoader Error: Asset file not found in archive: " << fullKey << "\n";
            return result;
        }

        const PakEntry &entry = it->second;

        m_PakFileStream.seekg(entry.offset, std::ios::beg);

        AssetHeader assetHeader;
        m_PakFileStream.read(reinterpret_cast<char *>(&assetHeader), sizeof(AssetHeader));

        std::vector<char> compressedBuffer(assetHeader.compressedSize);
        m_PakFileStream.read(compressedBuffer.data(), assetHeader.compressedSize);

        result.type = static_cast<AssetType>(assetHeader.assetType);
        result.data.resize(assetHeader.uncompressedSize);

        if (result.type == AssetType::Texture) {
            result.textureMeta = assetHeader.meta.texture;
        } else if (result.type == AssetType::Audio) {
            result.audioMeta = assetHeader.meta.audio;
        }

        if (assetHeader.compressionType == static_cast<uint32_t>(CompressionType::ZSTD)) {
            const size_t actualDecompressedSize = ZSTD_decompress(
                result.data.data(), assetHeader.uncompressedSize,
                compressedBuffer.data(), assetHeader.compressedSize
            );

            if (ZSTD_isError(actualDecompressedSize)) {
                std::cerr << "PakLoader Error: File decompression failed for " << fullKey << "\n";
                result.data.clear();
            }
        } else {
            result.data = std::move(compressedBuffer);
        }

        return result;
    }
}
