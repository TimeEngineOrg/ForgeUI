#include "ForgeUI/Render/ForgeDrawList.h"
#include <string>
#include <functional>

extern void RegisterTest(const std::string& name, std::function<bool()> func);
#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

FORGE_TEST(DrawList_BatchConsolidation) {
    forge::ForgeArena arena(1024 * 1024);

    forge::ForgeDrawList drawList;
    drawList.Initialize(&arena, 1024, 2048, 64);

    drawList.AddRect(forge::ForgeVec2(0, 0), forge::ForgeVec2(100, 100), 0xFFFFFFFF);
    drawList.AddRect(forge::ForgeVec2(100, 0), forge::ForgeVec2(200, 100), 0xFF0000FF);
    drawList.AddRect(forge::ForgeVec2(200, 0), forge::ForgeVec2(300, 100), 0x00FF00FF);

    if (drawList.GetVertexCount() != 12) return false;
    if (drawList.GetIndexCount() != 18) return false;
    if (drawList.GetCommandCount() != 1) return false;

    const forge::ForgeDrawCmd* cmd = drawList.GetCommands();
    if (cmd[0].elemCount != 18) return false;
    if (cmd[0].elemOffset != 0) return false;

    drawList.AddCircle(forge::ForgeVec2(50, 50), 25.0f, 0xFFFFFFFF);
    if (drawList.GetCommandCount() != 2) return false;

    return true;
}

FORGE_TEST(DrawList_ClippingStack) {
    forge::ForgeArena arena(1024 * 1024);

    forge::ForgeDrawList drawList;
    drawList.Initialize(&arena, 1024, 2048, 64);

    drawList.PushClipRect(forge::ForgeVec4(10, 10, 100, 100));
    drawList.AddRect(forge::ForgeVec2(0, 0), forge::ForgeVec2(50, 50), 0xFFFFFFFF);

    const forge::ForgeDrawVertex* verts = drawList.GetVertices();
    if (verts[0].clipRect.x != 10.0f) return false;
    if (verts[0].clipRect.y != 10.0f) return false;
    if (verts[0].clipRect.z != 100.0f) return false;
    if (verts[0].clipRect.w != 100.0f) return false;

    drawList.PushClipRect(forge::ForgeVec4(20, 20, 80, 80));
    drawList.AddRect(forge::ForgeVec2(0, 0), forge::ForgeVec2(50, 50), 0xFFFFFFFF);

    verts = drawList.GetVertices();
    if (verts[4].clipRect.x != 20.0f) return false;
    if (verts[4].clipRect.y != 20.0f) return false;
    if (verts[4].clipRect.z != 80.0f) return false;
    if (verts[4].clipRect.w != 80.0f) return false;

    drawList.PopClipRect();
    if (drawList.CurrentClipRect().x != 10.0f) return false;
    drawList.PopClipRect();

    return true;
}

FORGE_TEST(DrawList_ClipStackOverflowProtection) {
    forge::ForgeArena arena(1024 * 1024);
    forge::ForgeDrawList drawList;
    drawList.Initialize(&arena, 1024, 2048, 64);

    for (size_t i = 0; i < 50; ++i) {
        drawList.PushClipRect(forge::ForgeVec4(0.0f, 0.0f, 100.0f, 100.0f));
    }

    for (size_t i = 0; i < 60; ++i) {
        drawList.PopClipRect();
    }

    return true;
}

void RegisterDrawListTests() {
    RegisterTest("DrawList_BatchConsolidation", Test_DrawList_BatchConsolidation);
    RegisterTest("DrawList_ClippingStack", Test_DrawList_ClippingStack);
    RegisterTest("DrawList_ClipStackOverflowProtection", Test_DrawList_ClipStackOverflowProtection);
}
