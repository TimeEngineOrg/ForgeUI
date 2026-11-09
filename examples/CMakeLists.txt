# ==============================================================================
# ForgeUI Examples Subproject
# ==============================================================================

# 1. Minimal Demo
add_executable(ForgeDemo Demo/main.cpp)
target_link_libraries(ForgeDemo PRIVATE ForgeUI)

# 2. High-Capacity Stress Test (100,000 Elements)
add_executable(ForgeStressTest StressTest/main.cpp)
target_link_libraries(ForgeStressTest PRIVATE ForgeUI)

# 3. Interactive Hardware OpenGL 3.3 Windowed Demo
if(FORGE_BUILD_WINDOWED_DEMO)
    include(FetchContent)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
    )
    FetchContent_MakeAvailable(glfw)

    add_executable(ForgeWindowedDemo WindowedDemo/main.cpp)
    target_link_libraries(ForgeWindowedDemo PRIVATE ForgeUI glfw)
    if(WIN32)
        target_link_libraries(ForgeWindowedDemo PRIVATE opengl32)
    endif()
endif()

# 4. Dear ImGui Head-to-Head Comparison Benchmark
if(FORGE_BUILD_BENCHMARK)
    include(FetchContent)
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.91.8
    )
    FetchContent_MakeAvailable(imgui)

    add_executable(ForgeBenchmark
        Benchmark/main.cpp
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
    )
    target_link_libraries(ForgeBenchmark PRIVATE ForgeUI)
    target_include_directories(ForgeBenchmark PRIVATE ${imgui_SOURCE_DIR})
endif()
