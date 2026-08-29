#pragma once

#include "ForgeTypes.h"
#include "ForgeID.h"
#include "ForgeArena.h"
#include "ForgeStorage.h"
#include "ForgeInput.h"
#include "../Render/ForgeDrawList.h"
#include "../Font/ForgeFont_MSDF.h"
#include <string_view>

namespace forge {

class ForgeContext {
public:
    ForgeContext() noexcept;
    ~ForgeContext() noexcept = default;

    ForgeContext(const ForgeContext&) = delete;
    ForgeContext& operator=(const ForgeContext&) = delete;

    ForgeContext(ForgeContext&&) noexcept = default;
    ForgeContext& operator=(ForgeContext&&) noexcept = default;

    void BeginFrame() noexcept;
    void EndFrame() noexcept;

    void SetRootDimensions(float width, float height) noexcept {
        m_rootWidth = width;
        m_rootHeight = height;
    }

    void ComputeLayout(float rootWidth, float rootHeight) noexcept;

    uint32_t Begin(std::string_view name, const ForgeElementConfig& config = {}) noexcept;
    uint32_t Begin(ForgeID id, const ForgeElementConfig& config = {}) noexcept;
    void End() noexcept;

    uint32_t Element(std::string_view name, const ForgeElementConfig& config = {}) noexcept;
    uint32_t Element(ForgeID id, const ForgeElementConfig& config = {}) noexcept;

    void PushID(std::string_view str) noexcept { m_idStack.Push(str); }
    void PushID(int32_t index) noexcept { m_idStack.Push(index); }
    void PushID(ForgeID id) noexcept { m_idStack.Push(id); }
    void PopID() noexcept { m_idStack.Pop(); }

    ForgeID GetID(std::string_view str) const noexcept { return m_idStack.GetID(str); }
    ForgeID GetID(int32_t index) const noexcept { return m_idStack.GetID(index); }

    ForgeStorage& GetStorage() noexcept { return m_storage; }
    const ForgeStorage& GetStorage() const noexcept { return m_storage; }

    ForgeArena& GetArena() noexcept { return m_arena; }
    const ForgeArena& GetArena() const noexcept { return m_arena; }

    ForgeIDStack& GetIDStack() noexcept { return m_idStack; }
    const ForgeIDStack& GetIDStack() const noexcept { return m_idStack; }

    ForgeInputState& GetInput() noexcept { return m_input; }
    const ForgeInputState& GetInput() const noexcept { return m_input; }

    ForgeDrawList& GetDrawList() noexcept { return m_drawList; }
    const ForgeDrawList& GetDrawList() const noexcept { return m_drawList; }

    ForgeFont_MSDF& GetFont() noexcept { return m_font; }
    const ForgeFont_MSDF& GetFont() const noexcept { return m_font; }

    uint64_t FrameIndex() const noexcept { return m_frameIndex; }
    float RootWidth() const noexcept { return m_rootWidth; }
    float RootHeight() const noexcept { return m_rootHeight; }

private:
    ForgeStorage m_storage;
    ForgeArena m_arena;
    ForgeIDStack m_idStack;
    ForgeInputState m_input;
    ForgeDrawList m_drawList;
    ForgeFont_MSDF m_font;

    uint64_t m_frameIndex = 0;
    float m_rootWidth = 0.0f;
    float m_rootHeight = 0.0f;
};

ForgeContext* CreateContext() noexcept;
void DestroyContext(ForgeContext* ctx) noexcept;
void SetCurrentContext(ForgeContext* ctx) noexcept;
ForgeContext* GetCurrentContext() noexcept;

class ForgeScope {
public:
    explicit ForgeScope(std::string_view name, const ForgeElementConfig& config = {}, ForgeContext* ctx = nullptr) noexcept
        : m_ctx(ctx ? ctx : GetCurrentContext()) {
        if (m_ctx) {
            m_ctx->Begin(name, config);
        }
    }

    explicit ForgeScope(ForgeID id, const ForgeElementConfig& config = {}, ForgeContext* ctx = nullptr) noexcept
        : m_ctx(ctx ? ctx : GetCurrentContext()) {
        if (m_ctx) {
            m_ctx->Begin(id, config);
        }
    }

    ~ForgeScope() noexcept {
        if (m_ctx) {
            m_ctx->End();
        }
    }

    ForgeScope(const ForgeScope&) = delete;
    ForgeScope& operator=(const ForgeScope&) = delete;

private:
    ForgeContext* m_ctx = nullptr;
};

}

#define FORGE_CONCAT_INNER(a, b) a##b
#define FORGE_CONCAT(a, b) FORGE_CONCAT_INNER(a, b)
#define FORGE_SCOPE(name, ...) ::forge::ForgeScope FORGE_CONCAT(_forge_scope_, __LINE__)(name, ##__VA_ARGS__)
