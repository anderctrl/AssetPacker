add_library(AssetLib STATIC
        src/AssetCompiler.cpp
        src/AssetLoader.cpp
)

target_include_directories(AssetLib PUBLIC include)

target_include_directories(AssetLib PRIVATE ${CMAKE_SOURCE_DIR}/external)

target_link_libraries(AssetLib PRIVATE libzstd_static)