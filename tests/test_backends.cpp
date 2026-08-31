#include "ForgeUI/Backend/ForgeBackend_Null.h"
#include "ForgeUI/Backend/ForgeBackend_CPU.h"
#include "ForgeUI/Backend/ForgeBackend_OpenGL.h"
#include "ForgeUI/Backend/ForgeBackend_DX11.h"
#include "ForgeUI/Backend/ForgeBackend_Vulkan.h"
#include "ForgeUI/Backend/ForgeBackend_Metal.h"
#include "ForgeUI/Render/ForgeDrawList.h"
#include "ForgeUI/Render/ForgePipelines.h"
#include "ForgeUI/Core/ForgeArena.h"
#include <string>
#include <functional>
#include <cstring>

extern void RegisterTest(const std::string& name, std::function<bool()> func);
#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

FORGE_TEST(Backends_NullBackendLifecycle) {
    forge::ForgeArena arena(1024 * 1024);
    forge::ForgeDrawList drawList;
    drawList.Initialize(&arena);

    drawList.AddRect(forge::ForgeVec2(0, 0), forge::ForgeVec2(100, 100), 0xFFFFFFFF);

    forge::ForgeBackend_Null backend;
    if (!backend.Initialize()) return false;

    backend.BeginFrame(800, 600);
    backend.RenderDrawList(&drawList);
    backend.EndFrame();

    if (backend.GetLastCommandCount() != 1) return false;
    if (backend.GetLastVertexCount() != 4) return false;

    uint64_t tex = backend.CreateTexture(64, 64, nullptr);
    if (tex == 0) return false;
    backend.DestroyTexture(tex);

    backend.Shutdown();
    return true;
}

FORGE_TEST(Backends_CPUSoftwareRasterizer) {
    forge::ForgeArena arena(1024 * 1024);
    forge::ForgeDrawList drawList;
    drawList.Initialize(&arena);

    drawList.AddRect(forge::ForgeVec2(10, 10), forge::ForgeVec2(20, 20), 0xFF00FF00);

    forge::ForgeBackend_CPU cpuBackend;
    if (!cpuBackend.Initialize()) return false;

    cpuBackend.BeginFrame(100, 100);
    cpuBackend.RenderDrawList(&drawList);
    cpuBackend.EndFrame();

    const uint32_t* fb = cpuBackend.GetFramebuffer();
    if (!fb) return false;

    uint32_t insidePixel = fb[15 * 100 + 15];
    uint32_t outsidePixel = fb[5 * 100 + 5];

    if (insidePixel != 0xFF00FF00) return false;
    if (outsidePixel != 0) return false;

    cpuBackend.Shutdown();
    return true;
}

FORGE_TEST(Backends_OpenGLAndESValidation) {
    forge::ForgeBackend_OpenGL glBackend;
    if (!glBackend.Initialize()) return false;

    forge::ForgeShaderSource fontShader = forge::ForgePipelines::GetShaderSource(forge::ForgePipelineType::MSDF_Text);
    forge::ForgeShaderSource shapeShader = forge::ForgePipelines::GetShaderSource(forge::ForgePipelineType::SDF_Shapes);

    if (!fontShader.vertexSource || std::strlen(fontShader.vertexSource) == 0) return false;
    if (!shapeShader.fragmentSource || std::strlen(shapeShader.fragmentSource) == 0) return false;

    uint64_t tex = glBackend.CreateTexture(32, 32, nullptr, true);
    if (tex == 0) return false;
    glBackend.UpdateTexture(tex, 0, 0, 16, 16, nullptr);
    glBackend.DestroyTexture(tex);

    glBackend.Shutdown();
    return true;
}

FORGE_TEST(Backends_VulkanPipelineLayoutValidation) {
    forge::ForgeBackend_Vulkan vkBackend;
    if (!vkBackend.Initialize()) return false;

    vkBackend.BeginFrame(1920, 1080);
    vkBackend.RenderDrawList(nullptr);
    vkBackend.EndFrame();

    uint64_t tex = vkBackend.CreateTexture(64, 64, nullptr);
    if (tex == 0) return false;
    vkBackend.UpdateTexture(tex, 0, 0, 32, 32, nullptr);
    vkBackend.DestroyTexture(tex);

    vkBackend.Shutdown();
    return true;
}

FORGE_TEST(Backends_MetalAndDX11Validation) {
    forge::ForgeBackend_Metal metalBackend;
    if (!metalBackend.Initialize()) return false;

    metalBackend.BeginFrame(1920, 1080);
    metalBackend.RenderDrawList(nullptr);
    metalBackend.EndFrame();

    uint64_t mTex = metalBackend.CreateTexture(128, 128, nullptr);
    if (mTex == 0) return false;
    metalBackend.DestroyTexture(mTex);
    metalBackend.Shutdown();

    forge::ForgeBackend_DX11 dxBackend;
    if (!dxBackend.Initialize()) return false;

    dxBackend.BeginFrame(1920, 1080);
    dxBackend.RenderDrawList(nullptr);
    dxBackend.EndFrame();

    uint64_t dxTex = dxBackend.CreateTexture(128, 128, nullptr);
    if (dxTex == 0) return false;
    dxBackend.DestroyTexture(dxTex);
    dxBackend.Shutdown();

    return true;
}

void RegisterBackendTests() {
    RegisterTest("Backends_NullBackendLifecycle", Test_Backends_NullBackendLifecycle);
    RegisterTest("Backends_CPUSoftwareRasterizer", Test_Backends_CPUSoftwareRasterizer);
    RegisterTest("Backends_OpenGLAndESValidation", Test_Backends_OpenGLAndESValidation);
    RegisterTest("Backends_VulkanPipelineLayoutValidation", Test_Backends_VulkanPipelineLayoutValidation);
    RegisterTest("Backends_MetalAndDX11Validation", Test_Backends_MetalAndDX11Validation);
}
