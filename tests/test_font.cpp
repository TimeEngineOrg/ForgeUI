#include "ForgeUI/Font/ForgeFont_MSDF.h"
#include <string>
#include <functional>

extern void RegisterTest(const std::string& name, std::function<bool()> func);
#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

FORGE_TEST(Font_GlyphMetrics) {
    forge::ForgeArena arena(1024 * 1024);

    forge::ForgeFont_MSDF font;
    font.Initialize(&arena, 1);

    forge::ForgeGlyphMetrics metrics;
    bool found = font.GetGlyphMetrics('A', &metrics);
    if (!found) return false;
    if (metrics.codepoint != 'A') return false;
    if (metrics.width <= 0.0f) return false;
    if (metrics.height <= 0.0f) return false;

    forge::ForgeVec2 size = font.MeasureText("Hello ForgeUI", 14.0f);
    if (size.x <= 0.0f) return false;
    if (size.y < 18.0f) return false;

    return true;
}

FORGE_TEST(Font_AtlasAllocator) {
    forge::ForgeArena arena(1024 * 1024);

    forge::ForgeAtlasAllocator allocator;
    allocator.Initialize(&arena, 512, 512, 128);

    uint32_t x1 = 0, y1 = 0;
    bool res1 = allocator.Allocate(32, 32, &x1, &y1);
    if (!res1) return false;
    if (x1 != 0 || y1 != 0) return false;

    uint32_t x2 = 0, y2 = 0;
    bool res2 = allocator.Allocate(32, 32, &x2, &y2);
    if (!res2) return false;
    if (x2 != 32 || y2 != 0) return false;

    return true;
}

void RegisterFontTests() {}
