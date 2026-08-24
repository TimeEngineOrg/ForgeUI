cmake_minimum_required(VERSION 3.20)
project(ForgeUI VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FORGEUI_BUILD_TESTS "Build ForgeUI unit tests" ON)

add_library(ForgeUI
    src/Core/ForgeID.cpp
    src/Core/ForgeArena.cpp
    src/Core/ForgeStorage.cpp
    src/Core/ForgeDirty.cpp
    src/Core/ForgeContext.cpp
    src/Layout/ForgeLayoutEngine.cpp
)

target_include_directories(ForgeUI PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (MSVC)
    target_compile_options(ForgeUI PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(ForgeUI PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (FORGEUI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
