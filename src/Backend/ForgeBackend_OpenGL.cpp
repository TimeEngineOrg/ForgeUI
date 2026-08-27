#include "ForgeUI/Backend/ForgeBackend_OpenGL.h"
#include <cstdlib>
#include <cstring>

namespace forge {

ForgeBackend_OpenGL::ForgeBackend_OpenGL()
    : m_DisplayWidth(1920)
    , m_DisplayHeight(1080)
    , m_VAO(0)
    , m_VBO(0)
    , m_IBO(0)
    , m_VBOSize(0)
    , m_IBOSize(0) {
    for (size_t i = 0; i < static_cast<size_t>(ForgePipelineType::Count); ++i) {
        m_Programs[i] = 0;
        m_LocProj[i] = -1;
        m_LocTexture[i] = -1;
    }
}

ForgeBackend_OpenGL::~ForgeBackend_OpenGL() {
    Shutdown();
}

bool ForgeBackend_OpenGL::CreatePipelineShaders() {
    return true;
}

void ForgeBackend_OpenGL::DestroyPipelineShaders() {
    for (size_t i = 0; i < static_cast<size_t>(ForgePipelineType::Count); ++i) {
        m_Programs[i] = 0;
    }
}

bool ForgeBackend_OpenGL::Initialize() {
    if (!CreatePipelineShaders()) {
        return false;
    }
    return true;
}

void ForgeBackend_OpenGL::Shutdown() {
    DestroyPipelineShaders();
}

void ForgeBackend_OpenGL::BeginFrame(uint32_t displayWidth, uint32_t displayHeight) {
    m_DisplayWidth = displayWidth;
    m_DisplayHeight = displayHeight;
}

void ForgeBackend_OpenGL::RenderDrawList(const ForgeDrawList* drawList) {
    if (!drawList || drawList->GetCommandCount() == 0) {
        return;
    }
}

void ForgeBackend_OpenGL::EndFrame() {}

uint64_t ForgeBackend_OpenGL::CreateTexture(uint32_t width, uint32_t height, const uint8_t* pixels, bool isRGBA) {
    (void)width;
    (void)height;
    (void)pixels;
    (void)isRGBA;
    static uint64_t s_TextureIdCounter = 1;
    return s_TextureIdCounter++;
}

void ForgeBackend_OpenGL::UpdateTexture(uint64_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels) {
    (void)handle;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)pixels;
}

void ForgeBackend_OpenGL::DestroyTexture(uint64_t handle) {
    (void)handle;
}

}
