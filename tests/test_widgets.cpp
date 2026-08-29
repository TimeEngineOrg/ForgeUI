#include "ForgeUI/Widgets/ForgeWidgets.h"
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
    input.mousePos = forge::ForgeVec2(20.0f, 15.0f);
    input.mousePressed[0] = true;
    input.mouseDown[0] = true;

    bool clicked = forge::Button("TestBtn", forge::ForgeDimension::Px(100.0f), forge::ForgeDimension::Px(30.0f));
    if (clicked) return false;

    forge::ForgeID btnId = ctx.GetID("TestBtn");
    if (input.activeId != btnId) return false;

    ctx.EndFrame();

    ctx.BeginFrame();
    input.mousePos = forge::ForgeVec2(20.0f, 15.0f);
    input.mousePressed[0] = false;
    input.mouseDown[0] = false;
    input.mouseReleased[0] = true;

    clicked = forge::Button("TestBtn", forge::ForgeDimension::Px(100.0f), forge::ForgeDimension::Px(30.0f));
    if (!clicked) return false;
    if (input.activeId != forge::FORGE_INVALID_ID) return false;

    ctx.EndFrame();

    return true;
}

FORGE_TEST(Widgets_SliderFloatClamping) {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    float value = 0.0f;

    ctx.BeginFrame();
    forge::ForgeInputState& input = ctx.GetInput();
    input.mousePos = forge::ForgeVec2(100.0f, 10.0f);
    input.mousePressed[0] = true;
    input.mouseDown[0] = true;

    bool changed = forge::SliderFloat("Speed", &value, 0.0f, 100.0f, forge::ForgeDimension::Px(200.0f));
    if (!changed) return false;
    if (value < 49.0f || value > 51.0f) return false;

    ctx.EndFrame();

    ctx.BeginFrame();
    input.mousePos = forge::ForgeVec2(300.0f, 10.0f);
    input.mouseDown[0] = true;

    changed = forge::SliderFloat("Speed", &value, 0.0f, 100.0f, forge::ForgeDimension::Px(200.0f));
    if (!changed) return false;
    if (value != 100.0f) return false;

    ctx.EndFrame();

    return true;
}

FORGE_TEST(Widgets_CheckboxToggle) {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    bool enabled = false;

    ctx.BeginFrame();
    forge::ForgeInputState& input = ctx.GetInput();
    input.mousePos = forge::ForgeVec2(5.0f, 5.0f);
    input.mousePressed[0] = true;
    input.mouseDown[0] = true;

    bool toggled = forge::Checkbox("Enable Bloom", &enabled);
    if (toggled) return false;
    if (enabled) return false;

    ctx.EndFrame();

    ctx.BeginFrame();
    input.mousePos = forge::ForgeVec2(5.0f, 5.0f);
    input.mousePressed[0] = false;
    input.mouseDown[0] = false;
    input.mouseReleased[0] = true;

    toggled = forge::Checkbox("Enable Bloom", &enabled);
    if (!toggled) return false;
    if (!enabled) return false;

    ctx.EndFrame();

    return true;
}

FORGE_TEST(Widgets_TextInputEditing) {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    char buffer[64] = "Hello";

    ctx.BeginFrame();
    forge::ForgeInputState& input = ctx.GetInput();
    input.mousePos = forge::ForgeVec2(10.0f, 10.0f);
    input.mousePressed[0] = true;

    forge::TextInput("UserName", buffer, sizeof(buffer));

    forge::ForgeID inputId = ctx.GetID("UserName");
    if (input.focusedId != inputId) return false;

    ctx.EndFrame();

    ctx.BeginFrame();
    input.keyBackspace = true;
    bool modified = forge::TextInput("UserName", buffer, sizeof(buffer));
    if (!modified) return false;
    if (std::string(buffer) != "Hell") return false;

    ctx.EndFrame();

    ctx.BeginFrame();
    input.keyBackspace = false;
    input.textInputBuffer[0] = 'o';
    input.textInputBuffer[1] = '!';
    input.textInputBuffer[2] = '\0';
    input.textInputLength = 2;

    modified = forge::TextInput("UserName", buffer, sizeof(buffer));
    if (!modified) return false;
    if (std::string(buffer) != "Hello!") return false;

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

void RegisterWidgetsTests() {}
