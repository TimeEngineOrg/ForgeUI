#include "ForgeUI/Core/ForgeID.h"
#include <string>
#include <string_view>
#include <functional>

extern void RegisterTest(const std::string& name, std::function<bool()> func);
#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

FORGE_TEST(Hash_CompileTimeAndRuntimeConsistency) {
    using namespace forge::literals;

    constexpr forge::ForgeID constHash = "TestLabel"_forge_id;
    forge::ForgeID runtimeHash = forge::HashString("TestLabel");

    if (constHash == forge::FORGE_INVALID_ID) return false;
    if (constHash != runtimeHash) return false;

    constexpr forge::ForgeID emptyHash = forge::HashString("");
    if (emptyHash != forge::FORGE_INVALID_ID) return false;

    return true;
}

FORGE_TEST(Hash_ScopedIDStackHierarchy) {
    forge::ForgeIDStack stack;

    forge::ForgeID rootBtn = stack.GetID("Button");

    stack.Push("PanelA");
    forge::ForgeID panelABtn = stack.GetID("Button");
    stack.Pop();

    stack.Push("PanelB");
    forge::ForgeID panelBBtn = stack.GetID("Button");
    stack.Pop();

    if (rootBtn == panelABtn) return false;
    if (rootBtn == panelBBtn) return false;
    if (panelABtn == panelBBtn) return false;

    stack.Push("List");
    stack.Push(0);
    forge::ForgeID item0 = stack.GetID("Item");
    stack.Pop();

    stack.Push(1);
    forge::ForgeID item1 = stack.GetID("Item");
    stack.Pop();
    stack.Pop();

    if (item0 == item1) return false;

    return true;
}

void RegisterHashTests() {
    RegisterTest("Hash_CompileTimeAndRuntimeConsistency", Test_Hash_CompileTimeAndRuntimeConsistency);
    RegisterTest("Hash_ScopedIDStackHierarchy", Test_Hash_ScopedIDStackHierarchy);
}
