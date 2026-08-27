cmake_minimum_required(VERSION 3.20)
project(ForgeUI CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

set(FORGE_SOURCES
    src/Core/ForgeID.cpp
    src/Core/ForgeArena.cpp
    src/Core/ForgeStorage.cpp
    src/Core/ForgeContext.cpp
    src/Core/ForgeDirty.cpp
    src/Layout/ForgeLayoutEngine.cpp
    src/Render/ForgeDrawList.cpp
    src/Render/ForgePipelines.cpp
    src/Font/ForgeFont_MSDF.cpp
    src/Backend/ForgeBackend_OpenGL.cpp
)

add_library(ForgeUI STATIC ${FORGE_SOURCES})
target_include_directories(ForgeUI PUBLIC include)

enable_testing()
add_subdirectory(tests)
