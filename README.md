# Asset Packer

A lightweight, modern C++20 asset pipeline, cooking, and virtual file system (VFS) archiving toolchain designed for game
engines.

This repository contains a static processing library (`AssetLib`) for building/reading performance-ready runtime formats
and a CLI compiler tool (`AssetTool`).

---

## 🚀 Features

- **Metadata Packing & Optimization:** Extracts asset metadata.
- **ZSTD Compression:** Minimal on-disk footprint.
- **Master Archiving (`.pak`):** Smashes hundreds of compiled asset files into a single bundle.

---

## 🛠️ Binary Specifications

### 1. File Asset Header (`AssetHeader`)

Every compiled `.asset` wrapper starts with a uniform fixed-size block, embedding explicit union blocks for specific
asset types:

| Field            | Type     | Byte Width | Description                                            |
|------------------|----------|------------|--------------------------------------------------------|
| magic            | char[4]  | 4 bytes    | Asset file validation signature: `AST0`                |
| version          | uint32_t | 4 bytes    | Format variant structure version control               |
| assetType        | uint32_t | 4 bytes    | Classification (0: Raw, 1: Texture, 2: Audio, 3: Font) |
| compressionType  | uint32_t | 4 bytes    | Compression system flag (0: None, 1: ZSTD)             |
| compressedSize   | uint64_t | 8 bytes    | Total size of payload on disk                          |
| uncompressedSize | uint64_t | 8 bytes    | Exact buffer allocation space required in RAM          |

### 2. Package Archive (`.pak`) Layout

The `.pak` file places a quick lookup binary Table of Contents at the front of the file, allowing immediate seek
indexing without reading the full package into memory:

* **`PakHeader`** (8 bytes): Contains the archive signature `GPAK` and total contained file count.
* **`PakEntry[]`** ($144 \text{ bytes} \times \text{count}$): A consecutive array storing a virtual path string
  alongside the raw byte offset and file size.
* **Data Payload**: The continuous block containing all standalone compiled `.asset` files stacked together.

---

## 🏭 Operational Workflow

```
[Raw Project Folders] (textures/, audio/, fonts/)
▼
AssetTool --folder (Recursive Compiler)
▼
[Cooked Directory with (.asset) files]
▼
AssetTool --pack (Table of Contents Bundler)
▼
[Unified Bundle] (main.pak)
```

---

## 💻 CLI Tool Commands

### Compile a Single Asset

Manually convert an isolated source file into a cooked wrapper:

```bash
./AssetTool player.png cooked_assets/player.asset texture
./AssetTool footsteps.wav cooked_assets/footsteps.asset audio
```

### Recursive Folder Batch Compilation

Scan a raw working workspace folder and automatically generate matching .asset structures while mirroring nested
subfolders:

```bash
./AssetTool --folder raw_assets/ cooked_assets/
```

### Pack an Archive Bundle

Sash all generated .asset components within a cooked staging directory directly into a master game container:

```bash
./AssetTool --pack cooked_assets/ main.pak
```

---

## 🕹️ Example

Using the built-in loader architecture, mounting the archive file layout and pulling raw textures directly into graphics
memory takes only a few lines of code:

```c++
#include <AssetLib/AssetPakLoader.h>

AssetLib::PakArchive VFS;

if (VFS.Mount("main.pak")) {
    if (VFS.HasFile("textures/characters/player")) {
        AssetLib::LoadedAsset texture = VFS.LoadAsset("textures/characters/player");
        unsigned int width = texture.textureMeta.width;
        unsigned int height = texture.textureMeta.height;
        void* pixelBuffer = texture.data.data();
    }
}
```