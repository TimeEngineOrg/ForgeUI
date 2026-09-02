#include "ForgeUI/Core/ForgeContext.h"
#include "ForgeUI/Widgets/ForgeWidgets.h"
#include "ForgeUI/Backend/ForgeBackend_OpenGL.h"

#include <GLFW/glfw3.h>
#include <cstdio>
#include <chrono>
#include <vector>

static forge::ForgeContext g_Context;
static forge::ForgeBackend_OpenGL g_Backend;

static void MousePosCallback(GLFWwindow*, double xpos, double ypos) {
    auto& input = g_Context.GetInput();
    input.mouseDelta.x = static_cast<float>(xpos) - input.mousePos.x;
    input.mouseDelta.y = static_cast<float>(ypos) - input.mousePos.y;
    input.mousePos.x = static_cast<float>(xpos);
    input.mousePos.y = static_cast<float>(ypos);
}

static void MouseButtonCallback(GLFWwindow*, int button, int action, int) {
    if (button >= 0 && button < 3) {
        auto& input = g_Context.GetInput();
        if (action == GLFW_PRESS) {
            input.mouseDown[button] = true;
            input.mousePressed[button] = true;
        } else if (action == GLFW_RELEASE) {
            input.mouseDown[button] = false;
            input.mouseReleased[button] = true;
        }
    }
}

static void ScrollCallback(GLFWwindow*, double xoffset, double yoffset) {
    auto& input = g_Context.GetInput();
    input.scrollDelta.x += static_cast<float>(xoffset);
    input.scrollDelta.y += static_cast<float>(yoffset);
}

static void CharCallback(GLFWwindow*, unsigned int c) {
    auto& input = g_Context.GetInput();
    if (c >= 32 && c < 127 && input.textInputLength < sizeof(input.textInputBuffer) - 1) {
        input.textInputBuffer[input.textInputLength++] = static_cast<char>(c);
        input.textInputBuffer[input.textInputLength] = '\0';
    }
}

static void KeyCallback(GLFWwindow*, int key, int, int action, int) {
    auto& input = g_Context.GetInput();
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_BACKSPACE) input.keyBackspace = true;
        if (key == GLFW_KEY_DELETE) input.keyDelete = true;
        if (key == GLFW_KEY_ENTER) input.keyEnter = true;
        if (key == GLFW_KEY_ESCAPE) input.keyEscape = true;
        if (key == GLFW_KEY_TAB) input.keyTab = true;
    }
}

static const uint8_t s_Font5x7[96][7] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x04,0x04,0x04,0x04,0x00,0x04,0x00},
    {0x0A,0x0A,0x00,0x00,0x00,0x00,0x00},
    {0x0A,0x0A,0x1F,0x0A,0x1F,0x0A,0x0A},
    {0x04,0x1E,0x05,0x0E,0x14,0x0F,0x04},
    {0x12,0x15,0x12,0x08,0x09,0x15,0x09},
    {0x0C,0x12,0x14,0x08,0x15,0x12,0x0D},
    {0x06,0x06,0x04,0x00,0x00,0x00,0x00},
    {0x02,0x04,0x08,0x08,0x08,0x04,0x02},
    {0x08,0x04,0x02,0x02,0x02,0x04,0x08},
    {0x00,0x04,0x15,0x0E,0x15,0x04,0x00},
    {0x00,0x04,0x04,0x1F,0x04,0x04,0x00},
    {0x00,0x00,0x00,0x00,0x06,0x06,0x04},
    {0x00,0x00,0x00,0x1F,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x06,0x06},
    {0x00,0x01,0x02,0x04,0x08,0x10,0x00},
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E},
    {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E},
    {0x0E,0x11,0x01,0x06,0x08,0x10,0x1F},
    {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E},
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02},
    {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E},
    {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E},
    {0x1F,0x01,0x02,0x04,0x08,0x08,0x08},
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E},
    {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C},
    {0x00,0x06,0x06,0x00,0x06,0x06,0x00},
    {0x00,0x06,0x06,0x00,0x06,0x06,0x04},
    {0x02,0x04,0x08,0x10,0x08,0x04,0x02},
    {0x00,0x00,0x1F,0x00,0x1F,0x00,0x00},
    {0x08,0x04,0x02,0x01,0x02,0x04,0x08},
    {0x0E,0x11,0x01,0x02,0x04,0x00,0x04},
    {0x0E,0x11,0x01,0x0D,0x15,0x15,0x0E},
    {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11},
    {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E},
    {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E},
    {0x1C,0x12,0x11,0x11,0x11,0x12,0x1C},
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F},
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10},
    {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F},
    {0x11,0x11,0x11,0x1F,0x11,0x11,0x11},
    {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E},
    {0x07,0x02,0x02,0x02,0x02,0x12,0x0C},
    {0x11,0x12,0x14,0x18,0x14,0x12,0x11},
    {0x10,0x10,0x10,0x10,0x10,0x10,0x1F},
    {0x11,0x1B,0x15,0x15,0x11,0x11,0x11},
    {0x11,0x11,0x19,0x15,0x13,0x11,0x11},
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E},
    {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10},
    {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D},
    {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11},
    {0x0E,0x11,0x10,0x0E,0x01,0x11,0x0E},
    {0x1F,0x04,0x04,0x04,0x04,0x04,0x04},
    {0x11,0x11,0x11,0x11,0x11,0x11,0x0E},
    {0x11,0x11,0x11,0x11,0x11,0x0A,0x04},
    {0x11,0x11,0x11,0x15,0x15,0x15,0x0A},
    {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11},
    {0x11,0x11,0x0A,0x04,0x04,0x04,0x04},
    {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F},
    {0x0E,0x08,0x08,0x08,0x08,0x08,0x0E},
    {0x00,0x10,0x08,0x04,0x02,0x01,0x00},
    {0x0E,0x02,0x02,0x02,0x02,0x02,0x0E},
    {0x04,0x0A,0x11,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x1F},
    {0x04,0x04,0x02,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x0E,0x01,0x0F,0x11,0x0F},
    {0x10,0x10,0x16,0x19,0x11,0x11,0x1E},
    {0x00,0x00,0x0E,0x10,0x10,0x11,0x0E},
    {0x01,0x01,0x0D,0x13,0x11,0x11,0x0F},
    {0x00,0x00,0x0E,0x11,0x1F,0x10,0x0E},
    {0x06,0x09,0x08,0x1C,0x08,0x08,0x08},
    {0x00,0x00,0x0F,0x11,0x0F,0x01,0x0E},
    {0x10,0x10,0x16,0x19,0x11,0x11,0x11},
    {0x04,0x00,0x0C,0x04,0x04,0x04,0x0E},
    {0x02,0x00,0x06,0x02,0x02,0x12,0x0C},
    {0x10,0x10,0x12,0x14,0x18,0x14,0x12},
    {0x0C,0x04,0x04,0x04,0x04,0x04,0x0E},
    {0x00,0x00,0x1A,0x15,0x15,0x11,0x11},
    {0x00,0x00,0x16,0x19,0x11,0x11,0x11},
    {0x00,0x00,0x0E,0x11,0x11,0x11,0x0E},
    {0x00,0x00,0x1E,0x11,0x1E,0x10,0x10},
    {0x00,0x00,0x0D,0x13,0x0F,0x01,0x01},
    {0x00,0x00,0x16,0x19,0x10,0x10,0x10},
    {0x00,0x00,0x0E,0x10,0x0E,0x01,0x1E},
    {0x08,0x08,0x1C,0x08,0x08,0x09,0x06},
    {0x00,0x00,0x11,0x11,0x11,0x13,0x0D},
    {0x00,0x00,0x11,0x11,0x11,0x0A,0x04},
    {0x00,0x00,0x11,0x11,0x15,0x15,0x0A},
    {0x00,0x00,0x11,0x0A,0x04,0x0A,0x11},
    {0x00,0x00,0x11,0x11,0x0F,0x01,0x0E},
    {0x00,0x00,0x1F,0x02,0x04,0x08,0x1F},
    {0x02,0x04,0x04,0x08,0x04,0x04,0x02},
    {0x04,0x04,0x04,0x04,0x04,0x04,0x04},
    {0x08,0x04,0x04,0x02,0x04,0x04,0x08},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

static uint64_t CreateDefaultFontAtlas(forge::ForgeBackend_OpenGL& backend) {
    const uint32_t atlasSize = 512;
    std::vector<uint8_t> pixels(atlasSize * atlasSize * 4, 0);

    for (int c = 32; c < 127; ++c) {
        int row = (c - 32) / 16;
        int col = (c - 32) % 16;
        int gx = col * 32 + 5;
        int gy = row * 32 + 4;

        int fontIdx = c - 32;
        for (int r = 0; r < 7; ++r) {
            uint8_t bits = s_Font5x7[fontIdx][r];
            for (int b = 0; b < 5; ++b) {
                bool on = (bits & (1 << (4 - b))) != 0;
                if (on) {
                    for (int sy = 0; sy < 3; ++sy) {
                        for (int sx = 0; sx < 3; ++sx) {
                            int px = gx + b * 3 + sx;
                            int py = gy + r * 3 + sy;
                            if (px < (int)atlasSize && py < (int)atlasSize) {
                                size_t idx = (py * atlasSize + px) * 4;
                                pixels[idx + 0] = 255;
                                pixels[idx + 1] = 255;
                                pixels[idx + 2] = 255;
                                pixels[idx + 3] = 255;
                            }
                        }
                    }
                }
            }
        }
    }

    return backend.CreateTexture(atlasSize, atlasSize, pixels.data(), true);
}

int main() {
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ForgeUI - Hardware OpenGL 3.3 Interactive Demo", nullptr, nullptr);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetCursorPosCallback(window, MousePosCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetCharCallback(window, CharCallback);
    glfwSetKeyCallback(window, KeyCallback);

    forge::SetCurrentContext(&g_Context);

    if (!g_Backend.Initialize()) {
        printf("Failed to initialize ForgeUI OpenGL 3.3 backend!\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    uint64_t fontAtlas = CreateDefaultFontAtlas(g_Backend);
    g_Context.GetFont().SetAtlasTextureHandle(fontAtlas);

    float winX = 40.0f;
    float winY = 40.0f;
    float winW = 540.0f;
    float winH = 640.0f;

    float vpX = 610.0f;
    float vpY = 40.0f;
    float vpW = 630.0f;
    float vpH = 640.0f;

    const char* tabs[] = { "Display & 3D Scene", "Reactive Audio", "Simulation Speed", "Preset Profiles" };
    int32_t activeTab = 0;

    char profileName[64] = "Forge_Operator_01";
    float fov = 90.0f;
    float brightness = 1.0f;
    float renderScale = 1.0f;
    bool rayTracing = true;
    bool wireframeMode = false;
    bool showTelemetry = true;

    float masterVol = 0.85f;
    bool muteAll = false;

    float rotSpeed = 1.25f;
    float waveFreq = 4.0f;

    const char* qualityPresets[] = { "Low Performance", "Balanced Mode", "High Fidelity" };
    int32_t presetIndex = 2;
    bool presetDropdownOpen = false;

    forge::ForgeColor themeAccent(0.18f, 0.58f, 0.96f, 1.0f);
    bool advancedColorOpen = false;

    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    double fps = 60.0;
    float simTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        frameCount++;
        double elapsed = std::chrono::duration<double>(currentTime - lastTime).count();
        if (elapsed >= 0.5) {
            fps = frameCount / elapsed;
            frameCount = 0;
            lastTime = currentTime;
        }
        simTime += 0.016f;

        int displayW = 0, displayH = 0;
        glfwGetFramebufferSize(window, &displayW, &displayH);

        static int prevW = 0, prevH = 0;
        if (displayW != prevW || displayH != prevH) {
            float fW = static_cast<float>(displayW);
            float fH = static_cast<float>(displayH);
            winX = fW * 0.025f;
            winY = fH * 0.04f;
            winW = fW * 0.465f;
            winH = fH * 0.92f;

            vpX = fW * 0.51f;
            vpY = fH * 0.04f;
            vpW = fW * 0.465f;
            vpH = fH * 0.92f;

            prevW = displayW;
            prevH = displayH;
        }

        g_Context.BeginFrame();
        glfwPollEvents();
        g_Backend.BeginFrame(static_cast<uint32_t>(displayW), static_cast<uint32_t>(displayH));

        if (forge::BeginWindow("Engine Configuration & Parameters", &winX, &winY, &winW, &winH)) {
            forge::TabBar("MainSettingsTabs", tabs, 4, &activeTab);

            if (activeTab == 0) {
                forge::Label("=== Live 3D Viewport Controls ===", 14.0f, forge::ForgeColor(0.4f, 0.8f, 1.0f, 1.0f));
                forge::TextInput("Profile Identity", profileName, sizeof(profileName), forge::ForgeDimension::Px(winW - 32.0f));
                
                forge::SliderFloat("Field of View (Zoom)", &fov, 60.0f, 120.0f, forge::ForgeDimension::Px(winW - 32.0f));
                forge::SliderFloat("Scene Ambient Light", &brightness, 0.5f, 2.0f, forge::ForgeDimension::Px(winW - 32.0f));
                forge::SliderFloat("3D Geometry Scale", &renderScale, 0.5f, 1.8f, forge::ForgeDimension::Px(winW - 32.0f));

                forge::Checkbox("Wireframe Shading Mode", &wireframeMode);
                forge::Checkbox("Neon Specular Glow", &rayTracing);
                forge::Checkbox("Display Performance Telemetry", &showTelemetry);

                if (forge::CollapsingHeader("Theme Accent Color", &advancedColorOpen)) {
                    forge::ColorPicker("Accent Color", &themeAccent, forge::ForgeDimension::Px(winW - 32.0f));
                }
            } else if (activeTab == 1) {
                forge::Label("=== Reactive Audio Equalizer ===", 14.0f, forge::ForgeColor(0.4f, 0.9f, 0.6f, 1.0f));
                forge::Checkbox("Mute Equalizer Output", &muteAll);
                forge::SliderFloat("Master Wave Amplitude", &masterVol, 0.0f, 1.0f, forge::ForgeDimension::Px(winW - 32.0f));

                forge::Label("Live Audio Buffer Output:", 12.0f, forge::ForgeColor(0.8f, 0.8f, 0.8f, 1.0f));
                float audioLoad = muteAll ? 0.0f : masterVol;
                forge::ProgressBar(audioLoad, forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(16.0f), themeAccent);
            } else if (activeTab == 2) {
                forge::Label("=== Simulation Dynamics ===", 14.0f, forge::ForgeColor(1.0f, 0.75f, 0.3f, 1.0f));
                forge::SliderFloat("Cube Rotation Speed", &rotSpeed, 0.0f, 5.0f, forge::ForgeDimension::Px(winW - 32.0f));
                forge::SliderFloat("Audio Wave Frequency", &waveFreq, 1.0f, 10.0f, forge::ForgeDimension::Px(winW - 32.0f));

                if (forge::Button("Reset Rotation Speed to Normal", forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(30.0f))) {
                    rotSpeed = 1.25f;
                    waveFreq = 4.0f;
                }
            } else {
                forge::Label("=== Graphics Quality Presets ===", 14.0f, forge::ForgeColor(0.9f, 0.5f, 0.9f, 1.0f));
                if (forge::Dropdown("Active Profile", &presetIndex, qualityPresets, 3, &presetDropdownOpen, forge::ForgeDimension::Px(winW - 32.0f))) {
                    if (presetIndex == 0) {
                        fov = 75.0f; renderScale = 0.75f; brightness = 0.7f; rayTracing = false; wireframeMode = true;
                    } else if (presetIndex == 1) {
                        fov = 85.0f; renderScale = 1.0f; brightness = 1.0f; rayTracing = false; wireframeMode = false;
                    } else if (presetIndex == 2) {
                        fov = 90.0f; renderScale = 1.0f; brightness = 1.2f; rayTracing = true; wireframeMode = false;
                    }
                }

                if (forge::Button("Reset All Parameters to Default", forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(32.0f))) {
                    fov = 90.0f;
                    brightness = 1.0f;
                    renderScale = 1.0f;
                    masterVol = 0.85f;
                    muteAll = false;
                    rotSpeed = 1.25f;
                    waveFreq = 4.0f;
                    rayTracing = true;
                    wireframeMode = false;
                    showTelemetry = true;
                    themeAccent = forge::ForgeColor(0.18f, 0.58f, 0.96f, 1.0f);
                }
            }
        }
        forge::EndWindow();

        if (forge::BeginWindow("Live 3D Viewport & Audio Equalizer", &vpX, &vpY, &vpW, &vpH)) {
            forge::Label("Real-Time Reactive Simulation Canvas:", 14.0f, themeAccent);

            auto& drawList = g_Context.GetDrawList();
            float canvasX = vpX + 16.0f;
            float canvasY = vpY + 70.0f;
            float canvasW = vpW - 32.0f;
            float canvasH = 200.0f;

            float bgR = 0.08f * brightness;
            float bgG = 0.10f * brightness;
            float bgB = 0.14f * brightness;
            drawList.AddRoundedRect(forge::ForgeVec2(canvasX, canvasY), forge::ForgeVec2(canvasX + canvasW, canvasY + canvasH), 6.0f, forge::ForgeColor(bgR, bgG, bgB, 1.0f).ToRGBA8());
            drawList.AddBorder(forge::ForgeVec2(canvasX, canvasY), forge::ForgeVec2(canvasX + canvasW, canvasY + canvasH), 6.0f, 1.0f, themeAccent.ToRGBA8());

            float centerX = canvasX + canvasW * 0.5f;
            float centerY = canvasY + canvasH * 0.5f;
            float fovScale = (fov / 90.0f) * renderScale;
            float rot = simTime * rotSpeed;
            float cosR = std::cos(rot);
            float sinR = std::sin(rot);

            float boxSize = 40.0f * fovScale;
            float hx = boxSize * 0.7f;
            float hy = boxSize * 0.7f;

            forge::ForgeVec2 p0(centerX + (-hx * cosR - -hy * sinR), centerY + (-hx * sinR + -hy * cosR));
            forge::ForgeVec2 p1(centerX + ( hx * cosR - -hy * sinR), centerY + ( hx * sinR + -hy * cosR));
            forge::ForgeVec2 p2(centerX + ( hx * cosR -  hy * sinR), centerY + ( hx * sinR +  hy * cosR));
            forge::ForgeVec2 p3(centerX + (-hx * cosR -  hy * sinR), centerY + (-hx * sinR +  hy * cosR));

            if (!wireframeMode) {
                drawList.AddRoundedRect(forge::ForgeVec2(centerX - hx, centerY - hy), forge::ForgeVec2(centerX + hx, centerY + hy), 4.0f, themeAccent.ToRGBA8());
            }
            drawList.AddBorder(forge::ForgeVec2(centerX - hx, centerY - hy), forge::ForgeVec2(centerX + hx, centerY + hy), 4.0f, 2.0f, rayTracing ? 0xFF00FFFF : 0xFFFFFFFF);

            drawList.AddCircle(p0, 3.0f, rayTracing ? 0xFF00FFFF : 0xFFFFFFFF);
            drawList.AddCircle(p1, 3.0f, rayTracing ? 0xFF00FFFF : 0xFFFFFFFF);
            drawList.AddCircle(p2, 3.0f, rayTracing ? 0xFF00FFFF : 0xFFFFFFFF);
            drawList.AddCircle(p3, 3.0f, rayTracing ? 0xFF00FFFF : 0xFFFFFFFF);

            float telY = canvasY + canvasH + 16.0f;
            if (showTelemetry) {
                char buf1[128], buf2[128], buf3[128];
                snprintf(buf1, sizeof(buf1), "Live Engine FPS: %.1f | Frame Time: %.2f ms", fps, (fps > 0.0 ? 1000.0 / fps : 16.6));
                snprintf(buf2, sizeof(buf2), "Render Scale: %.0f%% | Field of View: %.1f deg", renderScale * 100.0f, fov);
                snprintf(buf3, sizeof(buf3), "Glow Specular: %s | Shading: %s | Speed: %.2fx", rayTracing ? "ENABLED" : "OFF", wireframeMode ? "WIREFRAME" : "SOLID", rotSpeed);

                auto& font = g_Context.GetFont();
                font.AppendTextToDrawList(&drawList, buf1, forge::ForgeVec2(canvasX, telY), 13.0f, forge::ForgeColor(0.3f, 1.0f, 0.5f, 1.0f).ToRGBA8());
                font.AppendTextToDrawList(&drawList, buf2, forge::ForgeVec2(canvasX, telY + 20.0f), 13.0f, forge::ForgeColor(0.9f, 0.9f, 0.9f, 1.0f).ToRGBA8());
                font.AppendTextToDrawList(&drawList, buf3, forge::ForgeVec2(canvasX, telY + 40.0f), 13.0f, forge::ForgeColor(0.7f, 0.8f, 1.0f, 1.0f).ToRGBA8());
            }

            float eqY = telY + 70.0f;
            auto& font = g_Context.GetFont();
            font.AppendTextToDrawList(&drawList, "Reactive Audio Frequency Equalizer (18 Bands):", forge::ForgeVec2(canvasX, eqY), 13.0f, forge::ForgeColor(0.85f, 0.85f, 0.85f, 1.0f).ToRGBA8());

            const int numBands = 18;
            float bandW = (canvasW - (numBands - 1) * 4.0f) / numBands;
            float effVol = muteAll ? 0.0f : masterVol;
            float eqBarsY = eqY + 24.0f;

            for (int b = 0; b < numBands; ++b) {
                float freqAmp = std::abs(std::sin(simTime * waveFreq + b * 0.5f)) * 0.7f + std::abs(std::cos(simTime * 2.0f + b * 0.3f)) * 0.3f;
                float bandH = std::max(6.0f, freqAmp * 60.0f * effVol);
                float bx = canvasX + b * (bandW + 4.0f);
                float by = eqBarsY + (60.0f - bandH);

                forge::ForgeColor bandCol = (b % 2 == 0) ? themeAccent : forge::ForgeColor(themeAccent.g, themeAccent.b, themeAccent.r, 1.0f);
                drawList.AddRoundedRect(forge::ForgeVec2(bx, by), forge::ForgeVec2(bx + bandW, eqBarsY + 60.0f), 2.0f, bandCol.ToRGBA8());
            }
        }
        forge::EndWindow();

        g_Context.EndFrame();

        g_Backend.RenderDrawList(&g_Context.GetDrawList());
        g_Backend.EndFrame();

        glfwSwapBuffers(window);
    }

    g_Backend.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
