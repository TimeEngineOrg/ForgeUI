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
target_include_directories(ForgeUI PUBLIC include)

add_executable(ForgeDemo examples/Demo/main.cpp)
target_link_libraries(ForgeDemo PRIVATE ForgeUI)
target_include_directories(ForgeDemo PRIVATE include ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ForgeStressTest examples/StressTest/main.cpp)
target_link_libraries(ForgeStressTest PRIVATE ForgeUI)
target_include_directories(ForgeStressTest PRIVATE include ${CMAKE_CURRENT_SOURCE_DIR})

option(FORGE_BUILD_BENCHMARK "Build Dear ImGui comparison benchmark" OFF)

if(FORGE_BUILD_BENCHMARK)
    include(FetchContent)
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.91.8
    )
    FetchContent_MakeAvailable(imgui)

    add_executable(ForgeBenchmark
        examples/Benchmark/main.cpp
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
    )
    target_link_libraries(ForgeBenchmark PRIVATE ForgeUI)
    target_include_directories(ForgeBenchmark PRIVATE include ${imgui_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
endif()

enable_testing()
add_subdirectory(tests)
