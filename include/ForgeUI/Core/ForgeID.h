#pragma once

#include "ForgeTypes.h"
#include <string_view>
#include <span>

namespace forge {

inline constexpr uint64_t FNV1A_64_OFFSET = 14695981039346656037ULL;
inline constexpr uint64_t FNV1A_64_PRIME = 1099511628211ULL;

constexpr uint64_t HashFNV1a64(const char* data, size_t length, uint64_t seed = FNV1A_64_OFFSET) noexcept {
    uint64_t hash = seed;
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<uint8_t>(data[i]));
        hash *= FNV1A_64_PRIME;
    }
    return hash;
}

constexpr uint64_t HashFNV1a64(std::string_view str, uint64_t seed = FNV1A_64_OFFSET) noexcept {
    return HashFNV1a64(str.data(), str.size(), seed);
}

constexpr uint64_t MurmurHash3_64(const char* data, size_t len, uint64_t seed = 0) noexcept {
    const size_t nblocks = len / 8;

    uint64_t h1 = seed;
    const uint64_t c1 = 0x87c37b91114253d5ULL;
    const uint64_t c2 = 0x4cf5ad432745937fULL;

    for (size_t i = 0; i < nblocks; ++i) {
        size_t idx = i * 8;
        uint64_t k1 = static_cast<uint64_t>(static_cast<uint8_t>(data[idx + 0])) |
                     (static_cast<uint64_t>(static_cast<uint8_t>(data[idx + 1])) << 8) |
                     (static_cast<uint64_t>(static_cast<uint8_t>(data[idx + 2])) << 16) |
                     (static_cast<uint64_t>(static_cast<uint8_t>(data[idx + 3])) << 24) |
                     (static_cast<uint64_t>(static_cast<uint8_t>(data[idx + 4])) << 32) |
                     (static_cast<uint64_t>(static_cast<uint8_t>(data[idx + 5])) << 40) |
                     (static_cast<uint64_t>(static_cast<uint8_t>(data[idx + 6])) << 48) |
                     (static_cast<uint64_t>(static_cast<uint8_t>(data[idx + 7])) << 56);

        k1 *= c1;
        k1 = (k1 << 31) | (k1 >> 33);
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 27) | (h1 >> 37);
        h1 = h1 * 5 + 0x52dce729;
    }

    const char* tail = data + nblocks * 8;
    uint64_t k1 = 0;

    switch (len & 7) {
        case 7: k1 ^= static_cast<uint64_t>(static_cast<uint8_t>(tail[6])) << 48; [[fallthrough]];
        case 6: k1 ^= static_cast<uint64_t>(static_cast<uint8_t>(tail[5])) << 40; [[fallthrough]];
        case 5: k1 ^= static_cast<uint64_t>(static_cast<uint8_t>(tail[4])) << 32; [[fallthrough]];
        case 4: k1 ^= static_cast<uint64_t>(static_cast<uint8_t>(tail[3])) << 24; [[fallthrough]];
        case 3: k1 ^= static_cast<uint64_t>(static_cast<uint8_t>(tail[2])) << 16; [[fallthrough]];
        case 2: k1 ^= static_cast<uint64_t>(static_cast<uint8_t>(tail[1])) << 8;  [[fallthrough]];
        case 1: k1 ^= static_cast<uint64_t>(static_cast<uint8_t>(tail[0]));
                k1 *= c1;
                k1 = (k1 << 31) | (k1 >> 33);
                k1 *= c2;
                h1 ^= k1;
    }

    h1 ^= len;
    h1 ^= h1 >> 33;
    h1 *= 0xff51afd7ed558ccdULL;
    h1 ^= h1 >> 33;
    h1 *= 0xc4ceb9fe1a85ec53ULL;
    h1 ^= h1 >> 33;

    return h1;
}

inline uint64_t MurmurHash3_64(const void* key, size_t len, uint64_t seed = 0) noexcept {
    return MurmurHash3_64(static_cast<const char*>(key), len, seed);
}

constexpr uint64_t HashCombine(uint64_t seed, uint64_t value) noexcept {
    return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
}

constexpr ForgeID HashString(std::string_view str, uint64_t seed = 0) noexcept {
    if (str.empty()) return FORGE_INVALID_ID;
    return MurmurHash3_64(str.data(), str.size(), seed);
}

inline ForgeID HashInt(int64_t val, uint64_t seed = 0) noexcept {
    return MurmurHash3_64(&val, sizeof(val), seed);
}

class ForgeIDStack {
public:
    constexpr ForgeIDStack() noexcept {
        Reset();
    }

    constexpr void Reset() noexcept {
        m_depth = 1;
        m_stack[0] = FNV1A_64_OFFSET;
    }

    constexpr void Push(std::string_view str) noexcept {
        if (m_depth < FORGE_MAX_STACK_DEPTH) {
            uint64_t current = m_stack[m_depth - 1];
            uint64_t next = HashString(str, current);
            m_stack[m_depth++] = next;
        }
    }

    void Push(int32_t index) noexcept {
        if (m_depth < FORGE_MAX_STACK_DEPTH) {
            uint64_t current = m_stack[m_depth - 1];
            uint64_t next = HashInt(index, current);
            m_stack[m_depth++] = next;
        }
    }

    constexpr void Push(ForgeID id) noexcept {
        if (m_depth < FORGE_MAX_STACK_DEPTH) {
            uint64_t current = m_stack[m_depth - 1];
            uint64_t next = HashCombine(current, id);
            m_stack[m_depth++] = next;
        }
    }

    constexpr void Pop() noexcept {
        if (m_depth > 1) {
            --m_depth;
        }
    }

    constexpr ForgeID Peek() const noexcept {
        return m_stack[m_depth - 1];
    }

    constexpr ForgeID GetID(std::string_view str) const noexcept {
        return HashString(str, Peek());
    }

    ForgeID GetID(int32_t index) const noexcept {
        return HashInt(index, Peek());
    }

    constexpr size_t Depth() const noexcept {
        return m_depth;
    }

private:
    ForgeID m_stack[FORGE_MAX_STACK_DEPTH]{};
    size_t m_depth = 1;
};

namespace literals {
constexpr ForgeID operator""_forge_id(const char* str, size_t len) noexcept {
    return HashString(std::string_view(str, len));
}
}

}
