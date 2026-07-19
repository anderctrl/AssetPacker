cmake_minimum_required(VERSION 4.2)
project(AssetPacker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        zstd
        GIT_REPOSITORY https://github.com/facebook/zstd.git
        GIT_TAG v1.5.7
        SOURCE_SUBDIR build/cmake
)

set(ZSTD_BUILD_PROGRAMS OFF CACHE INTERNAL "")
set(ZSTD_BUILD_TESTS OFF CACHE INTERNAL "")
set(ZSTD_BUILD_CONTRIB OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(zstd)

add_subdirectory(AssetLib)
add_subdirectory(AssetTool)