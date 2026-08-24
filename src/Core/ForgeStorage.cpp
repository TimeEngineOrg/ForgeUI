#include "ForgeUI/Core/ForgeStorage.h"
#include <cstdlib>
#include <cstring>

namespace forge {

ForgeStorage::ForgeStorage() noexcept {
#if defined(_MSC_VER)
    m_layout = static_cast<ForgeLayoutArray*>(_aligned_malloc(sizeof(ForgeLayoutArray), FORGE_CACHE_LINE));
#else
    void* ptr = nullptr;
    if (posix_memalign(&ptr, FORGE_CACHE_LINE, sizeof(ForgeLayoutArray)) == 0) {
        m_layout = static_cast<ForgeLayoutArray*>(ptr);
    } else {
        m_layout = nullptr;
    }
#endif
    Reset();
}

ForgeStorage::~ForgeStorage() noexcept {
    if (m_layout) {
#if defined(_MSC_VER)
        _aligned_free(m_layout);
#else
        free(m_layout);
#endif
        m_layout = nullptr;
    }
}

ForgeStorage::ForgeStorage(ForgeStorage&& other) noexcept
    : m_layout(other.m_layout), m_count(other.m_count), m_stackDepth(other.m_stackDepth) {
    std::memcpy(m_parentStack, other.m_parentStack, sizeof(m_parentStack));
    other.m_layout = nullptr;
    other.m_count = 0;
    other.m_stackDepth = 0;
}

ForgeStorage& ForgeStorage::operator=(ForgeStorage&& other) noexcept {
    if (this != &other) {
        if (m_layout) {
#if defined(_MSC_VER)
            _aligned_free(m_layout);
#else
            free(m_layout);
#endif
        }
        m_layout = other.m_layout;
        m_count = other.m_count;
        m_stackDepth = other.m_stackDepth;
        std::memcpy(m_parentStack, other.m_parentStack, sizeof(m_parentStack));

        other.m_layout = nullptr;
        other.m_count = 0;
        other.m_stackDepth = 0;
    }
    return *this;
}

void ForgeStorage::Reset() noexcept {
    m_count = 0;
    m_stackDepth = 0;
}

uint32_t ForgeStorage::BeginElement(ForgeID id, const ForgeElementConfig& config) noexcept {
    if (!m_layout || m_count >= FORGE_MAX_ELEMENTS) {
        return static_cast<uint32_t>(FORGE_MAX_ELEMENTS - 1);
    }

    uint32_t index = static_cast<uint32_t>(m_count++);
    m_layout->ids[index] = id;
    m_layout->configs[index] = config;
    m_layout->computedWidth[index] = 0.0f;
    m_layout->computedHeight[index] = 0.0f;
    m_layout->computedX[index] = 0.0f;
    m_layout->computedY[index] = 0.0f;
    m_layout->subtreeSize[index] = 1;
    m_layout->childCount[index] = 0;
    m_layout->cullFlags[index] = ForgeCullFlags::None;
    m_layout->clipRects[index] = ForgeRect{0.0f, 0.0f, 0.0f, 0.0f};

    if (m_stackDepth > 0) {
        uint32_t parent = m_parentStack[m_stackDepth - 1];
        m_layout->parentIndex[index] = parent;
        m_layout->childCount[parent] += 1;
    } else {
        m_layout->parentIndex[index] = index;
    }

    if (m_stackDepth < FORGE_MAX_STACK_DEPTH) {
        m_parentStack[m_stackDepth++] = index;
    }

    return index;
}

void ForgeStorage::EndElement() noexcept {
    if (!m_layout || m_stackDepth == 0) return;

    uint32_t index = m_parentStack[--m_stackDepth];
    uint16_t subSize = static_cast<uint16_t>(m_count - index);
    m_layout->subtreeSize[index] = subSize;
}

}
