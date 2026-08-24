add_executable(forge_tests
    test_main.cpp
    test_hash.cpp
    test_storage.cpp
    test_arena.cpp
    test_layout.cpp
    test_dirty.cpp
)

target_link_libraries(forge_tests PRIVATE ForgeUI)

add_test(NAME ForgeCoreTests COMMAND forge_tests)
