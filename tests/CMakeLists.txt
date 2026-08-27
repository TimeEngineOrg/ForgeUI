cmake_minimum_required(VERSION 3.20)

set(TEST_SOURCES
    test_main.cpp
    test_hash.cpp
    test_arena.cpp
    test_storage.cpp
    test_dirty.cpp
    test_layout.cpp
    test_drawlist.cpp
    test_font.cpp
)

add_executable(ForgeCoreTests ${TEST_SOURCES})
target_link_libraries(ForgeCoreTests PRIVATE ForgeUI)
target_include_directories(ForgeCoreTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_test(NAME CoreTests COMMAND ForgeCoreTests)
