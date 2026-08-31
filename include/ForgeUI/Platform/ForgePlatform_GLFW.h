#pragma once

#include "../Core/ForgeInput.h"

namespace forge {

class ForgePlatform_GLFW {
public:
    static void OnMouseMove(ForgeInputState* input, float x, float y) noexcept {
        if (!input) return;
        input->mouseDelta.x = x - input->mousePos.x;
        input->mouseDelta.y = y - input->mousePos.y;
        input->mousePos.x = x;
        input->mousePos.y = y;
    }

    static void OnMouseButton(ForgeInputState* input, int button, int action) noexcept {
        if (!input || button < 0 || button > 2) return;
        if (action == 1) {
            input->mouseDown[button] = true;
            input->mousePressed[button] = true;
        } else if (action == 0) {
            input->mouseDown[button] = false;
            input->mouseReleased[button] = true;
        }
    }

    static void OnScroll(ForgeInputState* input, float xoffset, float yoffset) noexcept {
        if (!input) return;
        input->scrollDelta.x += xoffset;
        input->scrollDelta.y += yoffset;
    }

    static void OnChar(ForgeInputState* input, unsigned int codepoint) noexcept {
        if (!input) return;
        if (codepoint >= 32 && codepoint < 127 && input->textInputLength < 31) {
            input->textInputBuffer[input->textInputLength++] = static_cast<char>(codepoint);
            input->textInputBuffer[input->textInputLength] = '\0';
        }
    }

    static void OnKey(ForgeInputState* input, int key, int action) noexcept {
        if (!input) return;
        if (action == 1 || action == 2) {
            if (key == 259) input->keyBackspace = true;
            if (key == 261) input->keyDelete = true;
            if (key == 257) input->keyEnter = true;
            if (key == 256) input->keyEscape = true;
            if (key == 258) input->keyTab = true;
        }
    }
};

}
