#pragma once

#include "ForgeTypes.h"
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string_view>
#include <span>
#include <type_traits>
#include <new>

namespace forge {

class ForgeArena {
public:
    explicit ForgeArena(size_t capacity = 4 * 1024 * 1024) noexcept;
    ~ForgeArena() noexcept;

    ForgeArena(const ForgeArena&) = delete;
    ForgeArena& operator=(const ForgeArena&) = delete;

    ForgeArena(ForgeArena&& other) noexcept;
    ForgeArena& operator=(ForgeArena&& other) noexcept;

    void* Allocate(size_t size, size_t alignment = FORGE_CACHE_LINE) noexcept;

    template <typename T, typename... Args>
    T* Create(Args&&... args) noexcept {
        void* ptr = Allocate(sizeof(T), alignof(T));
        if (!ptr) return nullptr;
        return ::new (ptr) T(static_cast<Args&&>(args)...);
    }

    template <typename T>
    std::span<T> AllocateSpan(size_t count) noexcept {
        void* ptr = Allocate(sizeof(T) * count, alignof(T));
        if (!ptr) return {};
        return std::span<T>(static_cast<T*>(ptr), count);
    }

    std::string_view DuplicateString(std::string_view str) noexcept;

    void Reset() noexcept;

    size_t Capacity() const noexcept { return m_capacity; }
    size_t Used() const noexcept { return m_offset; }
    size_t Remaining() const noexcept { return (m_capacity > m_offset) ? (m_capacity - m_offset) : 0; }

private:
    uint8_t* m_buffer = nullptr;
    size_t m_capacity = 0;
    size_t m_offset = 0;
};

}
