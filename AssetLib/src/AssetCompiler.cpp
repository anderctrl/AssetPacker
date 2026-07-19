#include "AssetLib/AssetCompiler.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "zstd.h"
#include "AssetLib/AssetCommon.h"

bool AssetLib::CreateAssetFile(const std::string &sourcePath, const std::string &outputPath, AssetType type,
                                  CompressionType compression) {
    std::ifstream sourceFile(sourcePath, std::ios::binary | std::ios::ate);

    if (!sourceFile.is_open()) return false;

    std::streamsize rawSize = sourceFile.tellg();
    sourceFile.seekg(0, std::ios::beg);

    std::vector<char> rawBuffer(rawSize);
    if (!sourceFile.read(rawBuffer.data(), rawSize)) return false;
    sourceFile.close();

    std::vector<char> finalBuffer;
    uint64_t compressedSize = 0;

    switch (compression) {
        case CompressionType::ZSTD: {
            size_t maxCompressedSize = ZSTD_compressBound(rawSize);
            finalBuffer.resize(maxCompressedSize);

            size_t actualSize = ZSTD_compress(
                finalBuffer.data(),
                maxCompressedSize,
                rawBuffer.data(),
                rawSize,
                10
            );

            if (ZSTD_isError(actualSize)) {
                std::cerr << "ZSTD Error: " << ZSTD_getErrorName(actualSize) << "\n";
                return false;
            }

            finalBuffer.resize(actualSize);
            compressedSize = actualSize;
        }
        case CompressionType::None:
        default: {
            finalBuffer = rawBuffer;
            compressedSize = rawSize;
        }
    }

    AssetHeader header;
    header.assetType = static_cast<uint32_t>(type);
    header.compressionType = static_cast<uint32_t>(compression);
    header.uncompressedSize = static_cast<uint64_t>(rawSize);
    header.compressedSize = compressedSize;

    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) return false;

    outputFile.write(reinterpret_cast<const char *>(&header), sizeof(header));
    outputFile.write(finalBuffer.data(), finalBuffer.size());
    outputFile.close();

    return true;
}
