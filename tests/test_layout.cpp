#include "ForgeUI/Layout/ForgeLayoutEngine.h"
#include "ForgeUI/Core/ForgeContext.h"
#include <string>
#include <functional>

extern void RegisterTest(const std::string& name, std::function<bool()> func);
#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

FORGE_TEST(Layout_FixedRowWithGapAndPadding) {
    forge::ForgeStorage storage;

    forge::ForgeElementConfig rootCfg{};
    rootCfg.layout.width = forge::ForgeDimension::Px(400.0f);
    rootCfg.layout.height = forge::ForgeDimension::Px(200.0f);
    rootCfg.layout.direction = forge::ForgeFlexDirection::Row;
    rootCfg.layout.padding = {10.0f, 10.0f, 10.0f, 10.0f};
    rootCfg.layout.gap = 20.0f;

    forge::ForgeElementConfig childCfg{};
    childCfg.layout.width = forge::ForgeDimension::Px(100.0f);
    childCfg.layout.height = forge::ForgeDimension::Px(50.0f);

    uint32_t root = storage.BeginElement(1, rootCfg);
    uint32_t c1 = storage.BeginElement(2, childCfg);
    storage.EndElement();
    uint32_t c2 = storage.BeginElement(3, childCfg);
    storage.EndElement();
    storage.EndElement();

    forge::ForgeLayoutEngine::ComputeLayout(storage, 400.0f, 200.0f);

    if (storage.GetX(root) != 0.0f || storage.GetY(root) != 0.0f) return false;
    if (storage.GetWidth(root) != 400.0f || storage.GetHeight(root) != 200.0f) return false;

    if (storage.GetX(c1) != 10.0f || storage.GetY(c1) != 10.0f) return false;
    if (storage.GetWidth(c1) != 100.0f || storage.GetHeight(c1) != 50.0f) return false;

    if (storage.GetX(c2) != 130.0f || storage.GetY(c2) != 10.0f) return false;
    if (storage.GetWidth(c2) != 100.0f || storage.GetHeight(c2) != 50.0f) return false;

    return true;
}

FORGE_TEST(Layout_FlexGrowDistribution) {
    forge::ForgeStorage storage;

    forge::ForgeElementConfig rootCfg{};
    rootCfg.layout.width = forge::ForgeDimension::Px(300.0f);
    rootCfg.layout.height = forge::ForgeDimension::Px(100.0f);
    rootCfg.layout.direction = forge::ForgeFlexDirection::Row;

    forge::ForgeElementConfig fixedChild{};
    fixedChild.layout.width = forge::ForgeDimension::Px(100.0f);
    fixedChild.layout.height = forge::ForgeDimension::Px(100.0f);

    forge::ForgeElementConfig growChild1{};
    growChild1.layout.width = forge::ForgeDimension::FlexGrow(1.0f);
    growChild1.layout.height = forge::ForgeDimension::Px(100.0f);

    forge::ForgeElementConfig growChild2{};
    growChild2.layout.width = forge::ForgeDimension::FlexGrow(1.0f);
    growChild2.layout.height = forge::ForgeDimension::Px(100.0f);

    storage.BeginElement(1, rootCfg);
    uint32_t cFixed = storage.BeginElement(2, fixedChild);
    storage.EndElement();
    uint32_t cGrow1 = storage.BeginElement(3, growChild1);
    storage.EndElement();
    uint32_t cGrow2 = storage.BeginElement(4, growChild2);
    storage.EndElement();
    storage.EndElement();

    forge::ForgeLayoutEngine::ComputeLayout(storage, 300.0f, 100.0f);

    if (storage.GetWidth(cFixed) != 100.0f) return false;
    if (storage.GetWidth(cGrow1) != 100.0f) return false;
    if (storage.GetWidth(cGrow2) != 100.0f) return false;

    if (storage.GetX(cFixed) != 0.0f) return false;
    if (storage.GetX(cGrow1) != 100.0f) return false;
    if (storage.GetX(cGrow2) != 200.0f) return false;

    return true;
}

FORGE_TEST(Layout_ColumnAlignmentAndJustify) {
    forge::ForgeStorage storage;

    forge::ForgeElementConfig rootCfg{};
    rootCfg.layout.width = forge::ForgeDimension::Px(200.0f);
    rootCfg.layout.height = forge::ForgeDimension::Px(300.0f);
    rootCfg.layout.direction = forge::ForgeFlexDirection::Column;
    rootCfg.layout.alignment = forge::ForgeAlignment::Center;
    rootCfg.layout.justify = forge::ForgeJustify::Center;

    forge::ForgeElementConfig childCfg{};
    childCfg.layout.width = forge::ForgeDimension::Px(100.0f);
    childCfg.layout.height = forge::ForgeDimension::Px(100.0f);

    storage.BeginElement(1, rootCfg);
    uint32_t child = storage.BeginElement(2, childCfg);
    storage.EndElement();
    storage.EndElement();

    forge::ForgeLayoutEngine::ComputeLayout(storage, 200.0f, 300.0f);

    if (storage.GetX(child) != 50.0f) return false;
    if (storage.GetY(child) != 100.0f) return false;

    return true;
}

FORGE_TEST(Layout_ClippingAndCulling) {
    forge::ForgeStorage storage;

    forge::ForgeElementConfig rootCfg{};
    rootCfg.layout.width = forge::ForgeDimension::Px(100.0f);
    rootCfg.layout.height = forge::ForgeDimension::Px(100.0f);
    rootCfg.layout.direction = forge::ForgeFlexDirection::Column;
    rootCfg.layout.clipContent = true;

    forge::ForgeElementConfig child1{};
    child1.layout.width = forge::ForgeDimension::Px(50.0f);
    child1.layout.height = forge::ForgeDimension::Px(60.0f);

    forge::ForgeElementConfig child2{};
    child2.layout.width = forge::ForgeDimension::Px(50.0f);
    child2.layout.height = forge::ForgeDimension::Px(60.0f);

    forge::ForgeElementConfig child3{};
    child3.layout.width = forge::ForgeDimension::Px(50.0f);
    child3.layout.height = forge::ForgeDimension::Px(50.0f);

    storage.BeginElement(1, rootCfg);
    uint32_t c1 = storage.BeginElement(2, child1);
    storage.EndElement();
    uint32_t c2 = storage.BeginElement(3, child2);
    storage.EndElement();
    uint32_t c3 = storage.BeginElement(4, child3);
    storage.EndElement();
    storage.EndElement();

    forge::ForgeLayoutEngine::ComputeLayout(storage, 100.0f, 100.0f);

    if ((storage.GetCullFlags(c1) & forge::ForgeCullFlags::Offscreen) != forge::ForgeCullFlags::None) return false;
    if ((storage.GetCullFlags(c2) & forge::ForgeCullFlags::Offscreen) != forge::ForgeCullFlags::None) return false;
    if ((storage.GetCullFlags(c3) & forge::ForgeCullFlags::Offscreen) != forge::ForgeCullFlags::Offscreen) return false;

    return true;
}

void RegisterLayoutTests() {
    RegisterTest("Layout_FixedRowWithGapAndPadding", Test_Layout_FixedRowWithGapAndPadding);
    RegisterTest("Layout_FlexGrowDistribution", Test_Layout_FlexGrowDistribution);
    RegisterTest("Layout_ColumnAlignmentAndJustify", Test_Layout_ColumnAlignmentAndJustify);
    RegisterTest("Layout_ClippingAndCulling", Test_Layout_ClippingAndCulling);
}
