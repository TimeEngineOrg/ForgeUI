#pragma once

#include "ForgeDrawList.h"

namespace forge {

class IForgeBackend {
public:
    virtual ~IForgeBackend() {}

    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    virtual void BeginFrame(uint32_t displayWidth, uint32_t displayHeight) = 0;
    virtual void RenderDrawList(const ForgeDrawList* drawList) = 0;
    virtual void EndFrame() = 0;

    virtual uint64_t CreateTexture(uint32_t width, uint32_t height, const uint8_t* pixels, bool isRGBA = true) = 0;
    virtual void UpdateTexture(uint64_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels) = 0;
    virtual void DestroyTexture(uint64_t handle) = 0;
};

}
