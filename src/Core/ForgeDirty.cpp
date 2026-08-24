#include "ForgeUI/Core/ForgeDirty.h"

namespace forge {

void ForgeDirtySystem::BubbleUpDirty(ForgeStorage& storage, uint32_t elementIndex) noexcept {
    if (elementIndex >= storage.Count()) return;

    uint32_t current = elementIndex;
    while (true) {
        const auto& config = storage.GetConfig(current);
        bool fixedW = (config.layout.width.mode == ForgeSizingMode::Fixed);
        bool fixedH = (config.layout.height.mode == ForgeSizingMode::Fixed);

        uint32_t parent = storage.GetParentIndex(current);
        if (parent == current) break;

        if (fixedW && fixedH) {
            break;
        }

        current = parent;
    }
}

void ForgeDirtySystem::CascadeDirty(ForgeStorage& storage, uint32_t elementIndex) noexcept {
    if (elementIndex >= storage.Count()) return;

    uint16_t subSize = storage.GetSubtreeSize(elementIndex);
    uint32_t endIndex = elementIndex + subSize;
    if (endIndex > storage.Count()) {
        endIndex = static_cast<uint32_t>(storage.Count());
    }
}

}
