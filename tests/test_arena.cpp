#include "ForgeUI/Core/ForgeArena.h"
#include <string>
#include <functional>

extern void RegisterTest(const std::string& name, std::function<bool()> func);
#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

FORGE_TEST(Arena_AlignedAllocationAndReset) {
    forge::ForgeArena arena(1024 * 1024);

    void* ptr1 = arena.Allocate(128, 64);
    if (!ptr1) return false;
    if (reinterpret_cast<uintptr_t>(ptr1) % 64 != 0) return false;

    void* ptr2 = arena.Allocate(15, 64);
    if (!ptr2) return false;
    if (reinterpret_cast<uintptr_t>(ptr2) % 64 != 0) return false;

    std::string_view original = "ForgeUI Arena String";
    std::string_view duplicated = arena.DuplicateString(original);
    if (duplicated != original) return false;
    if (duplicated.data() == original.data()) return false;

    size_t usedBefore = arena.Used();
    if (usedBefore == 0) return false;

    arena.Reset();
    if (arena.Used() != 0) return false;

    return true;
}

void RegisterArenaTests() {
    RegisterTest("Arena_AlignedAllocationAndReset", Test_Arena_AlignedAllocationAndReset);
}
