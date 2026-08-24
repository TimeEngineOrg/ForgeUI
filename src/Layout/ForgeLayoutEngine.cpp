#include "ForgeUI/Layout/ForgeLayoutEngine.h"
#include <algorithm>

namespace forge {

void ForgeLayoutEngine::ComputeLayout(ForgeStorage& storage, float rootWidth, float rootHeight) noexcept {
    if (storage.Count() == 0) return;

    MeasurePass(storage, rootWidth, rootHeight);
    ArrangePass(storage, rootWidth, rootHeight);
}

void ForgeLayoutEngine::MeasurePass(ForgeStorage& storage, float rootWidth, float rootHeight) noexcept {
    size_t count = storage.Count();
    if (count == 0) return;

    for (size_t idx = count; idx > 0; --idx) {
        uint32_t i = static_cast<uint32_t>(idx - 1);
        const auto& config = storage.GetConfig(i);
        const auto& layout = config.layout;

        float padH = layout.padding.x + layout.padding.z;
        float padV = layout.padding.y + layout.padding.w;

        float w = 0.0f;
        float h = 0.0f;

        if (layout.width.mode == ForgeSizingMode::Fixed) {
            w = layout.width.value;
        } else if (layout.width.mode == ForgeSizingMode::Percent) {
            uint32_t p = storage.GetParentIndex(i);
            float parentW = (p == i) ? rootWidth : storage.GetWidth(p);
            w = (parentW * layout.width.value) / 100.0f;
        }

        if (layout.height.mode == ForgeSizingMode::Fixed) {
            h = layout.height.value;
        } else if (layout.height.mode == ForgeSizingMode::Percent) {
            uint32_t p = storage.GetParentIndex(i);
            float parentH = (p == i) ? rootHeight : storage.GetHeight(p);
            h = (parentH * layout.height.value) / 100.0f;
        }

        uint16_t childCount = storage.GetChildCount(i);
        if (childCount > 0) {
            float childSumMain = 0.0f;
            float childMaxCross = 0.0f;

            uint32_t c = i + 1;
            uint32_t endChild = i + storage.GetSubtreeSize(i);
            uint32_t directChildren = 0;

            while (c < endChild && c < count) {
                if (storage.GetParentIndex(c) == i) {
                    float cw = storage.GetWidth(c);
                    float ch = storage.GetHeight(c);
                    const auto& childLayout = storage.GetConfig(c).layout;
                    float cmX = childLayout.margin.x + childLayout.margin.z;
                    float cmY = childLayout.margin.y + childLayout.margin.w;

                    if (layout.direction == ForgeFlexDirection::Row) {
                        childSumMain += cw + cmX;
                        childMaxCross = std::max(childMaxCross, ch + cmY);
                    } else {
                        childSumMain += ch + cmY;
                        childMaxCross = std::max(childMaxCross, cw + cmX);
                    }
                    ++directChildren;
                }
                c += storage.GetSubtreeSize(c);
            }

            if (directChildren > 1) {
                childSumMain += (directChildren - 1) * layout.gap;
            }

            if (layout.width.mode == ForgeSizingMode::FitContent) {
                if (layout.direction == ForgeFlexDirection::Row) {
                    w = childSumMain + padH;
                } else {
                    w = childMaxCross + padH;
                }
            }

            if (layout.height.mode == ForgeSizingMode::FitContent) {
                if (layout.direction == ForgeFlexDirection::Row) {
                    h = childMaxCross + padV;
                } else {
                    h = childSumMain + padV;
                }
            }
        } else {
            if (layout.width.mode == ForgeSizingMode::FitContent && w == 0.0f) {
                w = padH;
            }
            if (layout.height.mode == ForgeSizingMode::FitContent && h == 0.0f) {
                h = padV;
            }
        }

        if (i == 0) {
            if (layout.width.mode == ForgeSizingMode::FitContent && w == 0.0f) w = rootWidth;
            if (layout.height.mode == ForgeSizingMode::FitContent && h == 0.0f) h = rootHeight;
            if (layout.width.mode == ForgeSizingMode::Percent) w = rootWidth;
            if (layout.height.mode == ForgeSizingMode::Percent) h = rootHeight;
        }

        storage.SetWidth(i, w);
        storage.SetHeight(i, h);
    }
}

void ForgeLayoutEngine::ArrangePass(ForgeStorage& storage, float rootWidth, float rootHeight) noexcept {
    size_t count = storage.Count();
    if (count == 0) return;

    storage.SetX(0, 0.0f);
    storage.SetY(0, 0.0f);
    storage.SetClipRect(0, ForgeRect{0.0f, 0.0f, rootWidth, rootHeight});

    for (uint32_t i = 0; i < count; ++i) {
        if ((storage.GetCullFlags(i) & ForgeCullFlags::Offscreen) != ForgeCullFlags::None) {
            uint16_t subSize = storage.GetSubtreeSize(i);
            if (subSize > 1) {
                i += (subSize - 1);
            }
            continue;
        }

        float parentX = storage.GetX(i);
        float parentY = storage.GetY(i);
        float parentW = storage.GetWidth(i);
        float parentH = storage.GetHeight(i);

        const auto& parentConfig = storage.GetConfig(i);
        const auto& layout = parentConfig.layout;

        ForgeRect parentClip = storage.GetClipRect(i);
        if (layout.clipContent) {
            ForgeRect selfRect{parentX, parentY, parentW, parentH};
            parentClip = parentClip.Intersect(selfRect);
        }

        float innerW = std::max(0.0f, parentW - (layout.padding.x + layout.padding.z));
        float innerH = std::max(0.0f, parentH - (layout.padding.y + layout.padding.w));

        uint16_t childCount = storage.GetChildCount(i);
        if (childCount == 0) continue;

        float totalFixedMain = 0.0f;
        float totalGrowWeight = 0.0f;
        uint32_t directChildren = 0;

        uint32_t c = i + 1;
        uint32_t endChild = i + storage.GetSubtreeSize(i);

        while (c < endChild && c < count) {
            if (storage.GetParentIndex(c) == i) {
                const auto& childLayout = storage.GetConfig(c).layout;
                float cmMain = (layout.direction == ForgeFlexDirection::Row)
                    ? (childLayout.margin.x + childLayout.margin.z)
                    : (childLayout.margin.y + childLayout.margin.w);

                const auto& mainDim = (layout.direction == ForgeFlexDirection::Row) ? childLayout.width : childLayout.height;
                if (mainDim.mode == ForgeSizingMode::Grow) {
                    totalGrowWeight += (mainDim.value > 0.0f ? mainDim.value : 1.0f);
                    totalFixedMain += cmMain;
                } else {
                    float measuredMain = (layout.direction == ForgeFlexDirection::Row) ? storage.GetWidth(c) : storage.GetHeight(c);
                    totalFixedMain += measuredMain + cmMain;
                }
                ++directChildren;
            }
            c += storage.GetSubtreeSize(c);
        }

        float gapTotal = (directChildren > 1) ? (directChildren - 1) * layout.gap : 0.0f;
        float availableMain = (layout.direction == ForgeFlexDirection::Row) ? innerW : innerH;
        float remainingSpace = std::max(0.0f, availableMain - totalFixedMain - gapTotal);

        if (totalGrowWeight > 0.0f && remainingSpace > 0.0f) {
            c = i + 1;
            while (c < endChild && c < count) {
                if (storage.GetParentIndex(c) == i) {
                    const auto& childLayout = storage.GetConfig(c).layout;
                    const auto& mainDim = (layout.direction == ForgeFlexDirection::Row) ? childLayout.width : childLayout.height;
                    if (mainDim.mode == ForgeSizingMode::Grow) {
                        float weight = (mainDim.value > 0.0f ? mainDim.value : 1.0f);
                        float allocated = (remainingSpace * weight) / totalGrowWeight;
                        if (layout.direction == ForgeFlexDirection::Row) {
                            storage.SetWidth(c, allocated);
                        } else {
                            storage.SetHeight(c, allocated);
                        }
                    }
                }
                c += storage.GetSubtreeSize(c);
            }
            remainingSpace = 0.0f;
        }

        float mainOffset = 0.0f;
        float extraSpacing = 0.0f;

        if (layout.justify == ForgeJustify::Center) {
            mainOffset = remainingSpace * 0.5f;
        } else if (layout.justify == ForgeJustify::End) {
            mainOffset = remainingSpace;
        } else if (layout.justify == ForgeJustify::SpaceBetween) {
            if (directChildren > 1) {
                extraSpacing = remainingSpace / static_cast<float>(directChildren - 1);
            }
        } else if (layout.justify == ForgeJustify::SpaceAround) {
            if (directChildren > 0) {
                extraSpacing = remainingSpace / static_cast<float>(directChildren);
                mainOffset = extraSpacing * 0.5f;
            }
        }

        float curMain = mainOffset;
        c = i + 1;

        while (c < endChild && c < count) {
            if (storage.GetParentIndex(c) == i) {
                const auto& childLayout = storage.GetConfig(c).layout;
                float childW = storage.GetWidth(c);
                float childH = storage.GetHeight(c);

                if (layout.direction == ForgeFlexDirection::Row) {
                    if (layout.alignment == ForgeAlignment::Stretch && childLayout.height.mode != ForgeSizingMode::Fixed) {
                        childH = std::max(0.0f, innerH - (childLayout.margin.y + childLayout.margin.w));
                        storage.SetHeight(c, childH);
                    }

                    float crossOffset = 0.0f;
                    float availableCross = std::max(0.0f, innerH - (childLayout.margin.y + childLayout.margin.w));
                    if (layout.alignment == ForgeAlignment::Center) {
                        crossOffset = (availableCross - childH) * 0.5f;
                    } else if (layout.alignment == ForgeAlignment::End) {
                        crossOffset = availableCross - childH;
                    }

                    float posX = parentX + layout.padding.x + curMain + childLayout.margin.x;
                    float posY = parentY + layout.padding.y + crossOffset + childLayout.margin.y;

                    storage.SetX(c, posX);
                    storage.SetY(c, posY);
                    storage.SetClipRect(c, parentClip);

                    ForgeRect childBounds{posX, posY, childW, childH};
                    if (childW > 0.0f && childH > 0.0f && !childBounds.Overlaps(parentClip)) {
                        storage.SetCullFlags(c, storage.GetCullFlags(c) | ForgeCullFlags::Offscreen);
                    }

                    curMain += childW + childLayout.margin.x + childLayout.margin.z + layout.gap + extraSpacing;
                } else {
                    if (layout.alignment == ForgeAlignment::Stretch && childLayout.width.mode != ForgeSizingMode::Fixed) {
                        childW = std::max(0.0f, innerW - (childLayout.margin.x + childLayout.margin.z));
                        storage.SetWidth(c, childW);
                    }

                    float crossOffset = 0.0f;
                    float availableCross = std::max(0.0f, innerW - (childLayout.margin.x + childLayout.margin.z));
                    if (layout.alignment == ForgeAlignment::Center) {
                        crossOffset = (availableCross - childW) * 0.5f;
                    } else if (layout.alignment == ForgeAlignment::End) {
                        crossOffset = availableCross - childW;
                    }

                    float posX = parentX + layout.padding.x + crossOffset + childLayout.margin.x;
                    float posY = parentY + layout.padding.y + curMain + childLayout.margin.y;

                    storage.SetX(c, posX);
                    storage.SetY(c, posY);
                    storage.SetClipRect(c, parentClip);

                    ForgeRect childBounds{posX, posY, childW, childH};
                    if (childW > 0.0f && childH > 0.0f && !childBounds.Overlaps(parentClip)) {
                        storage.SetCullFlags(c, storage.GetCullFlags(c) | ForgeCullFlags::Offscreen);
                    }

                    curMain += childH + childLayout.margin.y + childLayout.margin.w + layout.gap + extraSpacing;
                }
            }
            c += storage.GetSubtreeSize(c);
        }
    }
}

}
