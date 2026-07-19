#pragma once
#include <string>
#include <vector>

#include "AssetCommon.h"

namespace AssetLib {

    bool CompileTexture(const std::string& sourcePath, std::vector<char>& outRawData, TextureMetadata& outMeta);

    bool CompileAudio(const std::string& sourcePath, std::vector<char>& outRawData, AudioMetadata& outMeta);

    bool CreateAssetFile(const std::string &sourcePath, const std::string &outputPath, AssetType type,
                         CompressionType compression);
}
