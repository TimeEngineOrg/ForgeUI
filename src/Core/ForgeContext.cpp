#include "ForgeUI/Core/ForgeContext.h"
#include "ForgeUI/Layout/ForgeLayoutEngine.h"

namespace forge {

namespace {
thread_local ForgeContext* g_currentContext = nullptr;
}

ForgeContext::ForgeContext() noexcept
    : m_arena(8 * 1024 * 1024) {
    if (!g_currentContext) {
        g_currentContext = this;
    }
}

void ForgeContext::BeginFrame() noexcept {
    m_storage.Reset();
    m_arena.Reset();
    m_idStack.Reset();
    ++m_frameIndex;
}

void ForgeContext::EndFrame() noexcept {
    if (m_rootWidth > 0.0f && m_rootHeight > 0.0f) {
        ComputeLayout(m_rootWidth, m_rootHeight);
    }
}

void ForgeContext::ComputeLayout(float rootWidth, float rootHeight) noexcept {
    m_rootWidth = rootWidth;
    m_rootHeight = rootHeight;
    ForgeLayoutEngine::ComputeLayout(m_storage, rootWidth, rootHeight);
}

uint32_t ForgeContext::Begin(std::string_view name, const ForgeElementConfig& config) noexcept {
    ForgeID id = m_idStack.GetID(name);
    m_idStack.Push(id);
    return m_storage.BeginElement(id, config);
}

uint32_t ForgeContext::Begin(ForgeID id, const ForgeElementConfig& config) noexcept {
    m_idStack.Push(id);
    return m_storage.BeginElement(id, config);
}

void ForgeContext::End() noexcept {
    m_storage.EndElement();
    m_idStack.Pop();
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
    if (g_currentContext == ctx) {
        g_currentContext = nullptr;
    }
}

void SetCurrentContext(ForgeContext* ctx) noexcept {
    g_currentContext = ctx;
}

ForgeContext* GetCurrentContext() noexcept {
    return g_currentContext;
}

}
