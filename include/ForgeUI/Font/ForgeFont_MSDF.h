#pragma once

#include "IForgeFontRasterizer.h"
#include "../Core/ForgeArena.h"
#include "../Render/ForgeDrawList.h"

namespace forge {

struct ForgeAtlasNode {
    int16_t x;
    int16_t y;
    int16_t width;
};

class ForgeAtlasAllocator {
public:
    ForgeAtlasAllocator();
    ~ForgeAtlasAllocator();

    void Initialize(ForgeArena* arena, uint32_t width, uint32_t height, uint32_t maxNodes = 1024);
    void Reset();

    bool Allocate(uint32_t width, uint32_t height, uint32_t* outX, uint32_t* outY);

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

private:
    ForgeArena* m_Arena;
    ForgeAtlasNode* m_Nodes;
    uint32_t m_NodeCount;
    uint32_t m_MaxNodes;
    uint32_t m_Width;
    uint32_t m_Height;
};

class ForgeFont_MSDF : public IForgeFontRasterizer {
public:
    ForgeFont_MSDF();
    virtual ~ForgeFont_MSDF();

    void Initialize(ForgeArena* arena, uint64_t atlasTextureHandle = 0);

    virtual bool GetGlyphMetrics(uint32_t codepoint, ForgeGlyphMetrics* outMetrics) override;
    virtual float GetLineHeight() const override { return m_LineHeight; }
    virtual float GetAscent() const override { return m_Ascent; }
    virtual float GetDescent() const override { return m_Descent; }

    ForgeVec2 MeasureText(const char* text, float fontSize) const;
    void AppendTextToDrawList(ForgeDrawList* drawList, const char* text, const ForgeVec2& pos, float fontSize, uint32_t color) const;

    void SetAtlasTextureHandle(uint64_t handle) { m_AtlasTextureHandle = handle; }
    uint64_t GetAtlasTextureHandle() const { return m_AtlasTextureHandle; }

    ForgeAtlasAllocator* GetDynamicAtlasAllocator() { return &m_AtlasAllocator; }

private:
    ForgeArena* m_Arena;
    uint64_t m_AtlasTextureHandle;
    float m_LineHeight;
    float m_Ascent;
    float m_Descent;

    ForgeGlyphMetrics m_StaticGlyphs[128];
    bool m_StaticGlyphLoaded[128];

    ForgeAtlasAllocator m_AtlasAllocator;
};

}
