#include <iostream>
#include <string>
#include <AssetLib/AssetCompiler.h>
#include <AssetLib/AssetCommon.h>

void PrintUsage() {
    std::cout << "Usage: AssetTool <input_file> <output_file> <type>\n";
    std::cout << "Supported types: texture, audio, font\n";
    std::cout << "Example: AssetTool player.png player.asset texture\n";
}

AssetLib::AssetType ParseType(const std::string &typeStr) {
    if (typeStr == "texture") return AssetLib::AssetType::Texture;
    if (typeStr == "audio") return AssetLib::AssetType::Audio;
    if (typeStr == "font") return AssetLib::AssetType::Font;
    return AssetLib::AssetType::Unknown;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        PrintUsage();
        return 1;
    }

    const std::string inputFile = argv[1];
    const std::string outputFile = argv[2];
    const std::string typeString = argv[3];

    const AssetLib::AssetType assetType = ParseType(typeString);
    if (assetType == AssetLib::AssetType::Unknown) {
        std::cerr << "Error: Unknown asset type '" << typeString << "'\n";
        PrintUsage();
        return 1;
    }

    std::cout << "Compiling asset: " << inputFile << " -> " << outputFile << "...\n";

    if (AssetLib::CreateAssetFile(inputFile, outputFile, assetType, AssetLib::CompressionType::ZSTD)) {
        std::cout << "Successfully compiled and compressed asset!\n";
        return 0;
    }
    std::cerr << "Error: Failed to compile asset. Check your file paths.\n";
    return 1;
}
