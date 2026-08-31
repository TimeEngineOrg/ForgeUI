#include "ForgeUI/Core/ForgeDirty.h"
#include <string>
#include <functional>

extern void RegisterTest(const std::string& name, std::function<bool()> func);
#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

FORGE_TEST(Dirty_BubbleUpAndCascade) {
    forge::ForgeStorage storage;

    forge::ForgeElementConfig fixedRoot{};
    fixedRoot.layout.width = forge::ForgeDimension::Px(500.0f);
    fixedRoot.layout.height = forge::ForgeDimension::Px(500.0f);

    forge::ForgeElementConfig fitContainer{};
    fitContainer.layout.width = forge::ForgeDimension::Fit();
    fitContainer.layout.height = forge::ForgeDimension::Fit();

    forge::ForgeElementConfig leaf{};
    leaf.layout.width = forge::ForgeDimension::Px(50.0f);
    leaf.layout.height = forge::ForgeDimension::Px(50.0f);

    storage.BeginElement(1, fixedRoot);
    uint32_t cContainer = storage.BeginElement(2, fitContainer);
    uint32_t cLeaf = storage.BeginElement(3, leaf);
    storage.EndElement();
    storage.EndElement();
    storage.EndElement();

    forge::ForgeDirtySystem::BubbleUpDirty(storage, cLeaf);
    forge::ForgeDirtySystem::CascadeDirty(storage, cContainer);

    if (storage.GetSubtreeSize(cContainer) != 2) return false;

    return true;
}

void RegisterDirtyTests() {
    RegisterTest("Dirty_BubbleUpAndCascade", Test_Dirty_BubbleUpAndCascade);
}
