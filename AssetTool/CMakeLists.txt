add_executable(AssetTool
        main.cpp
)

target_link_libraries(AssetTool PRIVATE AssetLib)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()