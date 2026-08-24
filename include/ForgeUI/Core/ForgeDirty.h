#pragma once

#include "ForgeTypes.h"
#include "ForgeStorage.h"
#include <cstdint>

namespace forge {

enum class ForgeDirtyFlags : uint8_t {
    Clean = 0,
    SizeDirty = 1 << 0,
    OffsetDirty = 1 << 1,
    SubtreeDirty = 1 << 2
};

inline constexpr ForgeDirtyFlags operator|(ForgeDirtyFlags a, ForgeDirtyFlags b) noexcept {
    return static_cast<ForgeDirtyFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline constexpr ForgeDirtyFlags operator&(ForgeDirtyFlags a, ForgeDirtyFlags b) noexcept {
    return static_cast<ForgeDirtyFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline constexpr ForgeDirtyFlags& operator|=(ForgeDirtyFlags& a, ForgeDirtyFlags b) noexcept {
    a = a | b;
    return a;
}

class ForgeDirtySystem {
public:
    static void BubbleUpDirty(ForgeStorage& storage, uint32_t elementIndex) noexcept;
    static void CascadeDirty(ForgeStorage& storage, uint32_t elementIndex) noexcept;
};

}
