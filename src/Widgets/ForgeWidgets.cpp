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

    ForgeVec2 textPos = bounds.Position() + ForgeVec2(style.paddingX, (elementHeight - textSize.y) * 0.5f);
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

    ForgeVec2 pos(0.0f, 0.0f);
    font.AppendTextToDrawList(&drawList, text.data(), pos, fontSize, color.ToRGBA8());
}

bool SliderFloat(std::string_view label, float* value, float min, float max, const ForgeDimension& width, const ForgeSliderStyle& style, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !value) return false;

    ForgeID id = c->GetID(label);
    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    float elementWidth = (width.mode == ForgeSizingMode::Fixed) ? width.value : 200.0f;
    float elementHeight = style.height;

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(elementWidth);
    config.layout.height = ForgeDimension::Px(elementHeight);

    c->Element(label, config);

    ForgeVec2 pos(0.0f, 0.0f);
    ForgeRect bounds(pos.x, pos.y, elementWidth, elementHeight);

    bool hovered = IsHovered(bounds, input.mousePos);
    bool changed = false;

    if (hovered) {
        input.hotId = id;
        if (input.mousePressed[0]) {
            input.activeId = id;
        }
    }

    if (input.activeId == id) {
        if (input.mouseDown[0]) {
            float normalized = (input.mousePos.x - pos.x) / elementWidth;
            normalized = std::max(0.0f, std::min(1.0f, normalized));
            float newValue = min + normalized * (max - min);
            if (newValue != *value) {
                *value = newValue;
                changed = true;
            }
        } else {
            input.activeId = FORGE_INVALID_ID;
        }
    }

    drawList.AddRoundedRect(bounds.Position(), bounds.Position() + bounds.Size(), style.cornerRadius, style.trackBg.ToRGBA8());

    float fraction = (max > min) ? ((*value - min) / (max - min)) : 0.0f;
    fraction = std::max(0.0f, std::min(1.0f, fraction));

    ForgeVec2 fillMax(bounds.x + fraction * elementWidth, bounds.y + bounds.h);
    if (fillMax.x > bounds.x) {
        drawList.AddRoundedRect(bounds.Position(), fillMax, style.cornerRadius, style.fillBg.ToRGBA8());
    }

    float thumbX = bounds.x + fraction * (elementWidth - style.thumbWidth);
    ForgeRect thumbBounds(thumbX, bounds.y, style.thumbWidth, elementHeight);
    ForgeColor thumbColor = (hovered || input.activeId == id) ? style.thumbHoverBg : style.thumbBg;
    drawList.AddRoundedRect(thumbBounds.Position(), thumbBounds.Position() + thumbBounds.Size(), style.cornerRadius, thumbColor.ToRGBA8());

    char valueStr[32];
    std::snprintf(valueStr, sizeof(valueStr), "%.2f", *value);
    ForgeVec2 valSize = font.MeasureText(valueStr, style.fontSize);
    ForgeVec2 textPos(bounds.x + (elementWidth - valSize.x) * 0.5f, bounds.y + (elementHeight - valSize.y) * 0.5f);
    font.AppendTextToDrawList(&drawList, valueStr, textPos, style.fontSize, style.textColor.ToRGBA8());

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
    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    ForgeVec2 labelSize = font.MeasureText(label.data(), style.fontSize);
    float totalWidth = style.boxSize + 8.0f + labelSize.x;
    float totalHeight = std::max(style.boxSize, labelSize.y);

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(totalWidth);
    config.layout.height = ForgeDimension::Px(totalHeight);

    c->Element(label, config);

    ForgeVec2 pos(0.0f, 0.0f);
    ForgeRect bounds(pos.x, pos.y, totalWidth, totalHeight);
    ForgeRect boxBounds(pos.x, pos.y + (totalHeight - style.boxSize) * 0.5f, style.boxSize, style.boxSize);

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
                *value = !(*value);
                clicked = true;
            }
            input.activeId = FORGE_INVALID_ID;
        }
    }

    drawList.AddRoundedRect(boxBounds.Position(), boxBounds.Position() + boxBounds.Size(), style.cornerRadius, style.boxBg.ToRGBA8());
    if (style.borderWidth > 0.0f) {
        drawList.AddBorder(boxBounds.Position(), boxBounds.Position() + boxBounds.Size(), style.cornerRadius, style.borderWidth, style.borderColor.ToRGBA8());
    }

    if (*value) {
        ForgeRect checkInner(boxBounds.x + 3.0f, boxBounds.y + 3.0f, boxBounds.w - 6.0f, boxBounds.h - 6.0f);
        drawList.AddRoundedRect(checkInner.Position(), checkInner.Position() + checkInner.Size(), 2.0f, style.checkColor.ToRGBA8());
    }

    ForgeVec2 textPos(boxBounds.x + boxBounds.w + 8.0f, pos.y + (totalHeight - labelSize.y) * 0.5f);
    font.AppendTextToDrawList(&drawList, label.data(), textPos, style.fontSize, style.textColor.ToRGBA8());

    return clicked;
}

bool TextInput(std::string_view label, char* buffer, size_t bufferCapacity, const ForgeDimension& width, ForgeContext* ctx) {
    ForgeContext* c = ctx ? ctx : GetCurrentContext();
    if (!c || !buffer || bufferCapacity == 0) return false;

    ForgeID id = c->GetID(label);
    ForgeInputState& input = c->GetInput();
    ForgeDrawList& drawList = c->GetDrawList();
    ForgeFont_MSDF& font = c->GetFont();

    float elementWidth = (width.mode == ForgeSizingMode::Fixed) ? width.value : 200.0f;
    float elementHeight = 24.0f;

    ForgeElementConfig config{};
    config.layout.width = ForgeDimension::Px(elementWidth);
    config.layout.height = ForgeDimension::Px(elementHeight);

    c->Element(label, config);

    ForgeVec2 pos(0.0f, 0.0f);
    ForgeRect bounds(pos.x, pos.y, elementWidth, elementHeight);

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
