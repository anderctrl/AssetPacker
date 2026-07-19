add_library(AssetLib STATIC
        src/AssetCompiler.cpp
        src/AssetPacker.cpp
        src/AssetLoader.cpp
        src/AssetPakLoader.cpp
)

target_include_directories(AssetLib PUBLIC include)

target_include_directories(AssetLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../external)

target_link_libraries(AssetLib PRIVATE libzstd_static)