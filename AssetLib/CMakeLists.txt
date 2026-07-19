add_library(AssetLib STATIC
        src/AssetCompiler.cpp
)

target_include_directories(AssetLib PUBLIC include)

target_link_libraries(AssetLib PRIVATE libzstd_static)