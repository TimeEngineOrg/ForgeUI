#pragma once

#include "../Core/ForgeTypes.h"

namespace forge {

struct ForgeGlyphMetrics {
    uint32_t codepoint;
    float advanceX;
    float bearingX;
    float bearingY;
    float width;
    float height;
    float uvMinX;
    float uvMinY;
    float uvMaxX;
    float uvMaxY;
};

class IForgeFontRasterizer {
public:
    virtual ~IForgeFontRasterizer() {}

    virtual bool GetGlyphMetrics(uint32_t codepoint, ForgeGlyphMetrics* outMetrics) = 0;
    virtual float GetLineHeight() const = 0;
    virtual float GetAscent() const = 0;
    virtual float GetDescent() const = 0;
};

}
