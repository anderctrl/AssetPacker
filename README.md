# 📦 Asset Packer


A lightweight C++ asset pipeline and archiving system for game development.

This repository contains both a core library (`AssetLib`) for building and reading asset formats and a compilation toolchain (`AssetTool`).

---

## 🚀 Features

- **Metadata Uniformity:** Every single file begins with an identical data footprint (`AssetHeader`).
- **Zstd Compression:** Minimal on-disk footprint.
- **CMake Support:** Library available via FetchContent.

---

## 🛠️ The Data Specifications (`AssetHeader`)

| Field            | Type     | Byte Width | Description                                  |
|------------------|----------|------------|----------------------------------------------|
| magic            | char[4]  | 4 bytes    | Identification flag. `AST0`                  |
| version          | uint32_t | 4 bytes    | Structure version                            |
| assetType        | uint32_t | 4 bytes    | Classification (Texture, Font, etc.)         |
| compressionType  | uint32_t | 4 bytes    | None, ZSTD                                   |
| compressedSize   | uint64_t | 8 bytes    | Total size packed                            |
| uncompressedSize | uint64_t | 8 bytes    | RAM requirement allocation for decompression |

---

## 🏭 Pipeline Workflow

```
[Raw Sources] (.png/.wav/.ttf/...)
     ▼
AssetTool (Processes & Compresses)
     ▼
[Build Archive] (assets.pak)
```