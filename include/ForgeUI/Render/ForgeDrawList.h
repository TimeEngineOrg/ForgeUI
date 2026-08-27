#pragma once

#include "../Core/ForgeTypes.h"
#include "../Core/ForgeArena.h"

namespace forge {

enum class ForgePipelineType : uint8_t {
    SDF_Shapes = 0,
    MSDF_Text = 1,
    Textured_Quad = 2,
    Count
};

enum class ForgeShapeType : uint8_t {
    Rectangle = 0,
    RoundedRect = 1,
    Circle = 2,
    BorderOnly = 3,
    DropShadow = 4
};

struct alignas(16) ForgeDrawVertex {
    ForgeVec2 pos;
    ForgeVec2 uv;
    uint32_t color;
    ForgeVec4 clipRect;
    ForgeVec4 params;
};

struct ForgeDrawCmd {
    ForgePipelineType pipeline;
    uint8_t shapeType;
    uint16_t reserved;
    uint64_t textureHandle;
    uint32_t elemOffset;
    uint32_t elemCount;
    ForgeVec4 clipRect;
};

class ForgeDrawList {
public:
    ForgeDrawList();
    ~ForgeDrawList();

    void Initialize(ForgeArena* arena, uint32_t maxVertices = 65536, uint32_t maxIndices = 131072, uint32_t maxCommands = 4096);
    void Reset();

    void PushClipRect(const ForgeVec4& clipRect);
    void PopClipRect();
    ForgeVec4 CurrentClipRect() const;

    void AddRect(const ForgeVec2& min, const ForgeVec2& max, uint32_t color);
    void AddRoundedRect(const ForgeVec2& min, const ForgeVec2& max, float cornerRadius, uint32_t color);
    void AddBorder(const ForgeVec2& min, const ForgeVec2& max, float cornerRadius, float borderWidth, uint32_t borderColor);
    void AddCircle(const ForgeVec2& center, float radius, uint32_t color);
    void AddShadow(const ForgeVec2& min, const ForgeVec2& max, float cornerRadius, float shadowBlur, uint32_t shadowColor);

    void AddTexturedQuad(const ForgeVec2& min, const ForgeVec2& max, const ForgeVec2& uvMin, const ForgeVec2& uvMax, uint64_t textureHandle, uint32_t tintColor = 0xFFFFFFFF);
    void AddTextGlyph(const ForgeVec2& min, const ForgeVec2& max, const ForgeVec2& uvMin, const ForgeVec2& uvMax, uint64_t fontAtlasHandle, uint32_t color);

    const ForgeDrawVertex* GetVertices() const { return m_Vertices; }
    const uint32_t* GetIndices() const { return m_Indices; }
    const ForgeDrawCmd* GetCommands() const { return m_Commands; }

    uint32_t GetVertexCount() const { return m_VertexCount; }
    uint32_t GetIndexCount() const { return m_IndexCount; }
    uint32_t GetCommandCount() const { return m_CommandCount; }

private:
    void AddQuad(const ForgeVec2& min, const ForgeVec2& max, const ForgeVec2& uvMin, const ForgeVec2& uvMax, uint32_t color, ForgePipelineType pipeline, uint8_t shapeType, uint64_t textureHandle, const ForgeVec4& params);
    ForgeDrawCmd* GetOrAddCommand(ForgePipelineType pipeline, uint8_t shapeType, uint64_t textureHandle, const ForgeVec4& clipRect);

    ForgeArena* m_Arena;
    ForgeDrawVertex* m_Vertices;
    uint32_t* m_Indices;
    ForgeDrawCmd* m_Commands;

    uint32_t m_VertexCount;
    uint32_t m_IndexCount;
    uint32_t m_CommandCount;

    uint32_t m_MaxVertices;
    uint32_t m_MaxIndices;
    uint32_t m_MaxCommands;

    ForgeVec4 m_ClipStack[32];
    uint32_t m_ClipStackDepth;
};

}
