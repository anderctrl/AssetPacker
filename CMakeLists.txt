cmake_minimum_required(VERSION 4.2)
project(AssetPacker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ASSETPACKER_INSTALL "Enable installation rules for AssetPacker components" OFF)

include(FetchContent)

FetchContent_Declare(
        zstd
        GIT_REPOSITORY https://github.com/facebook/zstd.git
        GIT_TAG v1.5.7
        SOURCE_SUBDIR build/cmake
)

set(ZSTD_BUILD_SHARED OFF CACHE INTERNAL "Build shared lib")
set(ZSTD_BUILD_STATIC ON CACHE INTERNAL "Build static lib")
set(ZSTD_BUILD_PROGRAMS OFF CACHE INTERNAL "")
set(ZSTD_BUILD_TESTS OFF CACHE INTERNAL "")
set(ZSTD_BUILD_CONTRIB OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(zstd)

if(TARGET libzstd_static)
    set_target_properties(libzstd_static PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

if(NOT ASSETPACKER_INSTALL)
    macro(original_install)
        _install(${ARGV})
    endmacro()

    macro(install)
    endmacro()
endif()

add_subdirectory(AssetLib)
add_subdirectory(AssetTool)

if(NOT ASSETPACKER_INSTALL)
    macro(install)
        original_install(${ARGV})
    endmacro()
endif()