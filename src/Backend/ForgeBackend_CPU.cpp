#include "ForgeUI/Backend/ForgeBackend_CPU.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace forge {

ForgeBackend_CPU::ForgeBackend_CPU()
    : m_Framebuffer(nullptr)
    , m_Width(0)
    , m_Height(0)
    , m_Capacity(0) {}

ForgeBackend_CPU::~ForgeBackend_CPU() {
    Shutdown();
}

bool ForgeBackend_CPU::Initialize() {
    return true;
}

void ForgeBackend_CPU::Shutdown() {
    if (m_Framebuffer) {
        std::free(m_Framebuffer);
        m_Framebuffer = nullptr;
    }
    m_Width = 0;
    m_Height = 0;
    m_Capacity = 0;
}

void ForgeBackend_CPU::BeginFrame(uint32_t displayWidth, uint32_t displayHeight) {
    m_Width = displayWidth;
    m_Height = displayHeight;
    uint32_t reqSize = displayWidth * displayHeight;

    if (reqSize > m_Capacity) {
        if (m_Framebuffer) {
            std::free(m_Framebuffer);
        }
        m_Framebuffer = static_cast<uint32_t*>(std::malloc(sizeof(uint32_t) * reqSize));
        m_Capacity = reqSize;
    }

    if (m_Framebuffer && reqSize > 0) {
        std::memset(m_Framebuffer, 0, sizeof(uint32_t) * reqSize);
    }
}

void ForgeBackend_CPU::RenderDrawList(const ForgeDrawList* drawList) {
    if (!drawList || !m_Framebuffer) return;

    const ForgeDrawVertex* vertices = drawList->GetVertices();
    const uint32_t* indices = drawList->GetIndices();
    const ForgeDrawCmd* commands = drawList->GetCommands();
    uint32_t cmdCount = drawList->GetCommandCount();

    for (uint32_t c = 0; c < cmdCount; ++c) {
        const ForgeDrawCmd& cmd = commands[c];
        for (uint32_t i = 0; i < cmd.elemCount; i += 6) {
            uint32_t i0 = indices[cmd.elemOffset + i + 0];
            uint32_t i1 = indices[cmd.elemOffset + i + 1];
            uint32_t i2 = indices[cmd.elemOffset + i + 2];
            uint32_t i5 = indices[cmd.elemOffset + i + 5];

            const ForgeDrawVertex& v0 = vertices[i0];
            const ForgeDrawVertex& v1 = vertices[i1];
            const ForgeDrawVertex& v2 = vertices[i2];
            const ForgeDrawVertex& v3 = vertices[i5];

            float vxMin = std::min(std::min(v0.pos.x, v1.pos.x), std::min(v2.pos.x, v3.pos.x));
            float vyMin = std::min(std::min(v0.pos.y, v1.pos.y), std::min(v2.pos.y, v3.pos.y));
            float vxMax = std::max(std::max(v0.pos.x, v1.pos.x), std::max(v2.pos.x, v3.pos.x));
            float vyMax = std::max(std::max(v0.pos.y, v1.pos.y), std::max(v2.pos.y, v3.pos.y));

            int minX = std::max(0, static_cast<int>(vxMin));
            int minY = std::max(0, static_cast<int>(vyMin));
            int maxX = std::min(static_cast<int>(m_Width), static_cast<int>(vxMax));
            int maxY = std::min(static_cast<int>(m_Height), static_cast<int>(vyMax));

            float cx0 = v0.clipRect.x;
            float cy0 = v0.clipRect.y;
            float cx1 = v0.clipRect.z;
            float cy1 = v0.clipRect.w;

            int clipMinX = cx0 > 0.0f ? static_cast<int>(cx0) : 0;
            int clipMinY = cy0 > 0.0f ? static_cast<int>(cy0) : 0;
            int clipMaxX = cx1 < static_cast<float>(m_Width) ? static_cast<int>(cx1) : static_cast<int>(m_Width);
            int clipMaxY = cy1 < static_cast<float>(m_Height) ? static_cast<int>(cy1) : static_cast<int>(m_Height);

            minX = std::max(minX, clipMinX);
            minY = std::max(minY, clipMinY);
            maxX = std::min(maxX, clipMaxX);
            maxY = std::min(maxY, clipMaxY);

            uint32_t col = v0.color;

            for (int y = minY; y < maxY; ++y) {
                uint32_t* row = &m_Framebuffer[y * m_Width];
                for (int x = minX; x < maxX; ++x) {
                    row[x] = col;
                }
            }
        }
    }
}

void ForgeBackend_CPU::EndFrame() {}

uint64_t ForgeBackend_CPU::CreateTexture(uint32_t width, uint32_t height, const uint8_t* pixels, bool isRGBA) {
    (void)width; (void)height; (void)pixels; (void)isRGBA;
    static uint64_t s_Counter = 1;
    return s_Counter++;
}

void ForgeBackend_CPU::UpdateTexture(uint64_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels) {
    (void)handle; (void)x; (void)y; (void)width; (void)height; (void)pixels;
}

void ForgeBackend_CPU::DestroyTexture(uint64_t handle) {
    (void)handle;
}

}
