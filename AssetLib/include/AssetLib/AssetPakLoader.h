#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <AssetLib/AssetLoader.h>
#include <AssetLib/AssetCommon.h>

namespace AssetLib {
    class PakArchive {
    public:
        PakArchive() = default;

        ~PakArchive();

        bool Mount(const std::string &pakPath);

        bool HasFile(const std::string &virtualPath) const;

        LoadedAsset LoadAsset(const std::string &virtualPath);

        [[nodiscard]] const std::unordered_map<std::string, PakEntry>& GetIndex() const;

    private:
        std::ifstream m_PakFileStream;
        std::unordered_map<std::string, PakEntry> m_TOC;
    };
}
