#pragma once

#include "../Render/IForgeBackend.h"

namespace forge {

class ForgeBackend_CPU : public IForgeBackend {
public:
    ForgeBackend_CPU();
    virtual ~ForgeBackend_CPU();

    virtual bool Initialize() override;
    virtual void Shutdown() override;

    virtual void BeginFrame(uint32_t displayWidth, uint32_t displayHeight) override;
    virtual void RenderDrawList(const ForgeDrawList* drawList) override;
    virtual void EndFrame() override;

    virtual uint64_t CreateTexture(uint32_t width, uint32_t height, const uint8_t* pixels, bool isRGBA = true) override;
    virtual void UpdateTexture(uint64_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels) override;
    virtual void DestroyTexture(uint64_t handle) override;

    const uint32_t* GetFramebuffer() const { return m_Framebuffer; }
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

private:
    uint32_t* m_Framebuffer;
    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_Capacity;
};

}
