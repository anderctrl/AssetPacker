#include <AssetLib/AssetPacker.h>
#include <AssetLib/AssetCommon.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
#include <format>

namespace fs = std::filesystem;

namespace AssetLib {
    bool CreatePakArchive(const std::string &sourceFolder, const std::string &outputPakPath) {
        fs::path sourceDir(sourceFolder);
        if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir)) {
            std::cerr << std::format("Packer Error: Source folder does not exist: {}\n", sourceFolder);
            return false;
        }

        std::vector<fs::path> assetFiles;
        for (const auto &entry: fs::recursive_directory_iterator(sourceDir)) {
            if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".asset") {
                assetFiles.push_back(entry.path());
            }
        }

        PakHeader header;
        header.entryCount = static_cast<uint32_t>(assetFiles.size());

        std::vector<PakEntry> tableOfContents(header.entryCount);

        uint64_t currentDataOffset = sizeof(PakHeader) + (sizeof(PakEntry) * header.entryCount);

        for (size_t i = 0; i < assetFiles.size(); ++i) {
            fs::path relPath = fs::relative(assetFiles[i], sourceDir);
            std::string relPathStr = relPath.generic_string();

            std::memset(tableOfContents[i].filePath, 0, 128);
            std::strncpy(tableOfContents[i].filePath, relPathStr.c_str(),
                         std::min(relPathStr.length(), static_cast<size_t>(127)));

            uint64_t fileSize = fs::file_size(assetFiles[i]);
            tableOfContents[i].size = fileSize;
            tableOfContents[i].offset = currentDataOffset;

            currentDataOffset += fileSize;
        }

        std::ofstream pakFile(outputPakPath, std::ios::binary);
        if (!pakFile.is_open()) return false;

        pakFile.write(reinterpret_cast<const char *>(&header), sizeof(PakHeader));
        pakFile.write(reinterpret_cast<const char *>(tableOfContents.data()),
                      sizeof(PakEntry) * tableOfContents.size());

        std::cout << std::format("Packing {} files into {}...\n", header.entryCount, outputPakPath);
        for (const auto &assetFile: assetFiles) {
            std::ifstream srcFile(assetFile, std::ios::binary);
            if (!srcFile.is_open()) return false;

            pakFile << srcFile.rdbuf();
        }

        std::cout << std::format("Successfully generated archive pack! Total size: {} bytes\n", currentDataOffset);
        return true;
    }
}
