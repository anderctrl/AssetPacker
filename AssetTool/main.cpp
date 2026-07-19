#include <iostream>
#include <string>
#include <filesystem>
#include <format>

#include "AssetLib/AssetCommon.h"
#include "AssetLib/AssetCompiler.h"
#include "AssetLib/AssetLoader.h"
#include "AssetLib/AssetPacker.h" // Added for .pak generation support

namespace fs = std::filesystem;

void PrintUsage() {
    std::cout << "Usage (Single File): AssetTool <input_file> <output_file> <type>\n";
    std::cout << "Usage (Full Folder): AssetTool --folder <input_dir> <output_dir>\n";
    std::cout << "Usage (Pack Archive): AssetTool --pack <cooked_dir> <output_file.pak>\n";
    std::cout << "Supported types: texture, audio, fallback\n";
}

AssetLib::AssetType ParseType(const std::string &typeStr) {
    if (typeStr == "texture") return AssetLib::AssetType::Texture;
    if (typeStr == "audio") return AssetLib::AssetType::Audio;
    return AssetLib::AssetType::Raw;
}

AssetLib::AssetType DetectTypeFromExtension(const fs::path &ext) {
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tga") {
        return AssetLib::AssetType::Texture;
    }
    if (ext == ".wav") {
        return AssetLib::AssetType::Audio;
    }
    if (ext == ".ttf" || ext == ".otf" || ext == ".woff") {
        return AssetLib::AssetType::Font;
    }
    return AssetLib::AssetType::Raw;
}

void ProcessSingleFile(const std::string &input, const std::string &output, const std::string &typeStr) {
    const AssetLib::AssetType assetType = ParseType(typeStr);
    std::cout << std::format("Compiling individual asset: {}\n", input);

    if (AssetLib::CreateAssetFile(input, output, assetType, AssetLib::CompressionType::ZSTD)) {
        std::cout << "Successfully compiled asset!\n";
    } else {
        std::cerr << "Error: Failed to compile asset.\n";
    }
}

void ProcessFolder(const std::string &inputDir, const std::string &outputDir) {
    fs::path sourceDir(inputDir);
    fs::path targetBaseDir(outputDir);

    if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir)) {
        std::cerr << std::format("Error: Input directory does not exist: {}\n", inputDir);
        return;
    }

    std::cout << std::format("Scanning directory and subdirectories: {}\n", inputDir);

    for (const auto &entry: fs::recursive_directory_iterator(sourceDir)) {
        if (fs::is_regular_file(entry.path())) {
            const fs::path &filePath = entry.path();

            fs::path relativePath = fs::relative(filePath, sourceDir);
            fs::path targetPath = targetBaseDir / relativePath;

            targetPath.replace_extension(".asset");
            fs::create_directories(targetPath.parent_path());

            AssetLib::AssetType assetType = DetectTypeFromExtension(filePath.extension());

            std::cout << std::format("Processing: {} -> {}\n", filePath.string(), targetPath.string());

            AssetLib::CreateAssetFile(filePath.string(), targetPath.string(), assetType,
                                      AssetLib::CompressionType::ZSTD);
        }
    }
    std::cout << "Recursive batch compilation complete!\n";
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        PrintUsage();
        return 1;
    }

    const std::string firstArg = argv[1];

    if (firstArg == "--folder") {
        if (argc < 4) {
            std::cerr << "Error: Missing input or output directory configuration arguments.\n";
            PrintUsage();
            return 1;
        }
        ProcessFolder(argv[2], argv[3]);
        return 0;
    }

    if (firstArg == "--pack") {
        if (argc < 4) {
            std::cerr << "Error: Missing source directory or output file arguments.\n";
            PrintUsage();
            return 1;
        }
        if (AssetLib::CreatePakArchive(argv[2], argv[3])) {
            return 0;
        }
        return 1;
    }

    if (argc < 4) {
        PrintUsage();
        return 1;
    }
    ProcessSingleFile(argv[1], argv[2], argv[3]);
    return 0;
}
