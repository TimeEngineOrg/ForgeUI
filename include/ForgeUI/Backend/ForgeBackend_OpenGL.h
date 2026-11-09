#pragma once

#include "../Render/IForgeBackend.h"
#include "../Render/ForgePipelines.h"
#include "../Core/ForgeTypes.h"

namespace forge {

typedef void* (*ForgeGLLoaderFunc)(const char* name);

struct ForgeBackendStats {
    uint32_t drawCallCount = 0;
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    uint32_t vboBytes = 0;
};

struct ForgeGLStateBackup {
    int32_t activeTexture = 0;
    int32_t program = 0;
    int32_t texture2D = 0;
    int32_t vertexArray = 0;
    int32_t arrayBuffer = 0;
    int32_t elementArrayBuffer = 0;
    int32_t viewport[4] = {0, 0, 0, 0};
    int32_t scissorBox[4] = {0, 0, 0, 0};
    int32_t blendSrcRgb = 0;
    int32_t blendDstRgb = 0;
    int32_t blendSrcAlpha = 0;
    int32_t blendDstAlpha = 0;
    int32_t blendEquationRgb = 0;
    int32_t blendEquationAlpha = 0;
    uint8_t enableBlend = 0;
    uint8_t enableCullFace = 0;
    uint8_t enableDepthTest = 0;
    uint8_t enableScissorTest = 0;
};

class ForgeBackend_OpenGL : public IForgeBackend {
public:
    ForgeBackend_OpenGL();
    virtual ~ForgeBackend_OpenGL();

    // Loader configuration (call before Initialize or to override; falls back to platform default)
    void SetCustomLoader(ForgeGLLoaderFunc loader);
    ForgeGLLoaderFunc GetCustomLoader() const { return m_CustomLoader; }

    // Framebuffer auto-clear configuration (default: false to preserve host scene buffers)
    void SetAutoClear(bool enable, const ForgeColor& clearColor = ForgeColor(0.05f, 0.05f, 0.07f, 1.0f));
    bool GetAutoClear() const { return m_AutoClear; }
    const ForgeColor& GetClearColor() const { return m_ClearColor; }

    // Display DPI scaling for high-DPI viewports & scissor rectangles
    void SetDisplayScale(float scaleX, float scaleY);
    void GetDisplayScale(float& outScaleX, float& outScaleY) const { outScaleX = m_ScaleX; outScaleY = m_ScaleY; }

    // Backend telemetry and live draw statistics
    const ForgeBackendStats& GetBackendStats() const { return m_Stats; }

    // State management helpers
    void BackupGLState();
    void RestoreGLState();

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
    float m_ScaleX;
    float m_ScaleY;

    bool m_AutoClear;
    ForgeColor m_ClearColor;
    ForgeGLLoaderFunc m_CustomLoader;
    ForgeBackendStats m_Stats;
    ForgeGLStateBackup m_StateBackup;

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
