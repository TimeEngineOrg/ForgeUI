#include "ForgeUI/Font/ForgeFont_MSDF.h"
#include <cstring>

namespace forge {

ForgeAtlasAllocator::ForgeAtlasAllocator()
    : m_Arena(nullptr)
    , m_Nodes(nullptr)
    , m_NodeCount(0)
    , m_MaxNodes(0)
    , m_Width(0)
    , m_Height(0) {}

ForgeAtlasAllocator::~ForgeAtlasAllocator() {}

void ForgeAtlasAllocator::Initialize(ForgeArena* arena, uint32_t width, uint32_t height, uint32_t maxNodes) {
    m_Arena = arena;
    m_Width = width;
    m_Height = height;
    m_MaxNodes = maxNodes;
    m_Nodes = static_cast<ForgeAtlasNode*>(m_Arena->Allocate(sizeof(ForgeAtlasNode) * m_MaxNodes, 16));
    Reset();
}

void ForgeAtlasAllocator::Reset() {
    m_NodeCount = 1;
    if (m_Nodes) {
        m_Nodes[0].x = 0;
        m_Nodes[0].y = 0;
        m_Nodes[0].width = static_cast<int16_t>(m_Width);
    }
}

bool ForgeAtlasAllocator::Allocate(uint32_t width, uint32_t height, uint32_t* outX, uint32_t* outY) {
    int bestHeight = static_cast<int>(m_Height);
    int bestIndex = -1;
    int bestWidth = static_cast<int>(m_Width);

    for (uint32_t i = 0; i < m_NodeCount; ++i) {
        int y = m_Nodes[i].y;
        if (m_Nodes[i].x + static_cast<int>(width) > static_cast<int>(m_Width)) {
            continue;
        }

        int maxBottom = y + static_cast<int>(height);
        if (maxBottom > static_cast<int>(m_Height)) {
            continue;
        }

        int shrinkWidth = 0;
        for (uint32_t j = i; j < m_NodeCount && shrinkWidth < static_cast<int>(width); ++j) {
            if (m_Nodes[j].y > y) {
                y = m_Nodes[j].y;
            }
            shrinkWidth += m_Nodes[j].width;
        }

        if (y + static_cast<int>(height) <= static_cast<int>(m_Height)) {
            if (y < bestHeight || (y == bestHeight && m_Nodes[i].width < bestWidth)) {
                bestHeight = y;
                bestIndex = static_cast<int>(i);
                bestWidth = m_Nodes[i].width;
            }
        }
    }

    if (bestIndex == -1) {
        return false;
    }

    *outX = static_cast<uint32_t>(m_Nodes[bestIndex].x);
    *outY = static_cast<uint32_t>(bestHeight);

    if (m_NodeCount >= m_MaxNodes) {
        return false;
    }

    ForgeAtlasNode newNode;
    newNode.x = static_cast<int16_t>(*outX);
    newNode.y = static_cast<int16_t>(bestHeight + height);
    newNode.width = static_cast<int16_t>(width);

    if (m_Nodes[bestIndex].width == static_cast<int16_t>(width)) {
        m_Nodes[bestIndex] = newNode;
    } else {
        m_Nodes[bestIndex].x = static_cast<int16_t>(*outX + width);
        m_Nodes[bestIndex].width = static_cast<int16_t>(m_Nodes[bestIndex].width - width);

        for (uint32_t i = m_NodeCount; i > static_cast<uint32_t>(bestIndex); --i) {
            m_Nodes[i] = m_Nodes[i - 1];
        }
        m_Nodes[bestIndex] = newNode;
        m_NodeCount++;
    }

    return true;
}

ForgeFont_MSDF::ForgeFont_MSDF()
    : m_Arena(nullptr)
    , m_AtlasTextureHandle(0)
    , m_LineHeight(18.0f)
    , m_Ascent(14.0f)
    , m_Descent(4.0f) {
    for (int i = 0; i < 128; ++i) {
        m_StaticGlyphLoaded[i] = false;
    }
}

ForgeFont_MSDF::~ForgeFont_MSDF() {}

void ForgeFont_MSDF::Initialize(ForgeArena* arena, uint64_t atlasTextureHandle) {
    m_Arena = arena;
    m_AtlasTextureHandle = atlasTextureHandle;
    m_LineHeight = 18.0f;
    m_Ascent = 14.0f;
    m_Descent = 4.0f;

    m_AtlasAllocator.Initialize(m_Arena, 512, 512, 512);

    for (int c = 32; c < 127; ++c) {
        ForgeGlyphMetrics m;
        m.codepoint = static_cast<uint32_t>(c);
        m.advanceX = (c == ' ' || c == '.' || c == ':' || c == 'i' || c == 'l') ? 4.0f : ((c >= 'A' && c <= 'Z') ? 10.0f : 8.0f);
        m.bearingX = 0.0f;
        m.bearingY = 12.0f;
        m.width = m.advanceX;
        m.height = 14.0f;

        int row = (c - 32) / 16;
        int col = (c - 32) % 16;
        float uStep = 1.0f / 16.0f;
        float vStep = 1.0f / 16.0f;

        m.uvMinX = static_cast<float>(col) * uStep;
        m.uvMinY = static_cast<float>(row) * vStep;
        m.uvMaxX = m.uvMinX + uStep * 0.8f;
        m.uvMaxY = m.uvMinY + vStep * 0.8f;

        m_StaticGlyphs[c] = m;
        m_StaticGlyphLoaded[c] = true;
    }
}

bool ForgeFont_MSDF::GetGlyphMetrics(uint32_t codepoint, ForgeGlyphMetrics* outMetrics) {
    if (codepoint < 128 && m_StaticGlyphLoaded[codepoint]) {
        *outMetrics = m_StaticGlyphs[codepoint];
        return true;
    }
    return false;
}

ForgeVec2 ForgeFont_MSDF::MeasureText(const char* text, float fontSize) const {
    if (!text) {
        return ForgeVec2(0.0f, 0.0f);
    }

    float scale = fontSize / 14.0f;
    float currentLineWidth = 0.0f;
    float maxLineWidth = 0.0f;
    float totalHeight = m_LineHeight * scale;

    const char* ptr = text;
    while (*ptr) {
        if (*ptr == '\n') {
            if (currentLineWidth > maxLineWidth) {
                maxLineWidth = currentLineWidth;
            }
            currentLineWidth = 0.0f;
            totalHeight += m_LineHeight * scale;
            ptr++;
            continue;
        }

        uint32_t cp = static_cast<uint8_t>(*ptr);
        if (cp < 128 && m_StaticGlyphLoaded[cp]) {
            currentLineWidth += m_StaticGlyphs[cp].advanceX * scale;
        } else {
            currentLineWidth += 8.0f * scale;
        }
        ptr++;
    }

    if (currentLineWidth > maxLineWidth) {
        maxLineWidth = currentLineWidth;
    }

    return ForgeVec2(maxLineWidth, totalHeight);
}

void ForgeFont_MSDF::AppendTextToDrawList(ForgeDrawList* drawList, const char* text, const ForgeVec2& pos, float fontSize, uint32_t color) const {
    if (!drawList || !text) {
        return;
    }

    float scale = fontSize / 14.0f;
    float curX = pos.x;
    float curY = pos.y;

    const char* ptr = text;
    while (*ptr) {
        if (*ptr == '\n') {
            curX = pos.x;
            curY += m_LineHeight * scale;
            ptr++;
            continue;
        }

        uint32_t cp = static_cast<uint8_t>(*ptr);
        if (cp < 128 && m_StaticGlyphLoaded[cp]) {
            const ForgeGlyphMetrics& m = m_StaticGlyphs[cp];
            if (cp != ' ') {
                ForgeVec2 minPos(curX + m.bearingX * scale, curY + (m_Ascent - m.bearingY) * scale);
                ForgeVec2 maxPos(minPos.x + m.width * scale, minPos.y + m.height * scale);
                ForgeVec2 uvMin(m.uvMinX, m.uvMinY);
                ForgeVec2 uvMax(m.uvMaxX, m.uvMaxY);
                drawList->AddTextGlyph(minPos, maxPos, uvMin, uvMax, m_AtlasTextureHandle, color);
            }
            curX += m.advanceX * scale;
        } else {
            curX += 8.0f * scale;
        }
        ptr++;
    }
}

}
