#pragma once

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <algorithm>

namespace forge {

inline constexpr size_t FORGE_MAX_ELEMENTS = 65536;
inline constexpr size_t FORGE_MAX_STACK_DEPTH = 128;
inline constexpr size_t FORGE_CACHE_LINE = 64;

using ForgeID = uint64_t;
inline constexpr ForgeID FORGE_INVALID_ID = 0;

struct ForgeVec2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr ForgeVec2() noexcept = default;
    constexpr ForgeVec2(float x_, float y_) noexcept : x(x_), y(y_) {}

    constexpr ForgeVec2 operator+(const ForgeVec2& rhs) const noexcept { return {x + rhs.x, y + rhs.y}; }
    constexpr ForgeVec2 operator-(const ForgeVec2& rhs) const noexcept { return {x - rhs.x, y - rhs.y}; }
    constexpr ForgeVec2 operator*(float scalar) const noexcept { return {x * scalar, y * scalar}; }
    constexpr ForgeVec2 operator/(float scalar) const noexcept { return {x / scalar, y / scalar}; }

    constexpr ForgeVec2& operator+=(const ForgeVec2& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
    constexpr ForgeVec2& operator-=(const ForgeVec2& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
    constexpr ForgeVec2& operator*=(float scalar) noexcept { x *= scalar; y *= scalar; return *this; }
    constexpr ForgeVec2& operator/=(float scalar) noexcept { x /= scalar; y /= scalar; return *this; }

    constexpr bool operator==(const ForgeVec2& rhs) const noexcept { return x == rhs.x && y == rhs.y; }
    constexpr bool operator!=(const ForgeVec2& rhs) const noexcept { return !(*this == rhs); }
};

struct ForgeVec4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    constexpr ForgeVec4() noexcept = default;
    constexpr ForgeVec4(float x_, float y_, float z_, float w_) noexcept : x(x_), y(y_), z(z_), w(w_) {}

    constexpr ForgeVec4 operator+(const ForgeVec4& rhs) const noexcept { return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w}; }
    constexpr ForgeVec4 operator-(const ForgeVec4& rhs) const noexcept { return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w}; }
    constexpr ForgeVec4 operator*(float scalar) const noexcept { return {x * scalar, y * scalar, z * scalar, w * scalar}; }

    constexpr bool operator==(const ForgeVec4& rhs) const noexcept { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
    constexpr bool operator!=(const ForgeVec4& rhs) const noexcept { return !(*this == rhs); }
};

struct ForgeRect {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    constexpr ForgeRect() noexcept = default;
    constexpr ForgeRect(float x_, float y_, float w_, float h_) noexcept : x(x_), y(y_), w(w_), h(h_) {}

    constexpr float MinX() const noexcept { return x; }
    constexpr float MinY() const noexcept { return y; }
    constexpr float MaxX() const noexcept { return x + w; }
    constexpr float MaxY() const noexcept { return y + h; }

    constexpr ForgeVec2 Position() const noexcept { return {x, y}; }
    constexpr ForgeVec2 Size() const noexcept { return {w, h}; }

    constexpr bool Contains(ForgeVec2 point) const noexcept {
        return point.x >= x && point.x <= (x + w) && point.y >= y && point.y <= (y + h);
    }

    constexpr bool Overlaps(const ForgeRect& other) const noexcept {
        return !(MaxX() <= other.MinX() || MinX() >= other.MaxX() || MaxY() <= other.MinY() || MinY() >= other.MaxY());
    }

    constexpr ForgeRect Intersect(const ForgeRect& other) const noexcept {
        float nx = std::max(MinX(), other.MinX());
        float ny = std::max(MinY(), other.MinY());
        float mx = std::min(MaxX(), other.MaxX());
        float my = std::min(MaxY(), other.MaxY());
        return (mx > nx && my > ny) ? ForgeRect{nx, ny, mx - nx, my - ny} : ForgeRect{0.0f, 0.0f, 0.0f, 0.0f};
    }

    constexpr bool operator==(const ForgeRect& rhs) const noexcept {
        return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h;
    }
    constexpr bool operator!=(const ForgeRect& rhs) const noexcept { return !(*this == rhs); }
};

struct ForgeColor {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;

    constexpr ForgeColor() noexcept = default;
    constexpr ForgeColor(float r_, float g_, float b_, float a_ = 1.0f) noexcept : r(r_), g(g_), b(b_), a(a_) {}

    static constexpr ForgeColor FromRGBA8(uint8_t r8, uint8_t g8, uint8_t b8, uint8_t a8 = 255) noexcept {
        return {r8 / 255.0f, g8 / 255.0f, b8 / 255.0f, a8 / 255.0f};
    }

    static constexpr ForgeColor FromHex(uint32_t hex) noexcept {
        return {
            ((hex >> 24) & 0xFF) / 255.0f,
            ((hex >> 16) & 0xFF) / 255.0f,
            ((hex >> 8) & 0xFF) / 255.0f,
            (hex & 0xFF) / 255.0f
        };
    }

    constexpr uint32_t ToRGBA8() const noexcept {
        auto clamp255 = [](float v) constexpr -> uint32_t {
            if (v <= 0.0f) return 0;
            if (v >= 1.0f) return 255;
            return static_cast<uint32_t>(v * 255.0f + 0.5f);
        };
        return (clamp255(r) << 24) | (clamp255(g) << 16) | (clamp255(b) << 8) | clamp255(a);
    }

    constexpr bool operator==(const ForgeColor& rhs) const noexcept {
        return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
    }
    constexpr bool operator!=(const ForgeColor& rhs) const noexcept { return !(*this == rhs); }
};

enum class ForgeSizingMode : uint8_t {
    FitContent = 0,
    Fixed,
    Percent,
    Grow
};

enum class ForgeFlexDirection : uint8_t {
    Row = 0,
    Column
};

enum class ForgeAlignment : uint8_t {
    Start = 0,
    Center,
    End,
    Stretch
};

enum class ForgeJustify : uint8_t {
    Start = 0,
    Center,
    End,
    SpaceBetween,
    SpaceAround
};

enum class ForgeCullFlags : uint8_t {
    None = 0,
    Offscreen = 1 << 0,
    ZeroSize = 1 << 1,
    Hidden = 1 << 2
};

inline constexpr ForgeCullFlags operator|(ForgeCullFlags a, ForgeCullFlags b) noexcept {
    return static_cast<ForgeCullFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline constexpr ForgeCullFlags operator&(ForgeCullFlags a, ForgeCullFlags b) noexcept {
    return static_cast<ForgeCullFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

struct ForgeDimension {
    ForgeSizingMode mode = ForgeSizingMode::FitContent;
    float value = 0.0f;

    static constexpr ForgeDimension Fit() noexcept { return {ForgeSizingMode::FitContent, 0.0f}; }
    static constexpr ForgeDimension Px(float px) noexcept { return {ForgeSizingMode::Fixed, px}; }
    static constexpr ForgeDimension Pct(float pct) noexcept { return {ForgeSizingMode::Percent, pct}; }
    static constexpr ForgeDimension FlexGrow(float weight = 1.0f) noexcept { return {ForgeSizingMode::Grow, weight}; }
    static constexpr ForgeDimension Grow(float weight = 1.0f) noexcept { return {ForgeSizingMode::Grow, weight}; }
};

struct ForgeLayoutConfig {
    ForgeDimension width = ForgeDimension::Fit();
    ForgeDimension height = ForgeDimension::Fit();
    ForgeVec4 padding = {0.0f, 0.0f, 0.0f, 0.0f};
    ForgeVec4 margin = {0.0f, 0.0f, 0.0f, 0.0f};
    float gap = 0.0f;
    ForgeFlexDirection direction = ForgeFlexDirection::Column;
    ForgeAlignment alignment = ForgeAlignment::Start;
    ForgeJustify justify = ForgeJustify::Start;
    bool clipContent = false;
};

struct ForgeStyleConfig {
    ForgeColor backgroundColor = {0.0f, 0.0f, 0.0f, 0.0f};
    ForgeColor borderColor = {0.0f, 0.0f, 0.0f, 0.0f};
    ForgeColor shadowColor = {0.0f, 0.0f, 0.0f, 0.0f};
    ForgeVec4 cornerRadius = {0.0f, 0.0f, 0.0f, 0.0f};
    float borderWidth = 0.0f;
    ForgeVec2 shadowOffset = {0.0f, 0.0f};
    float shadowBlur = 0.0f;
};

struct ForgeElementConfig {
    ForgeLayoutConfig layout;
    ForgeStyleConfig style;
};

}
