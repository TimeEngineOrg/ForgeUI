#pragma once

#include "ForgeTypes.h"

namespace forge {

struct ForgeTheme {
    ForgeColor background{0.08f, 0.09f, 0.11f, 1.0f};
    ForgeColor panelBg{0.13f, 0.14f, 0.17f, 0.95f};
    ForgeColor panelHeader{0.16f, 0.18f, 0.22f, 1.0f};
    ForgeColor widgetBase{0.18f, 0.20f, 0.24f, 1.0f};
    ForgeColor widgetHover{0.24f, 0.28f, 0.35f, 1.0f};
    ForgeColor widgetActive{0.20f, 0.45f, 0.85f, 1.0f};
    ForgeColor accent{0.18f, 0.52f, 0.92f, 1.0f};
    ForgeColor accentHover{0.25f, 0.60f, 1.0f, 1.0f};
    ForgeColor textPrimary{0.95f, 0.96f, 0.98f, 1.0f};
    ForgeColor textMuted{0.60f, 0.64f, 0.70f, 1.0f};
    ForgeColor border{0.22f, 0.25f, 0.30f, 1.0f};
    ForgeColor success{0.20f, 0.75f, 0.40f, 1.0f};
    ForgeColor warning{0.95f, 0.65f, 0.15f, 1.0f};
    ForgeColor error{0.90f, 0.25f, 0.25f, 1.0f};

    float windowRounding{6.0f};
    float widgetRounding{4.0f};
    float borderWidth{1.0f};
    float itemSpacing{8.0f};
    float scrollbarWidth{8.0f};

    static ForgeTheme Dark() noexcept {
        return ForgeTheme{};
    }

    static ForgeTheme Light() noexcept {
        ForgeTheme t;
        t.background = ForgeColor{0.92f, 0.93f, 0.95f, 1.0f};
        t.panelBg = ForgeColor{0.98f, 0.98f, 0.99f, 0.98f};
        t.panelHeader = ForgeColor{0.88f, 0.90f, 0.94f, 1.0f};
        t.widgetBase = ForgeColor{0.84f, 0.86f, 0.90f, 1.0f};
        t.widgetHover = ForgeColor{0.78f, 0.82f, 0.88f, 1.0f};
        t.widgetActive = ForgeColor{0.15f, 0.45f, 0.85f, 1.0f};
        t.accent = ForgeColor{0.15f, 0.45f, 0.85f, 1.0f};
        t.accentHover = ForgeColor{0.20f, 0.55f, 0.95f, 1.0f};
        t.textPrimary = ForgeColor{0.10f, 0.12f, 0.15f, 1.0f};
        t.textMuted = ForgeColor{0.45f, 0.50f, 0.55f, 1.0f};
        t.border = ForgeColor{0.75f, 0.78f, 0.84f, 1.0f};
        return t;
    }

    static ForgeTheme Cyberpunk() noexcept {
        ForgeTheme t;
        t.background = ForgeColor{0.05f, 0.05f, 0.08f, 1.0f};
        t.panelBg = ForgeColor{0.09f, 0.08f, 0.14f, 0.96f};
        t.panelHeader = ForgeColor{0.14f, 0.11f, 0.22f, 1.0f};
        t.widgetBase = ForgeColor{0.15f, 0.12f, 0.25f, 1.0f};
        t.widgetHover = ForgeColor{0.25f, 0.18f, 0.40f, 1.0f};
        t.widgetActive = ForgeColor{0.95f, 0.10f, 0.50f, 1.0f};
        t.accent = ForgeColor{0.00f, 0.95f, 0.85f, 1.0f};
        t.accentHover = ForgeColor{0.20f, 1.0f, 0.90f, 1.0f};
        t.textPrimary = ForgeColor{0.95f, 0.95f, 1.0f, 1.0f};
        t.textMuted = ForgeColor{0.55f, 0.50f, 0.70f, 1.0f};
        t.border = ForgeColor{0.00f, 0.95f, 0.85f, 0.4f};
        return t;
    }
};

}
