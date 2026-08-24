#include "ForgeUI/Core/ForgeArena.h"
#include <cstdlib>

namespace forge {

ForgeArena::ForgeArena(size_t capacity) noexcept
    : m_capacity(capacity), m_offset(0) {
    if (capacity > 0) {
#if defined(_MSC_VER)
        m_buffer = static_cast<uint8_t*>(_aligned_malloc(capacity, FORGE_CACHE_LINE));
#else
        void* ptr = nullptr;
        if (posix_memalign(&ptr, FORGE_CACHE_LINE, capacity) == 0) {
            m_buffer = static_cast<uint8_t*>(ptr);
        } else {
            m_buffer = nullptr;
        }
#endif
    }
}

ForgeArena::~ForgeArena() noexcept {
    if (m_buffer) {
#if defined(_MSC_VER)
        _aligned_free(m_buffer);
#else
        free(m_buffer);
#endif
        m_buffer = nullptr;
    }
}

ForgeArena::ForgeArena(ForgeArena&& other) noexcept
    : m_buffer(other.m_buffer), m_capacity(other.m_capacity), m_offset(other.m_offset) {
    other.m_buffer = nullptr;
    other.m_capacity = 0;
    other.m_offset = 0;
}

ForgeArena& ForgeArena::operator=(ForgeArena&& other) noexcept {
    if (this != &other) {
        if (m_buffer) {
#if defined(_MSC_VER)
            _aligned_free(m_buffer);
#else
            free(m_buffer);
#endif
        }
        m_buffer = other.m_buffer;
        m_capacity = other.m_capacity;
        m_offset = other.m_offset;

        other.m_buffer = nullptr;
        other.m_capacity = 0;
        other.m_offset = 0;
    }
    return *this;
}

void* ForgeArena::Allocate(size_t size, size_t alignment) noexcept {
    if (!m_buffer || size == 0) return nullptr;

    if (alignment < alignof(void*)) {
        alignment = alignof(void*);
    }

    uintptr_t currentPtr = reinterpret_cast<uintptr_t>(m_buffer + m_offset);
    uintptr_t alignedPtr = (currentPtr + (alignment - 1)) & ~(alignment - 1);
    size_t padding = alignedPtr - currentPtr;

    if (m_offset + padding + size > m_capacity) {
        return nullptr;
    }

    m_offset += padding + size;
    return reinterpret_cast<void*>(alignedPtr);
}

std::string_view ForgeArena::DuplicateString(std::string_view str) noexcept {
    if (str.empty()) return {};

    void* mem = Allocate(str.size() + 1, alignof(char));
    if (!mem) return {};

    char* dst = static_cast<char*>(mem);
    std::memcpy(dst, str.data(), str.size());
    dst[str.size()] = '\0';

    return std::string_view(dst, str.size());
}

void ForgeArena::Reset() noexcept {
    m_offset = 0;
}

}
