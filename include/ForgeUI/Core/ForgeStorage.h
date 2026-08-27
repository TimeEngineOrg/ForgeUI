#pragma once

#include "ForgeTypes.h"
#include <cstdint>
#include <cstddef>

namespace forge {

struct alignas(FORGE_CACHE_LINE) ForgeLayoutArray {
    float computedWidth[FORGE_MAX_ELEMENTS];
    float computedHeight[FORGE_MAX_ELEMENTS];
    float computedX[FORGE_MAX_ELEMENTS];
    float computedY[FORGE_MAX_ELEMENTS];

    uint16_t subtreeSize[FORGE_MAX_ELEMENTS];
    uint16_t childCount[FORGE_MAX_ELEMENTS];
    uint32_t parentIndex[FORGE_MAX_ELEMENTS];

    ForgeCullFlags cullFlags[FORGE_MAX_ELEMENTS];
    ForgeID ids[FORGE_MAX_ELEMENTS];
    ForgeRect clipRects[FORGE_MAX_ELEMENTS];
    ForgeElementConfig configs[FORGE_MAX_ELEMENTS];
};

class ForgeStorage {
public:
    ForgeStorage() noexcept;
    ~ForgeStorage() noexcept;

    ForgeStorage(const ForgeStorage&) = delete;
    ForgeStorage& operator=(const ForgeStorage&) = delete;

    ForgeStorage(ForgeStorage&& other) noexcept;
    ForgeStorage& operator=(ForgeStorage&& other) noexcept;

    void Reset() noexcept;

    uint32_t BeginElement(ForgeID id, const ForgeElementConfig& config) noexcept;
    void EndElement() noexcept;

    size_t Count() const noexcept { return m_count; }
    size_t Capacity() const noexcept { return FORGE_MAX_ELEMENTS; }

    uint32_t NextSiblingIndex(uint32_t index) const noexcept {
        if (!m_layout || index >= m_count) return static_cast<uint32_t>(m_count);
        return index + m_layout->subtreeSize[index];
    }

    ForgeLayoutArray& Layout() noexcept { return *m_layout; }
    const ForgeLayoutArray& Layout() const noexcept { return *m_layout; }

    float GetWidth(uint32_t i) const noexcept { return m_layout ? m_layout->computedWidth[i] : 0.0f; }
    float GetHeight(uint32_t i) const noexcept { return m_layout ? m_layout->computedHeight[i] : 0.0f; }
    float GetX(uint32_t i) const noexcept { return m_layout ? m_layout->computedX[i] : 0.0f; }
    float GetY(uint32_t i) const noexcept { return m_layout ? m_layout->computedY[i] : 0.0f; }
    uint16_t GetSubtreeSize(uint32_t i) const noexcept { return m_layout ? m_layout->subtreeSize[i] : 0; }
    uint16_t GetChildCount(uint32_t i) const noexcept { return m_layout ? m_layout->childCount[i] : 0; }
    uint32_t GetParentIndex(uint32_t i) const noexcept { return m_layout ? m_layout->parentIndex[i] : 0; }
    ForgeID GetID(uint32_t i) const noexcept { return m_layout ? m_layout->ids[i] : FORGE_INVALID_ID; }
    ForgeCullFlags GetCullFlags(uint32_t i) const noexcept { return m_layout ? m_layout->cullFlags[i] : ForgeCullFlags::None; }
    const ForgeRect& GetClipRect(uint32_t i) const noexcept {
        static const ForgeRect defaultRect{0.0f, 0.0f, 0.0f, 0.0f};
        return m_layout ? m_layout->clipRects[i] : defaultRect;
    }
    const ForgeElementConfig& GetConfig(uint32_t i) const noexcept {
        static const ForgeElementConfig defaultConfig{};
        return m_layout ? m_layout->configs[i] : defaultConfig;
    }

    void SetWidth(uint32_t i, float v) noexcept { if (m_layout) m_layout->computedWidth[i] = v; }
    void SetHeight(uint32_t i, float v) noexcept { if (m_layout) m_layout->computedHeight[i] = v; }
    void SetX(uint32_t i, float v) noexcept { if (m_layout) m_layout->computedX[i] = v; }
    void SetY(uint32_t i, float v) noexcept { if (m_layout) m_layout->computedY[i] = v; }
    void SetCullFlags(uint32_t i, ForgeCullFlags flags) noexcept { if (m_layout) m_layout->cullFlags[i] = flags; }
    void SetClipRect(uint32_t i, const ForgeRect& rect) noexcept { if (m_layout) m_layout->clipRects[i] = rect; }

private:
    ForgeLayoutArray* m_layout = nullptr;
    size_t m_count = 0;
    uint32_t m_parentStack[FORGE_MAX_STACK_DEPTH]{};
    size_t m_stackDepth = 0;
};

}
