#include "ForgeUI/Render/ForgeDrawList.h"
#include <cstring>

namespace forge {

ForgeDrawList::ForgeDrawList()
    : m_Arena(nullptr)
    , m_Vertices(nullptr)
    , m_Indices(nullptr)
    , m_Commands(nullptr)
    , m_VertexCount(0)
    , m_IndexCount(0)
    , m_CommandCount(0)
    , m_MaxVertices(0)
    , m_MaxIndices(0)
    , m_MaxCommands(0)
    , m_ClipStackDepth(0) {
    m_ClipStack[0] = ForgeVec4(-100000.0f, -100000.0f, 100000.0f, 100000.0f);
}

ForgeDrawList::~ForgeDrawList() {}

void ForgeDrawList::Initialize(ForgeArena* arena, uint32_t maxVertices, uint32_t maxIndices, uint32_t maxCommands) {
    m_Arena = arena;
    m_MaxVertices = maxVertices;
    m_MaxIndices = maxIndices;
    m_MaxCommands = maxCommands;

    if (m_Arena) {
        m_Vertices = static_cast<ForgeDrawVertex*>(m_Arena->Allocate(sizeof(ForgeDrawVertex) * m_MaxVertices, 16));
        m_Indices = static_cast<uint32_t*>(m_Arena->Allocate(sizeof(uint32_t) * m_MaxIndices, 16));
        m_Commands = static_cast<ForgeDrawCmd*>(m_Arena->Allocate(sizeof(ForgeDrawCmd) * m_MaxCommands, 16));
    }

    if (!m_Vertices || !m_Indices || !m_Commands) {
        m_MaxVertices = 0;
        m_MaxIndices = 0;
        m_MaxCommands = 0;
        m_Vertices = nullptr;
        m_Indices = nullptr;
        m_Commands = nullptr;
    }

    Reset();
}

void ForgeDrawList::Reset() {
    m_VertexCount = 0;
    m_IndexCount = 0;
    m_CommandCount = 0;
    m_ClipStackDepth = 0;
    m_ClipStack[0] = ForgeVec4(-100000.0f, -100000.0f, 100000.0f, 100000.0f);
}

void ForgeDrawList::PushClipRect(const ForgeVec4& clipRect) {
    if (m_ClipStackDepth < 31) {
        m_ClipStackDepth++;
        ForgeVec4 parent = m_ClipStack[m_ClipStackDepth - 1];
        float minX = clipRect.x > parent.x ? clipRect.x : parent.x;
        float minY = clipRect.y > parent.y ? clipRect.y : parent.y;
        float maxX = clipRect.z < parent.z ? clipRect.z : parent.z;
        float maxY = clipRect.w < parent.w ? clipRect.w : parent.w;
        m_ClipStack[m_ClipStackDepth] = ForgeVec4(minX, minY, maxX, maxY);
    }
}

void ForgeDrawList::PopClipRect() {
    if (m_ClipStackDepth > 0) {
        m_ClipStackDepth--;
    }
}

ForgeVec4 ForgeDrawList::CurrentClipRect() const {
    return m_ClipStack[m_ClipStackDepth];
}

ForgeDrawCmd* ForgeDrawList::GetOrAddCommand(ForgePipelineType pipeline, uint8_t shapeType, uint64_t textureHandle, const ForgeVec4& clipRect) {
    if (m_CommandCount > 0) {
        ForgeDrawCmd* lastCmd = &m_Commands[m_CommandCount - 1];
        if (lastCmd->pipeline == pipeline &&
            lastCmd->shapeType == shapeType &&
            lastCmd->textureHandle == textureHandle &&
            lastCmd->clipRect.x == clipRect.x &&
            lastCmd->clipRect.y == clipRect.y &&
            lastCmd->clipRect.z == clipRect.z &&
            lastCmd->clipRect.w == clipRect.w) {
            return lastCmd;
        }
    }

    if (m_CommandCount < m_MaxCommands) {
        ForgeDrawCmd* cmd = &m_Commands[m_CommandCount++];
        cmd->pipeline = pipeline;
        cmd->shapeType = shapeType;
        cmd->reserved = 0;
        cmd->textureHandle = textureHandle;
        cmd->elemOffset = m_IndexCount;
        cmd->elemCount = 0;
        cmd->clipRect = clipRect;
        return cmd;
    }

    return nullptr;
}

void ForgeDrawList::AddQuad(const ForgeVec2& min, const ForgeVec2& max, const ForgeVec2& uvMin, const ForgeVec2& uvMax, uint32_t color, ForgePipelineType pipeline, uint8_t shapeType, uint64_t textureHandle, const ForgeVec4& params) {
    if (m_VertexCount + 4 > m_MaxVertices || m_IndexCount + 6 > m_MaxIndices) {
        return;
    }

    ForgeVec4 currentClip = CurrentClipRect();
    ForgeDrawCmd* cmd = GetOrAddCommand(pipeline, shapeType, textureHandle, currentClip);
    if (!cmd) {
        return;
    }

    uint32_t vIdx = m_VertexCount;

    m_Vertices[vIdx + 0].pos = ForgeVec2(min.x, min.y);
    m_Vertices[vIdx + 0].uv = ForgeVec2(uvMin.x, uvMin.y);
    m_Vertices[vIdx + 0].color = color;
    m_Vertices[vIdx + 0].clipRect = currentClip;
    m_Vertices[vIdx + 0].params = params;

    m_Vertices[vIdx + 1].pos = ForgeVec2(max.x, min.y);
    m_Vertices[vIdx + 1].uv = ForgeVec2(uvMax.x, uvMin.y);
    m_Vertices[vIdx + 1].color = color;
    m_Vertices[vIdx + 1].clipRect = currentClip;
    m_Vertices[vIdx + 1].params = params;

    m_Vertices[vIdx + 2].pos = ForgeVec2(max.x, max.y);
    m_Vertices[vIdx + 2].uv = ForgeVec2(uvMax.x, uvMax.y);
    m_Vertices[vIdx + 2].color = color;
    m_Vertices[vIdx + 2].clipRect = currentClip;
    m_Vertices[vIdx + 2].params = params;

    m_Vertices[vIdx + 3].pos = ForgeVec2(min.x, max.y);
    m_Vertices[vIdx + 3].uv = ForgeVec2(uvMin.x, uvMax.y);
    m_Vertices[vIdx + 3].color = color;
    m_Vertices[vIdx + 3].clipRect = currentClip;
    m_Vertices[vIdx + 3].params = params;

    uint32_t iIdx = m_IndexCount;
    m_Indices[iIdx + 0] = vIdx + 0;
    m_Indices[iIdx + 1] = vIdx + 1;
    m_Indices[iIdx + 2] = vIdx + 2;
    m_Indices[iIdx + 3] = vIdx + 0;
    m_Indices[iIdx + 4] = vIdx + 2;
    m_Indices[iIdx + 5] = vIdx + 3;

    m_VertexCount += 4;
    m_IndexCount += 6;
    cmd->elemCount += 6;
}

void ForgeDrawList::AddRect(const ForgeVec2& min, const ForgeVec2& max, uint32_t color) {
    AddQuad(min, max, ForgeVec2(0, 0), ForgeVec2(1, 1), color, ForgePipelineType::SDF_Shapes, 0, 0, ForgeVec4(0, 0, 0, 0));
}

void ForgeDrawList::AddRoundedRect(const ForgeVec2& min, const ForgeVec2& max, float cornerRadius, uint32_t color) {
    AddQuad(min, max, ForgeVec2(0, 0), ForgeVec2(1, 1), color, ForgePipelineType::SDF_Shapes, 0, 0, ForgeVec4(cornerRadius, 0, 0, 0));
}

void ForgeDrawList::AddBorder(const ForgeVec2& min, const ForgeVec2& max, float cornerRadius, float borderWidth, uint32_t borderColor) {
    AddQuad(min, max, ForgeVec2(0, 0), ForgeVec2(1, 1), borderColor, ForgePipelineType::SDF_Shapes, 1, 0, ForgeVec4(cornerRadius, borderWidth, 0, 0));
}

void ForgeDrawList::AddCircle(const ForgeVec2& center, float radius, uint32_t color) {
    AddQuad(ForgeVec2(center.x - radius, center.y - radius), ForgeVec2(center.x + radius, center.y + radius), ForgeVec2(0, 0), ForgeVec2(1, 1), color, ForgePipelineType::SDF_Shapes, 2, 0, ForgeVec4(radius, 0, 0, 0));
}

void ForgeDrawList::AddShadow(const ForgeVec2& min, const ForgeVec2& max, float cornerRadius, float shadowBlur, uint32_t shadowColor) {
    ForgeVec2 pMin(min.x - shadowBlur, min.y - shadowBlur);
    ForgeVec2 pMax(max.x + shadowBlur, max.y + shadowBlur);
    AddQuad(pMin, pMax, ForgeVec2(0, 0), ForgeVec2(1, 1), shadowColor, ForgePipelineType::SDF_Shapes, 3, 0, ForgeVec4(cornerRadius, shadowBlur, 0, 0));
}

void ForgeDrawList::AddTexturedQuad(const ForgeVec2& min, const ForgeVec2& max, const ForgeVec2& uvMin, const ForgeVec2& uvMax, uint64_t textureHandle, uint32_t tintColor) {
    AddQuad(min, max, uvMin, uvMax, tintColor, ForgePipelineType::Textured_Quad, 0, textureHandle, ForgeVec4(0, 0, 0, 0));
}

void ForgeDrawList::AddTextGlyph(const ForgeVec2& min, const ForgeVec2& max, const ForgeVec2& uvMin, const ForgeVec2& uvMax, uint64_t fontAtlasHandle, uint32_t color) {
    AddQuad(min, max, uvMin, uvMax, color, ForgePipelineType::MSDF_Text, 0, fontAtlasHandle, ForgeVec4(0, 0, 0, 0));
}

}
