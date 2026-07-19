#pragma once
#include <string>
#include <vector>
#include <AssetLib/AssetCommon.h>

namespace AssetLib {
    struct LoadedAsset {
        AssetType type = AssetType::Raw;
        std::vector<char> data;

        TextureMetadata textureMeta;
        AudioMetadata audioMeta;
    };

    LoadedAsset LoadAssetFile(const std::string &assetPath);
}
