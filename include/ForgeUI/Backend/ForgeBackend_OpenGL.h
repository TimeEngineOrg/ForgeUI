#pragma once

#include "../Render/IForgeBackend.h"
#include "../Render/ForgePipelines.h"

namespace forge {

class ForgeBackend_OpenGL : public IForgeBackend {
public:
    ForgeBackend_OpenGL();
    virtual ~ForgeBackend_OpenGL();

    virtual bool Initialize() override;
    virtual void Shutdown() override;

    virtual void BeginFrame(uint32_t displayWidth, uint32_t displayHeight) override;
    virtual void RenderDrawList(const ForgeDrawList* drawList) override;
    virtual void EndFrame() override;

    virtual uint64_t CreateTexture(uint32_t width, uint32_t height, const uint8_t* pixels, bool isRGBA = true) override;
    virtual void UpdateTexture(uint64_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels) override;
    virtual void DestroyTexture(uint64_t handle) override;

private:
    bool LoadGLFunctions();
    bool CreatePipelineShaders();
    void DestroyPipelineShaders();
    uint32_t CompileShader(uint32_t type, const char* source);
    uint32_t LinkProgram(uint32_t vs, uint32_t fs);

    uint32_t m_DisplayWidth;
    uint32_t m_DisplayHeight;

    uint32_t m_Programs[(size_t)ForgePipelineType::Count];
    int32_t m_LocProj[(size_t)ForgePipelineType::Count];
    int32_t m_LocTexture[(size_t)ForgePipelineType::Count];

    uint32_t m_VAO;
    uint32_t m_VBO;
    uint32_t m_IBO;

    uint32_t m_VBOSize;
    uint32_t m_IBOSize;

    bool m_GLLoaded;
};

}
