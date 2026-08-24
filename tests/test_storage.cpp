#include "ForgeUI/Core/ForgeStorage.h"
#include <string>
#include <functional>

extern void RegisterTest(const std::string& name, std::function<bool()> func);
#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

FORGE_TEST(Storage_HierarchyAndSubtreeSize) {
    forge::ForgeStorage storage;

    uint32_t root = storage.BeginElement(1, {});
    uint32_t child1 = storage.BeginElement(2, {});
    storage.EndElement();

    uint32_t child2 = storage.BeginElement(3, {});
    uint32_t grandChild1 = storage.BeginElement(4, {});
    storage.EndElement();
    uint32_t grandChild2 = storage.BeginElement(5, {});
    storage.EndElement();
    storage.EndElement();

    storage.EndElement();

    if (storage.Count() != 5) return false;

    if (storage.GetSubtreeSize(root) != 5) return false;
    if (storage.GetChildCount(root) != 2) return false;

    if (storage.GetSubtreeSize(child1) != 1) return false;
    if (storage.GetChildCount(child1) != 0) return false;

    if (storage.GetSubtreeSize(child2) != 3) return false;
    if (storage.GetChildCount(child2) != 2) return false;

    if (storage.GetSubtreeSize(grandChild1) != 1) return false;
    if (storage.GetSubtreeSize(grandChild2) != 1) return false;

    uint32_t nextAfterChild1 = storage.NextSiblingIndex(child1);
    if (nextAfterChild1 != child2) return false;

    uint32_t nextAfterChild2 = storage.NextSiblingIndex(child2);
    if (nextAfterChild2 != 5) return false;

    return true;
}

FORGE_TEST(Storage_ResetClearsCount) {
    forge::ForgeStorage storage;

    storage.BeginElement(100, {});
    storage.BeginElement(101, {});
    storage.EndElement();
    storage.EndElement();

    if (storage.Count() != 2) return false;

    storage.Reset();
    if (storage.Count() != 0) return false;

    return true;
}
