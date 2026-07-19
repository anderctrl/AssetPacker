#pragma once
#include <string>

#include "AssetCommon.h"

namespace AssetLib {
    bool CreateAssetFile(const std::string &sourcePath, const std::string &outputPath, AssetType type,
                         CompressionType compression);
}
