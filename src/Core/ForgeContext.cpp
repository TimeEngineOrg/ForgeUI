#include "ForgeUI/Core/ForgeContext.h"
#include "ForgeUI/Layout/ForgeLayoutEngine.h"

namespace forge {

static ForgeContext* g_CurrentContext = nullptr;

ForgeContext::ForgeContext() noexcept
    : m_storage()
    , m_arena(8 * 1024 * 1024)
    , m_idStack()
    , m_input()
    , m_drawList()
    , m_font()
    , m_frameIndex(0)
    , m_rootWidth(1920.0f)
    , m_rootHeight(1080.0f) {
    m_drawList.Initialize(&m_arena);
    m_font.Initialize(&m_arena);
}

void ForgeContext::BeginFrame() noexcept {
    m_frameIndex++;
    m_storage.Reset();
    m_arena.Reset();
    m_idStack.Reset();
    m_drawList.Reset();
    m_input.ResetFrameDeltas();
}

void ForgeContext::EndFrame() noexcept {
    ComputeLayout(m_rootWidth, m_rootHeight);
}

void ForgeContext::ComputeLayout(float rootWidth, float rootHeight) noexcept {
    m_rootWidth = rootWidth;
    m_rootHeight = rootHeight;
    ForgeLayoutEngine::ComputeLayout(m_storage, rootWidth, rootHeight);
}

uint32_t ForgeContext::Begin(std::string_view name, const ForgeElementConfig& config) noexcept {
    ForgeID id = m_idStack.GetID(name);
    m_idStack.Push(name);
    return m_storage.BeginElement(id, config);
}

uint32_t ForgeContext::Begin(ForgeID id, const ForgeElementConfig& config) noexcept {
    m_idStack.Push(id);
    return m_storage.BeginElement(id, config);
}

void ForgeContext::End() noexcept {
    m_idStack.Pop();
    m_storage.EndElement();
}

uint32_t ForgeContext::Element(std::string_view name, const ForgeElementConfig& config) noexcept {
    ForgeID id = m_idStack.GetID(name);
    uint32_t idx = m_storage.BeginElement(id, config);
    m_storage.EndElement();
    return idx;
}

uint32_t ForgeContext::Element(ForgeID id, const ForgeElementConfig& config) noexcept {
    uint32_t idx = m_storage.BeginElement(id, config);
    m_storage.EndElement();
    return idx;
}

ForgeContext* CreateContext() noexcept {
    return new (std::nothrow) ForgeContext();
}

void DestroyContext(ForgeContext* ctx) noexcept {
    delete ctx;
}

void SetCurrentContext(ForgeContext* ctx) noexcept {
    g_CurrentContext = ctx;
}

ForgeContext* GetCurrentContext() noexcept {
    return g_CurrentContext;
}

}
