#pragma once

#include "../Render/IForgeBackend.h"

namespace forge {

class ForgeBackend_DX11 : public IForgeBackend {
public:
    ForgeBackend_DX11() {}
    virtual ~ForgeBackend_DX11() {}

    virtual bool Initialize() override { return true; }
    virtual void Shutdown() override {}

    virtual void BeginFrame(uint32_t displayWidth, uint32_t displayHeight) override {
        (void)displayWidth; (void)displayHeight;
    }

    virtual void RenderDrawList(const ForgeDrawList* drawList) override {
        (void)drawList;
    }

    virtual void EndFrame() override {}

    virtual uint64_t CreateTexture(uint32_t width, uint32_t height, const uint8_t* pixels, bool isRGBA = true) override {
        (void)width; (void)height; (void)pixels; (void)isRGBA;
        static uint64_t s_Counter = 1;
        return s_Counter++;
    }

    virtual void UpdateTexture(uint64_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels) override {
        (void)handle; (void)x; (void)y; (void)width; (void)height; (void)pixels;
    }

    virtual void DestroyTexture(uint64_t handle) override {
        (void)handle;
    }
};

}
