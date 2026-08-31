#include "ForgeUI/Widgets/ForgeWidgets.h"
#include <cstdio>
#include <cstring>
#include <algorithm>

namespace forge {

static bool IsHovered(const ForgeRect& rect, const ForgeVec2& mousePos) {
    return rect.Contains(mousePos);
}

bool BeginPanel(std::string_view title, const ForgeDimension& width, const ForgeDimension& height, const ForgePanelStyle& style, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c) return false;

    ForgeElementConfig config{};
    config.layout.width = width;
    config.layout.height = height;
    config.layout.padding = ForgeVec4(12.0f, 12.0f, 12.0f, 12.0f);
    config.layout.gap = 8.0f;
    config.layout.direction = ForgeFlexDirection::Column;

    config.style.backgroundColor = style.bgColor;
    config.style.borderColor = style.borderColor;
    config.style.shadowColor = style.shadowColor;
    config.style.cornerRadius = ForgeVec4(style.cornerRadius, style.cornerRadius, style.cornerRadius, style.cornerRadius);
    config.style.borderWidth = style.borderWidth;
    config.style.shadowBlur = style.shadowBlur;

    c->Begin(title, config);
    return true;
}

void EndPanel(ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (c) {
        c->End();
    }
}

bool BeginWindow(std::string_view title, float* posX, float* posY, float* width, float* height, bool* open, const ForgePanelStyle& style, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !posX || !posY || !width || !height) return false;
    if (open && !(*open)) return false;

    ForgeID id = c->GetID(title);
    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    ForgeRect headerRect(*posX, *posY, *width, 28.0f);
    if (headerRect.Contains(input.mousePos) && input.mousePressed[0]) {
        input.activeId = id;
    }

    if (input.activeId == id) {
        if (input.mouseDown[0]) {
            *posX += input.mouseDelta.x;
            *posY += input.mouseDelta.y;
        } else {
            input.activeId = FORGE_INVALID_ID;
        }
    }

    ForgeRect winRect(*posX, *posY, *width, *height);
    drawList.AddRoundedRect(winRect.Position(), winRect.Position() + winRect.Size(), style.cornerRadius, style.bgColor.ToRGBA8());
    drawList.AddRoundedRect(headerRect.Position(), headerRect.Position() + headerRect.Size(), style.cornerRadius, style.headerColor.ToRGBA8());
    drawList.AddBorder(winRect.Position(), winRect.Position() + winRect.Size(), style.cornerRadius, style.borderWidth, style.borderColor.ToRGBA8());

    font.AppendTextToDrawList(&drawList, title.data(), ForgeVec2(*posX + 8.0f, *posY + 6.0f), 14.0f, 0xFFFFFFFF);

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(*width);
    config.layout.height = ForgeDimension::Px(*height - 28.0f);
    config.layout.padding = ForgeVec4(8.0f, 8.0f, 8.0f, 8.0f);
    config.layout.gap = 6.0f;
    config.layout.direction = ForgeFlexDirection::Column;

    c->Begin(title, config);
    return true;
}

void EndWindow(ForgeContext* ctx) {
    EndPanel(ctx);
}

bool Button(std::string_view label, const ForgeDimension& width, const ForgeDimension& height, const ForgeButtonStyle& style, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c) return false;

    ForgeID id = c->GetID(label);
    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    ForgeVec2 textSize = font.MeasureText(label.data(), style.fontSize);
    float elementWidth = (width.mode == ForgeSizingMode::Fixed) ? width.value : (textSize.x + style.paddingX * 2.0f);
    float elementHeight = (height.mode == ForgeSizingMode::Fixed) ? height.value : (textSize.y + style.paddingY * 2.0f);

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(elementWidth);
    config.layout.height = ForgeDimension::Px(elementHeight);

    uint32_t idx = c->Element(label, config);
    (void)idx;

    ForgeVec2 pos(0.0f, 0.0f);
    ForgeRect bounds(pos.x, pos.y, elementWidth, elementHeight);

    bool hovered = IsHovered(bounds, input.mousePos);
    bool clicked = false;

    if (hovered) {
        input.hotId = id;
        if (input.mousePressed[0]) {
            input.activeId = id;
        }
    }

    if (input.activeId == id) {
        if (input.mouseReleased[0]) {
            if (hovered) {
                clicked = true;
            }
            input.activeId = FORGE_INVALID_ID;
        }
    }

    ForgeColor bg = style.normalBg;
    if (input.activeId == id) {
        bg = style.activeBg;
    } else if (hovered) {
        bg = style.hoveredBg;
    }

    drawList.AddRoundedRect(bounds.Position(), bounds.Position() + bounds.Size(), style.cornerRadius, bg.ToRGBA8());
    if (style.borderWidth > 0.0f) {
        drawList.AddBorder(bounds.Position(), bounds.Position() + bounds.Size(), style.cornerRadius, style.borderWidth, style.borderColor.ToRGBA8());
    }

    ForgeVec2 textPos = bounds.Position() + ForgeVec2(style.paddingX, style.paddingY);
    font.AppendTextToDrawList(&drawList, label.data(), textPos, style.fontSize, style.textColor.ToRGBA8());

    return clicked;
}

void Label(std::string_view text, float fontSize, const ForgeColor& color, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c) return;

    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    ForgeVec2 textSize = font.MeasureText(text.data(), fontSize);

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(textSize.x);
    config.layout.height = ForgeDimension::Px(textSize.y);

    c->Element(text, config);

    ForgeVec2 textPos(0.0f, 0.0f);
    font.AppendTextToDrawList(&drawList, text.data(), textPos, fontSize, color.ToRGBA8());
}

bool SliderFloat(std::string_view label, float* value, float min, float max, const ForgeDimension& width, const ForgeSliderStyle& style, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !value) return false;

    ForgeID id = c->GetID(label);
    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    float w = (width.mode == ForgeSizingMode::Fixed) ? width.value : 200.0f;
    float h = style.height;

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(w);
    config.layout.height = ForgeDimension::Px(h);

    c->Element(label, config);

    ForgeRect bounds(0.0f, 0.0f, w, h);
    bool hovered = IsHovered(bounds, input.mousePos);
    bool changed = false;

    if (hovered && input.mousePressed[0]) {
        input.activeId = id;
    }

    if (input.activeId == id) {
        if (input.mouseDown[0]) {
            float norm = (input.mousePos.x - bounds.x) / bounds.w;
            norm = std::clamp(norm, 0.0f, 1.0f);
            float newVal = min + norm * (max - min);
            if (newVal != *value) {
                *value = newVal;
                changed = true;
            }
        } else {
            input.activeId = FORGE_INVALID_ID;
        }
    }

    float normVal = (*value - min) / (max - min);
    normVal = std::clamp(normVal, 0.0f, 1.0f);

    drawList.AddRoundedRect(bounds.Position(), bounds.Position() + bounds.Size(), style.cornerRadius, style.trackBg.ToRGBA8());

    ForgeVec2 fillMax(bounds.x + normVal * bounds.w, bounds.y + bounds.h);
    if (fillMax.x > bounds.x) {
        drawList.AddRoundedRect(bounds.Position(), fillMax, style.cornerRadius, style.fillBg.ToRGBA8());
    }

    float thumbX = bounds.x + normVal * (bounds.w - style.thumbWidth);
    ForgeVec2 thumbMin(thumbX, bounds.y);
    ForgeVec2 thumbMax(thumbX + style.thumbWidth, bounds.y + bounds.h);
    ForgeColor thumbCol = (input.activeId == id || hovered) ? style.thumbHoverBg : style.thumbBg;
    drawList.AddRoundedRect(thumbMin, thumbMax, style.cornerRadius, thumbCol.ToRGBA8());

    char buf[64];
    snprintf(buf, sizeof(buf), "%.2f", *value);
    ForgeVec2 textPos(bounds.x + bounds.w + 8.0f, bounds.y + 2.0f);
    font.AppendTextToDrawList(&drawList, buf, textPos, style.fontSize, style.textColor.ToRGBA8());

    return changed;
}

bool SliderInt(std::string_view label, int32_t* value, int32_t min, int32_t max, const ForgeDimension& width, const ForgeSliderStyle& style, ForgeContext* ctx) {
    if (!value) return false;
    float fVal = static_cast<float>(*value);
    bool changed = SliderFloat(label, &fVal, static_cast<float>(min), static_cast<float>(max), width, style, ctx);
    if (changed) {
        *value = static_cast<int32_t>(fVal + 0.5f);
    }
    return changed;
}

bool Checkbox(std::string_view label, bool* value, const ForgeCheckboxStyle& style, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !value) return false;

    ForgeID id = c->GetID(label);
    (void)id;
    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    ForgeVec2 textSize = font.MeasureText(label.data(), style.fontSize);
    float totalW = style.boxSize + 8.0f + textSize.x;
    float totalH = std::max(style.boxSize, textSize.y);

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(totalW);
    config.layout.height = ForgeDimension::Px(totalH);

    c->Element(label, config);

    ForgeRect bounds(0.0f, 0.0f, totalW, totalH);
    bool hovered = IsHovered(bounds, input.mousePos);
    bool changed = false;

    if (hovered && input.mousePressed[0]) {
        *value = !(*value);
        changed = true;
    }

    ForgeVec2 boxMin(bounds.x, bounds.y + (totalH - style.boxSize) * 0.5f);
    ForgeVec2 boxMax(boxMin.x + style.boxSize, boxMin.y + style.boxSize);

    drawList.AddRoundedRect(boxMin, boxMax, style.cornerRadius, style.boxBg.ToRGBA8());
    drawList.AddBorder(boxMin, boxMax, style.cornerRadius, style.borderWidth, style.borderColor.ToRGBA8());

    if (*value) {
        ForgeVec2 checkMin = boxMin + ForgeVec2(3.0f, 3.0f);
        ForgeVec2 checkMax = boxMax - ForgeVec2(3.0f, 3.0f);
        drawList.AddRoundedRect(checkMin, checkMax, 2.0f, style.checkColor.ToRGBA8());
    }

    ForgeVec2 textPos(boxMax.x + 8.0f, bounds.y + (totalH - textSize.y) * 0.5f);
    font.AppendTextToDrawList(&drawList, label.data(), textPos, style.fontSize, style.textColor.ToRGBA8());

    return changed;
}

bool TextInput(std::string_view label, char* buffer, size_t bufferCapacity, const ForgeDimension& width, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !buffer || bufferCapacity == 0) return false;

    ForgeID id = c->GetID(label);
    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    float w = (width.mode == ForgeSizingMode::Fixed) ? width.value : 200.0f;
    float h = 26.0f;

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(w);
    config.layout.height = ForgeDimension::Px(h);

    c->Element(label, config);

    ForgeRect bounds(0.0f, 0.0f, w, h);
    bool hovered = IsHovered(bounds, input.mousePos);

    if (hovered && input.mousePressed[0]) {
        input.focusedId = id;
    }

    bool modified = false;
    if (input.focusedId == id) {
        size_t len = std::strlen(buffer);
        if (input.keyBackspace && len > 0) {
            buffer[len - 1] = '\0';
            modified = true;
        }

        if (input.textInputLength > 0 && len + input.textInputLength < bufferCapacity) {
            for (uint8_t i = 0; i < input.textInputLength; ++i) {
                buffer[len + i] = input.textInputBuffer[i];
            }
            buffer[len + input.textInputLength] = '\0';
            modified = true;
        }
    }

    ForgeColor bg = (input.focusedId == id) ? ForgeColor(0.18f, 0.20f, 0.24f, 1.0f) : ForgeColor(0.12f, 0.13f, 0.15f, 1.0f);
    drawList.AddRoundedRect(bounds.Position(), bounds.Position() + bounds.Size(), 3.0f, bg.ToRGBA8());
    drawList.AddBorder(bounds.Position(), bounds.Position() + bounds.Size(), 3.0f, 1.0f, ForgeColor(0.35f, 0.38f, 0.45f, 1.0f).ToRGBA8());

    ForgeVec2 textPos(bounds.x + 6.0f, bounds.y + 4.0f);
    font.AppendTextToDrawList(&drawList, buffer, textPos, 14.0f, ForgeColor(0.95f, 0.95f, 0.95f, 1.0f).ToRGBA8());

    if (input.focusedId == id) {
        ForgeVec2 textLen = font.MeasureText(buffer, 14.0f);
        ForgeVec2 curMin(textPos.x + textLen.x + 1.0f, bounds.y + 3.0f);
        ForgeVec2 curMax(curMin.x + 1.5f, bounds.y + bounds.h - 3.0f);
        drawList.AddRect(curMin, curMax, 0xFFFFFFFF);
    }

    return modified;
}

bool Dropdown(std::string_view label, int32_t* selectedIndex, const char* const* items, size_t itemCount, bool* isOpen, const ForgeDimension& width, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !selectedIndex || !items || itemCount == 0 || !isOpen) return false;

    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    float w = (width.mode == ForgeSizingMode::Fixed) ? width.value : 200.0f;
    float h = 26.0f;

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(w);
    config.layout.height = ForgeDimension::Px(h);
    c->Element(label, config);

    ForgeRect bounds(0.0f, 0.0f, w, h);
    bool hovered = IsHovered(bounds, input.mousePos);

    if (hovered && input.mousePressed[0]) {
        *isOpen = !(*isOpen);
    }

    drawList.AddRoundedRect(bounds.Position(), bounds.Position() + bounds.Size(), 4.0f, 0xFF2A2D34);
    drawList.AddBorder(bounds.Position(), bounds.Position() + bounds.Size(), 4.0f, 1.0f, 0xFF3D4450);

    const char* current = (*selectedIndex >= 0 && static_cast<size_t>(*selectedIndex) < itemCount) ? items[*selectedIndex] : "Select...";
    font.AppendTextToDrawList(&drawList, current, bounds.Position() + ForgeVec2(8.0f, 5.0f), 14.0f, 0xFFFFFFFF);

    font.AppendTextToDrawList(&drawList, *isOpen ? "^" : "v", bounds.Position() + ForgeVec2(w - 18.0f, 5.0f), 12.0f, 0xFFAAAAAA);

    bool changed = false;
    if (*isOpen) {
        for (size_t i = 0; i < itemCount; ++i) {
            ForgeRect itemRect(bounds.x, bounds.y + h + static_cast<float>(i) * 24.0f, w, 24.0f);
            bool itemHover = IsHovered(itemRect, input.mousePos);

            uint32_t itemBg = itemHover ? 0xFF3B4860 : 0xFF1E2128;
            drawList.AddRect(itemRect.Position(), itemRect.Position() + itemRect.Size(), itemBg);
            font.AppendTextToDrawList(&drawList, items[i], itemRect.Position() + ForgeVec2(8.0f, 4.0f), 13.0f, 0xFFFFFFFF);

            if (itemHover && input.mousePressed[0]) {
                *selectedIndex = static_cast<int32_t>(i);
                *isOpen = false;
                changed = true;
            }
        }
    }

    return changed;
}

bool TabBar(std::string_view id, const char* const* tabNames, size_t tabCount, int32_t* selectedTab, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !tabNames || tabCount == 0 || !selectedTab) return false;

    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Grow();
    config.layout.height = ForgeDimension::Px(32.0f);
    config.layout.direction = ForgeFlexDirection::Row;
    config.layout.gap = 4.0f;
    c->Begin(id, config);

    bool changed = false;
    float curX = 0.0f;

    for (size_t i = 0; i < tabCount; ++i) {
        ForgeVec2 txtSize = font.MeasureText(tabNames[i], 14.0f);
        float tabW = txtSize.x + 24.0f;
        ForgeRect tabRect(curX, 0.0f, tabW, 30.0f);

        bool active = (*selectedTab == static_cast<int32_t>(i));
        bool hover = IsHovered(tabRect, input.mousePos);

        uint32_t col = active ? 0xFF2F66B8 : (hover ? 0xFF353B45 : 0xFF20242C);
        drawList.AddRoundedRect(tabRect.Position(), tabRect.Position() + tabRect.Size(), 4.0f, col);

        font.AppendTextToDrawList(&drawList, tabNames[i], tabRect.Position() + ForgeVec2(12.0f, 6.0f), 14.0f, 0xFFFFFFFF);

        if (hover && input.mousePressed[0]) {
            *selectedTab = static_cast<int32_t>(i);
            changed = true;
        }

        curX += tabW + 4.0f;
    }

    c->End();
    return changed;
}

bool ColorPicker(std::string_view label, ForgeColor* color, const ForgeDimension& width, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !color) return false;

    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    float w = (width.mode == ForgeSizingMode::Fixed) ? width.value : 200.0f;

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(w);
    config.layout.height = ForgeDimension::Px(78.0f);
    config.layout.direction = ForgeFlexDirection::Column;
    config.layout.gap = 4.0f;
    c->Begin(label, config);

    ForgeRect preview(0.0f, 0.0f, 32.0f, 18.0f);
    drawList.AddRoundedRect(preview.Position(), preview.Position() + preview.Size(), 3.0f, color->ToRGBA8());
    font.AppendTextToDrawList(&drawList, label.data(), ForgeVec2(38.0f, 2.0f), 13.0f, 0xFFFFFFFF);

    bool chgR = SliderFloat("R", &color->r, 0.0f, 1.0f, ForgeDimension::Px(w - 20.0f), {}, c);
    bool chgG = SliderFloat("G", &color->g, 0.0f, 1.0f, ForgeDimension::Px(w - 20.0f), {}, c);
    bool chgB = SliderFloat("B", &color->b, 0.0f, 1.0f, ForgeDimension::Px(w - 20.0f), {}, c);

    c->End();
    return chgR || chgG || chgB;
}

void ProgressBar(float fraction, const ForgeDimension& width, const ForgeDimension& height, const ForgeColor& barColor, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c) return;

    ForgeDrawList& drawList = c->GetDrawList();

    float w = (width.mode == ForgeSizingMode::Fixed) ? width.value : 200.0f;
    float h = (height.mode == ForgeSizingMode::Fixed) ? height.value : 16.0f;

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(w);
    config.layout.height = ForgeDimension::Px(h);
    c->Element("ProgressBar", config);

    ForgeRect bounds(0.0f, 0.0f, w, h);
    drawList.AddRoundedRect(bounds.Position(), bounds.Position() + bounds.Size(), 3.0f, 0xFF1C1E24);

    float frac = std::clamp(fraction, 0.0f, 1.0f);
    if (frac > 0.0f) {
        ForgeVec2 fillMax(bounds.x + frac * bounds.w, bounds.y + bounds.h);
        drawList.AddRoundedRect(bounds.Position(), fillMax, 3.0f, barColor.ToRGBA8());
    }
}

bool CollapsingHeader(std::string_view label, bool* isExpanded, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !isExpanded) return false;

    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Grow();
    config.layout.height = ForgeDimension::Px(24.0f);
    c->Element(label, config);

    ForgeRect bounds(0.0f, 0.0f, 300.0f, 24.0f);
    bool hover = IsHovered(bounds, input.mousePos);

    if (hover && input.mousePressed[0]) {
        *isExpanded = !(*isExpanded);
    }

    uint32_t bg = hover ? 0xFF353C48 : 0xFF242830;
    drawList.AddRoundedRect(bounds.Position(), bounds.Position() + bounds.Size(), 3.0f, bg);

    const char* arrow = *isExpanded ? "v" : ">";
    font.AppendTextToDrawList(&drawList, arrow, bounds.Position() + ForgeVec2(8.0f, 4.0f), 12.0f, 0xFF888888);
    font.AppendTextToDrawList(&drawList, label.data(), bounds.Position() + ForgeVec2(24.0f, 4.0f), 13.0f, 0xFFFFFFFF);

    return *isExpanded;
}

bool ModalDialog(std::string_view title, bool* isOpen, const ForgeDimension& width, const ForgeDimension& height, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !isOpen || !(*isOpen)) return false;

    ForgeDrawList& drawList = c->GetDrawList();

    drawList.AddRect(ForgeVec2(0.0f, 0.0f), ForgeVec2(1920.0f, 1080.0f), 0x99000000);

    return BeginPanel(title, width, height, {}, c);
}

void EndModalDialog(ForgeContext* ctx) {
    EndPanel(ctx);
}

bool BeginScrollView(std::string_view label, const ForgeDimension& width, const ForgeDimension& height, float* scrollY, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !scrollY) return false;

    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();

    float w = (width.mode == ForgeSizingMode::Fixed) ? width.value : 300.0f;
    float h = (height.mode == ForgeSizingMode::Fixed) ? height.value : 400.0f;

    ForgeRect bounds(0.0f, 0.0f, w, h);
    if (IsHovered(bounds, input.mousePos)) {
        *scrollY += input.scrollDelta.y * 20.0f;
        if (*scrollY < 0.0f) *scrollY = 0.0f;
    }

    ForgeElementConfig config{};
    config.layout.width = width;
    config.layout.height = height;
    config.layout.clipContent = true;
    config.layout.direction = ForgeFlexDirection::Column;

    c->Begin(label, config);

    drawList.PushClipRect(ForgeVec4(bounds.x, bounds.y, bounds.x + bounds.w, bounds.y + bounds.h));
    return true;
}

void EndScrollView(ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c) return;

    c->GetDrawList().PopClipRect();
    c->End();
}

}
