#pragma once

#include "ForgeTypes.h"
#include "ForgeID.h"

namespace forge {

struct ForgeInputState {
    ForgeVec2 mousePos = {0.0f, 0.0f};
    ForgeVec2 mouseDelta = {0.0f, 0.0f};
    ForgeVec2 scrollDelta = {0.0f, 0.0f};

    bool mouseDown[3] = {false, false, false};
    bool mousePressed[3] = {false, false, false};
    bool mouseReleased[3] = {false, false, false};

    ForgeID activeId = FORGE_INVALID_ID;
    ForgeID hotId = FORGE_INVALID_ID;
    ForgeID focusedId = FORGE_INVALID_ID;

    char textInputBuffer[32] = {0};
    uint8_t textInputLength = 0;

    bool keyBackspace = false;
    bool keyDelete = false;
    bool keyEnter = false;
    bool keyEscape = false;
    bool keyTab = false;

    void ResetFrameDeltas() noexcept {
        mouseDelta = {0.0f, 0.0f};
        scrollDelta = {0.0f, 0.0f};
        mousePressed[0] = mousePressed[1] = mousePressed[2] = false;
        mouseReleased[0] = mouseReleased[1] = mouseReleased[2] = false;
        textInputLength = 0;
        textInputBuffer[0] = '\0';
        keyBackspace = false;
        keyDelete = false;
        keyEnter = false;
        keyEscape = false;
        keyTab = false;
    }
};

}
