cmake_minimum_required(VERSION 3.20)
project(ForgeUI CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------------------------
# Core Library Target
# ------------------------------------------------------------------------------
set(FORGE_SOURCES
    src/Core/ForgeID.cpp
    src/Core/ForgeArena.cpp
    src/Core/ForgeStorage.cpp
    src/Core/ForgeContext.cpp
    src/Core/ForgeDirty.cpp
    src/Core/ForgeJobSystem.cpp
    src/Layout/ForgeLayoutEngine.cpp
    src/Render/ForgeDrawList.cpp
    src/Render/ForgePipelines.cpp
    src/Font/ForgeFont_MSDF.cpp
    src/Widgets/ForgeWidgets.cpp
    src/Backend/ForgeBackend_CPU.cpp
    src/Backend/ForgeBackend_OpenGL.cpp
)

add_library(ForgeUI STATIC ${FORGE_SOURCES})
target_include_directories(ForgeUI PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# ------------------------------------------------------------------------------
# Monorepo Subprojects & Options
# ------------------------------------------------------------------------------
option(FORGE_BUILD_EXAMPLES "Build example applications" ON)
option(FORGE_BUILD_TESTS "Build ForgeUI unit tests" ON)
option(FORGE_BUILD_WINDOWED_DEMO "Build GLFW hardware windowed demo" ON)
option(FORGE_BUILD_BENCHMARK "Build Dear ImGui comparison benchmark" OFF)

if(FORGE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(FORGE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
