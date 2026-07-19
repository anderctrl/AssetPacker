#include <AssetLib/AssetCompiler.h>
#include <AssetLib/AssetCommon.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <zstd.h>

#define STB_IMAGE_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#include <format>
#include <stb/stb_image.h>
#include <dr_libs/dr_wav.h>

namespace AssetLib {
    bool CompileTexture(const std::string &sourcePath, std::vector<char> &outRawData, TextureMetadata &outMeta) {
        int width, height, channels;
        unsigned char *pixels = stbi_load(sourcePath.c_str(), &width, &height, &channels, 4);
        if (!pixels) return false;

        outMeta.width = width;
        outMeta.height = height;
        outMeta.channels = 4;

        const size_t totalBytes = width * height * 4;
        outRawData.assign(pixels, pixels + totalBytes);

        stbi_image_free(pixels);
        return true;
    }

    bool CompileAudio(const std::string &sourcePath, std::vector<char> &outRawData, AudioMetadata &outMeta) {
        unsigned int channels;
        unsigned int sampleRate;
        drwav_uint64 totalPCMFrameCount;

        drwav_int16 *pSampleData = drwav_open_file_and_read_pcm_frames_s16(
            sourcePath.c_str(), &channels, &sampleRate, &totalPCMFrameCount, nullptr
        );

        if (!pSampleData) {
            return false;
        }

        outMeta.sampleRate = sampleRate;
        outMeta.bitsPerSample = 16;
        outMeta.channels = static_cast<uint16_t>(channels);

        const size_t totalBytes = totalPCMFrameCount * channels * sizeof(drwav_int16);

        const auto pByteData = reinterpret_cast<const char *>(pSampleData);
        outRawData.assign(pByteData, pByteData + totalBytes);

        drwav_free(pSampleData, nullptr);
        return true;
    }

    bool CreateAssetFile(const std::string &sourcePath, const std::string &outputPath, AssetType type,
                         CompressionType compression) {
        std::vector<char> uncompressedPayload;
        AssetHeader header;
        header.assetType = static_cast<uint32_t>(type);

        if (type == AssetType::Texture) {
            if (!CompileTexture(sourcePath, uncompressedPayload, header.meta.texture)) {
                std::cerr << "Compiler Error: Failed to parse image layout.\n";
                return false;
            }
        } else if (type == AssetType::Audio) {
            if (!CompileAudio(sourcePath, uncompressedPayload, header.meta.audio)) {
                std::cerr << "Compiler Error: Failed to parse audio layout.\n";
                return false;
            }
        } else {
            std::ifstream file(sourcePath, std::ios::binary | std::ios::ate);
            if (!file.is_open()) return false;
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            uncompressedPayload.resize(size);
            file.read(uncompressedPayload.data(), size);
        }

        std::vector<char> finalPayload;
        uint64_t compSize = 0;

        if (compression == CompressionType::ZSTD && !uncompressedPayload.empty()) {
            size_t maxCompressedSize = ZSTD_compressBound(uncompressedPayload.size());
            finalPayload.resize(maxCompressedSize);

            size_t actualSize = ZSTD_compress(
                finalPayload.data(), maxCompressedSize,
                uncompressedPayload.data(), uncompressedPayload.size(), 1
            );

            if (ZSTD_isError(actualSize)) {
                return false;
            }
            finalPayload.resize(actualSize);
            compSize = actualSize;
        } else {
            finalPayload = uncompressedPayload;
            compSize = uncompressedPayload.size();
        }

        header.uncompressedSize = uncompressedPayload.size();
        header.compressedSize = compSize;

        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile.is_open()) return false;

        outFile.write(reinterpret_cast<const char *>(&header), sizeof(AssetHeader));
        outFile.write(finalPayload.data(), finalPayload.size());
        outFile.close();

        if (type == AssetType::Texture) {
            std::cout << std::format("    [Texture] {}x{} ({} channels)\n",
                                     header.meta.texture.width, header.meta.texture.height,
                                     header.meta.texture.channels);
        } else if (type == AssetType::Audio) {
            std::cout << std::format("    [Audio] {}Hz, {}-bit\n",
                                     header.meta.audio.sampleRate, header.meta.audio.bitsPerSample);
        } else if (type == AssetType::Font) {
            std::cout << "    [Font File Wrapper]\n";
        } else {
            std::cout << "    [Raw File]\n";
        }

        std::cout << std::format("    Size: {} bytes -> {} bytes compressed\n",
                                 header.uncompressedSize, compSize);

        return true;
    }
}
