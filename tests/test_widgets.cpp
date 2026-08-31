#include "ForgeUI/Widgets/ForgeWidgets.h"
#include "ForgeUI/Core/ForgeContext.h"
#include "ForgeUI/Core/ForgeTheme.h"
#include <string>
#include <functional>

extern void RegisterTest(const std::string& name, std::function<bool()> func);
#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

FORGE_TEST(Widgets_ButtonInteraction) {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    ctx.BeginFrame();
    forge::ForgeInputState& input = ctx.GetInput();
    input.mousePos = forge::ForgeVec2(50.0f, 15.0f);
    input.mousePressed[0] = true;
    input.mouseDown[0] = true;

    bool clicked = forge::Button("Save Button", forge::ForgeDimension::Px(100.0f), forge::ForgeDimension::Px(30.0f));
    if (clicked) return false;

    forge::ForgeID btnId = ctx.GetID("Save Button");
    if (input.activeId != btnId) return false;

    ctx.EndFrame();

    ctx.BeginFrame();
    input.mousePos = forge::ForgeVec2(50.0f, 15.0f);
    input.mousePressed[0] = false;
    input.mouseDown[0] = false;
    input.mouseReleased[0] = true;

    clicked = forge::Button("Save Button", forge::ForgeDimension::Px(100.0f), forge::ForgeDimension::Px(30.0f));
    if (!clicked) return false;
    if (input.activeId != forge::FORGE_INVALID_ID) return false;

    ctx.EndFrame();
    return true;
}

FORGE_TEST(Widgets_SliderFloatClamping) {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    float val = 0.0f;

    ctx.BeginFrame();
    forge::ForgeInputState& input = ctx.GetInput();
    input.mousePos = forge::ForgeVec2(100.0f, 10.0f);
    input.mousePressed[0] = true;
    input.mouseDown[0] = true;

    bool changed = forge::SliderFloat("Volume", &val, 0.0f, 1.0f, forge::ForgeDimension::Px(200.0f));
    if (!changed) return false;
    if (val < 0.45f || val > 0.55f) return false;

    ctx.EndFrame();

    ctx.BeginFrame();
    forge::ForgeInputState& input2 = ctx.GetInput();
    input2.mousePos = forge::ForgeVec2(300.0f, 10.0f);
    input2.mousePressed[0] = true;
    input2.mouseDown[0] = true;

    changed = forge::SliderFloat("Volume", &val, 0.0f, 1.0f, forge::ForgeDimension::Px(200.0f));
    if (!changed) return false;
    if (val != 1.0f) return false;

    ctx.EndFrame();
    return true;
}

FORGE_TEST(Widgets_CheckboxToggle) {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    bool checked = false;

    ctx.BeginFrame();
    forge::ForgeInputState& input = ctx.GetInput();
    input.mousePos = forge::ForgeVec2(8.0f, 8.0f);
    input.mousePressed[0] = true;

    bool toggled = forge::Checkbox("Enable Sound", &checked);
    if (!toggled) return false;
    if (!checked) return false;

    ctx.EndFrame();

    ctx.BeginFrame();
    input.mousePos = forge::ForgeVec2(8.0f, 8.0f);
    input.mousePressed[0] = true;

    toggled = forge::Checkbox("Enable Sound", &checked);
    if (!toggled) return false;
    if (checked) return false;

    ctx.EndFrame();
    return true;
}

FORGE_TEST(Widgets_TextInputEditing) {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    char buffer[64] = "Hello World";

    ctx.BeginFrame();
    forge::ForgeInputState& input = ctx.GetInput();
    input.mousePos = forge::ForgeVec2(50.0f, 10.0f);
    input.mousePressed[0] = true;

    forge::TextInput("UserName", buffer, sizeof(buffer));
    forge::ForgeID txtId = ctx.GetID("UserName");
    if (input.focusedId != txtId) return false;

    ctx.EndFrame();

    ctx.BeginFrame();
    input.mousePressed[0] = false;
    input.keyBackspace = true;

    bool modified = forge::TextInput("UserName", buffer, sizeof(buffer));
    if (!modified) return false;
    if (std::string(buffer) != "Hello Worl") return false;

    ctx.EndFrame();

    ctx.BeginFrame();
    input.keyBackspace = false;
    input.textInputBuffer[0] = 'd';
    input.textInputBuffer[1] = '!';
    input.textInputBuffer[2] = '\0';
    input.textInputLength = 2;

    modified = forge::TextInput("UserName", buffer, sizeof(buffer));
    if (!modified) return false;
    if (std::string(buffer) != "Hello World!") return false;

    ctx.EndFrame();
    return true;
}

FORGE_TEST(Widgets_ScrollViewClipping) {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    float scrollY = 0.0f;

    ctx.BeginFrame();
    forge::ForgeInputState& input = ctx.GetInput();
    input.mousePos = forge::ForgeVec2(50.0f, 50.0f);
    input.scrollDelta = forge::ForgeVec2(0.0f, 2.0f);

    bool ok = forge::BeginScrollView("ScrollView1", forge::ForgeDimension::Px(200.0f), forge::ForgeDimension::Px(300.0f), &scrollY);
    if (!ok) return false;
    if (scrollY != 40.0f) return false;

    forge::Label("Inner Item 1");
    forge::Label("Inner Item 2");

    forge::EndScrollView();

    const forge::ForgeDrawList& drawList = ctx.GetDrawList();
    if (drawList.CurrentClipRect().x != -100000.0f) return false;

    ctx.EndFrame();
    return true;
}

FORGE_TEST(Widgets_AdvancedSuiteDropdownAndTabs) {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    const char* tabs[] = {"Graphics", "Audio", "Controls"};
    int32_t selectedTab = 0;

    ctx.BeginFrame();
    forge::ForgeInputState& input = ctx.GetInput();
    input.mousePos = forge::ForgeVec2(120.0f, 15.0f);
    input.mousePressed[0] = true;

    bool tabChanged = forge::TabBar("SettingsTabs", tabs, 3, &selectedTab);
    if (!tabChanged || selectedTab != 1) return false;

    const char* options[] = {"Low", "Medium", "Ultra"};
    int32_t selectedOpt = 0;
    bool dropdownOpen = false;

    input.mousePos = forge::ForgeVec2(50.0f, 10.0f);
    input.mousePressed[0] = true;

    forge::Dropdown("Quality", &selectedOpt, options, 3, &dropdownOpen);
    if (!dropdownOpen) return false;

    ctx.EndFrame();
    return true;
}

FORGE_TEST(Widgets_ThemePresets) {
    forge::ForgeTheme dark = forge::ForgeTheme::Dark();
    forge::ForgeTheme light = forge::ForgeTheme::Light();
    forge::ForgeTheme cyberpunk = forge::ForgeTheme::Cyberpunk();

    if (dark.background.r > 0.5f) return false;
    if (light.background.r < 0.5f) return false;
    if (cyberpunk.accent.g < 0.8f) return false;

    return true;
}

void RegisterWidgetsTests() {
    RegisterTest("Widgets_ButtonInteraction", Test_Widgets_ButtonInteraction);
    RegisterTest("Widgets_SliderFloatClamping", Test_Widgets_SliderFloatClamping);
    RegisterTest("Widgets_CheckboxToggle", Test_Widgets_CheckboxToggle);
    RegisterTest("Widgets_TextInputEditing", Test_Widgets_TextInputEditing);
    RegisterTest("Widgets_ScrollViewClipping", Test_Widgets_ScrollViewClipping);
    RegisterTest("Widgets_AdvancedSuiteDropdownAndTabs", Test_Widgets_AdvancedSuiteDropdownAndTabs);
    RegisterTest("Widgets_ThemePresets", Test_Widgets_ThemePresets);
}
