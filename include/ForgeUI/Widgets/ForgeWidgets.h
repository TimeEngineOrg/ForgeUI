#pragma once

#include "../Core/ForgeTypes.h"
#include "../Core/ForgeID.h"
#include "../Core/ForgeContext.h"
#include "../Core/ForgeTheme.h"
#include <string_view>

namespace forge {

struct ForgeButtonStyle {
    ForgeColor normalBg = ForgeColor(0.2f, 0.22f, 0.25f, 1.0f);
    ForgeColor hoveredBg = ForgeColor(0.28f, 0.32f, 0.38f, 1.0f);
    ForgeColor activeBg = ForgeColor(0.15f, 0.18f, 0.22f, 1.0f);
    ForgeColor textColor = ForgeColor(1.0f, 1.0f, 1.0f, 1.0f);
    ForgeColor borderColor = ForgeColor(0.35f, 0.38f, 0.45f, 1.0f);
    float cornerRadius = 4.0f;
    float borderWidth = 1.0f;
    float paddingX = 12.0f;
    float paddingY = 6.0f;
    float fontSize = 14.0f;
};

struct ForgeSliderStyle {
    ForgeColor trackBg = ForgeColor(0.15f, 0.16f, 0.18f, 1.0f);
    ForgeColor fillBg = ForgeColor(0.25f, 0.50f, 0.85f, 1.0f);
    ForgeColor thumbBg = ForgeColor(0.9f, 0.9f, 0.9f, 1.0f);
    ForgeColor thumbHoverBg = ForgeColor(1.0f, 1.0f, 1.0f, 1.0f);
    ForgeColor textColor = ForgeColor(0.9f, 0.9f, 0.9f, 1.0f);
    float height = 20.0f;
    float thumbWidth = 10.0f;
    float cornerRadius = 3.0f;
    float fontSize = 12.0f;
};

struct ForgeCheckboxStyle {
    ForgeColor boxBg = ForgeColor(0.15f, 0.16f, 0.18f, 1.0f);
    ForgeColor checkColor = ForgeColor(0.25f, 0.65f, 0.95f, 1.0f);
    ForgeColor borderColor = ForgeColor(0.35f, 0.38f, 0.45f, 1.0f);
    ForgeColor textColor = ForgeColor(0.9f, 0.9f, 0.9f, 1.0f);
    float boxSize = 16.0f;
    float cornerRadius = 3.0f;
    float borderWidth = 1.0f;
    float fontSize = 14.0f;
};

struct ForgePanelStyle {
    ForgeColor bgColor = ForgeColor(0.12f, 0.13f, 0.15f, 0.95f);
    ForgeColor headerColor = ForgeColor(0.16f, 0.18f, 0.22f, 1.0f);
    ForgeColor borderColor = ForgeColor(0.22f, 0.24f, 0.28f, 1.0f);
    ForgeColor shadowColor = ForgeColor(0.0f, 0.0f, 0.0f, 0.4f);
    float cornerRadius = 6.0f;
    float borderWidth = 1.0f;
    float shadowBlur = 8.0f;
};

bool BeginPanel(std::string_view title, const ForgeDimension& width, const ForgeDimension& height, const ForgePanelStyle& style = {}, ForgeContext* ctx = nullptr);
void EndPanel(ForgeContext* ctx = nullptr);

bool BeginWindow(std::string_view title, float* posX, float* posY, float* width, float* height, bool* open = nullptr, const ForgePanelStyle& style = {}, ForgeContext* ctx = nullptr);
void EndWindow(ForgeContext* ctx = nullptr);

bool Button(std::string_view label, const ForgeDimension& width = ForgeDimension::Fit(), const ForgeDimension& height = ForgeDimension::Fit(), const ForgeButtonStyle& style = {}, ForgeContext* ctx = nullptr);

void Label(std::string_view text, float fontSize = 14.0f, const ForgeColor& color = ForgeColor(1.0f, 1.0f, 1.0f, 1.0f), ForgeContext* ctx = nullptr);

bool SliderFloat(std::string_view label, float* value, float min, float max, const ForgeDimension& width = ForgeDimension::Px(200.0f), const ForgeSliderStyle& style = {}, ForgeContext* ctx = nullptr);
bool SliderInt(std::string_view label, int32_t* value, int32_t min, int32_t max, const ForgeDimension& width = ForgeDimension::Px(200.0f), const ForgeSliderStyle& style = {}, ForgeContext* ctx = nullptr);

bool Checkbox(std::string_view label, bool* value, const ForgeCheckboxStyle& style = {}, ForgeContext* ctx = nullptr);

bool TextInput(std::string_view label, char* buffer, size_t bufferCapacity, const ForgeDimension& width = ForgeDimension::Px(200.0f), ForgeContext* ctx = nullptr);

bool Dropdown(std::string_view label, int32_t* selectedIndex, const char* const* items, size_t itemCount, bool* isOpen, const ForgeDimension& width = ForgeDimension::Px(200.0f), ForgeContext* ctx = nullptr);

bool TabBar(std::string_view id, const char* const* tabNames, size_t tabCount, int32_t* selectedTab, ForgeContext* ctx = nullptr);

bool ColorPicker(std::string_view label, ForgeColor* color, const ForgeDimension& width = ForgeDimension::Px(200.0f), ForgeContext* ctx = nullptr);

void ProgressBar(float fraction, const ForgeDimension& width = ForgeDimension::Px(200.0f), const ForgeDimension& height = ForgeDimension::Px(16.0f), const ForgeColor& barColor = ForgeColor(0.20f, 0.65f, 0.95f, 1.0f), ForgeContext* ctx = nullptr);

bool CollapsingHeader(std::string_view label, bool* isExpanded, ForgeContext* ctx = nullptr);

bool ModalDialog(std::string_view title, bool* isOpen, const ForgeDimension& width = ForgeDimension::Px(400.0f), const ForgeDimension& height = ForgeDimension::Px(250.0f), ForgeContext* ctx = nullptr);
void EndModalDialog(ForgeContext* ctx = nullptr);

bool BeginScrollView(std::string_view label, const ForgeDimension& width, const ForgeDimension& height, float* scrollY, ForgeContext* ctx = nullptr);
void EndScrollView(ForgeContext* ctx = nullptr);

}
