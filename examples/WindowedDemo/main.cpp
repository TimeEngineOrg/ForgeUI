#include "ForgeUI/Core/ForgeContext.h"
#include "ForgeUI/Widgets/ForgeWidgets.h"
#include "ForgeUI/Backend/ForgeBackend_OpenGL.h"
#include "ForgeUI/Font/ForgeDefaultFontData.h"

#include <GLFW/glfw3.h>
#include <cstdio>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>

static forge::ForgeContext g_Context;
static forge::ForgeBackend_OpenGL g_Backend;
static bool s_RequestScreenshot = false;

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
    if (action == GLFW_PRESS && key == GLFW_KEY_F12) {
        s_RequestScreenshot = true;
    }
}

static bool SaveScreenshotTGA(const char* filename, int width, int height) {
    if (width <= 0 || height <= 0) return false;
    std::vector<uint8_t> pixels(width * height * 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Invert rows vertically for standard top-down image layout
    std::vector<uint8_t> flipped(width * height * 4);
    int rowPitch = width * 4;
    for (int y = 0; y < height; ++y) {
        std::memcpy(&flipped[y * rowPitch], &pixels[(height - 1 - y) * rowPitch], rowPitch);
    }

    // Convert RGBA to BGRA for standard 32-bit true-color TGA
    for (int i = 0; i < width * height; ++i) {
        uint8_t r = flipped[i * 4 + 0];
        uint8_t b = flipped[i * 4 + 2];
        flipped[i * 4 + 0] = b;
        flipped[i * 4 + 2] = r;
    }

    FILE* f = fopen(filename, "wb");
    if (!f) return false;

    uint8_t header[18] = { 0 };
    header[2] = 2; // Uncompressed true-color image
    header[12] = static_cast<uint8_t>(width & 0xFF);
    header[13] = static_cast<uint8_t>((width >> 8) & 0xFF);
    header[14] = static_cast<uint8_t>(height & 0xFF);
    header[15] = static_cast<uint8_t>((height >> 8) & 0xFF);
    header[16] = 32; // 32 bits per pixel
    header[17] = 8;  // 8 bits attribute (alpha channel)

    fwrite(header, 1, 18, f);
    fwrite(flipped.data(), 1, flipped.size(), f);
    fclose(f);

    printf("[ForgeUI] Captured showcase screenshot to '%s' (%dx%d)\n", filename, width, height);
    return true;
}

static bool SaveScreenshotBMP(const char* filename, int width, int height) {
    if (width <= 0 || height <= 0) return false;
    std::vector<uint8_t> pixels(width * height * 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Invert rows vertically and swap R/B for BMP
    std::vector<uint8_t> flipped(width * height * 4);
    int rowPitch = width * 4;
    for (int y = 0; y < height; ++y) {
        std::memcpy(&flipped[y * rowPitch], &pixels[(height - 1 - y) * rowPitch], rowPitch);
    }
    for (int i = 0; i < width * height; ++i) {
        uint8_t r = flipped[i * 4 + 0];
        uint8_t b = flipped[i * 4 + 2];
        flipped[i * 4 + 0] = b;
        flipped[i * 4 + 2] = r;
    }

    FILE* f = fopen(filename, "wb");
    if (!f) return false;

    uint32_t imageSize = width * height * 4;
    uint32_t fileSize = 54 + imageSize;

    uint8_t header[54] = {
        'B', 'M',
        static_cast<uint8_t>(fileSize & 0xFF), static_cast<uint8_t>((fileSize >> 8) & 0xFF), static_cast<uint8_t>((fileSize >> 16) & 0xFF), static_cast<uint8_t>((fileSize >> 24) & 0xFF),
        0, 0, 0, 0,
        54, 0, 0, 0,
        40, 0, 0, 0,
        static_cast<uint8_t>(width & 0xFF), static_cast<uint8_t>((width >> 8) & 0xFF), static_cast<uint8_t>((width >> 16) & 0xFF), static_cast<uint8_t>((width >> 24) & 0xFF),
        static_cast<uint8_t>(-height & 0xFF), static_cast<uint8_t>((-height >> 8) & 0xFF), static_cast<uint8_t>((-height >> 16) & 0xFF), static_cast<uint8_t>((-height >> 24) & 0xFF),
        1, 0,
        32, 0,
        0, 0, 0, 0,
        static_cast<uint8_t>(imageSize & 0xFF), static_cast<uint8_t>((imageSize >> 8) & 0xFF), static_cast<uint8_t>((imageSize >> 16) & 0xFF), static_cast<uint8_t>((imageSize >> 24) & 0xFF),
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };

    fwrite(header, 1, 54, f);
    fwrite(flipped.data(), 1, imageSize, f);
    fclose(f);

    printf("[ForgeUI] Captured showcase screenshot to '%s' (%dx%d)\n", filename, width, height);
    return true;
}

static uint64_t CreateDefaultFontAtlas(forge::ForgeBackend_OpenGL& backend) {
    const uint32_t atlasSize = 512;
    std::vector<uint8_t> pixels(atlasSize * atlasSize * 4, 0);
    forge::UnpackDefaultFontAtlas(pixels.data(), atlasSize);
    return backend.CreateTexture(atlasSize, atlasSize, pixels.data(), true);
}

int main(int argc, char** argv) {
    bool autoScreenshot = false;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--screenshot") == 0) {
            autoScreenshot = true;
        }
    }

    glfwSetErrorCallback([](int error, const char* description) {
        fprintf(stderr, "[GLFW Error %d]: %s\n", error, description ? description : "Unknown");
    });

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ForgeUI - Hardware OpenGL 3.3 Interactive Demo", nullptr, nullptr);
    if (!window) {
        const char* description = nullptr;
        int code = glfwGetError(&description);
        fprintf(stderr, "[ERROR] Failed to create OpenGL 3.3 Core GLFW window (code: %d): %s\n", code, description ? description : "Unknown error");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Register custom GL proc loader
    g_Backend.SetCustomLoader((forge::ForgeGLLoaderFunc)glfwGetProcAddress);

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

    const char* tabs[] = { "Display & 3D Scene", "Reactive Audio", "Simulation Dynamics", "Preset Profiles" };
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

    forge::ForgeColor themeAccent(0.12f, 0.62f, 0.98f, 1.0f);
    bool advancedColorOpen = false;

    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    double fps = 60.0;
    float simTime = 0.0f;
    double layoutUs = 0.0;

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

        int windowW = 0, windowH = 0;
        int fbW = 0, fbH = 0;
        glfwGetWindowSize(window, &windowW, &windowH);
        glfwGetFramebufferSize(window, &fbW, &fbH);

        float dpiScaleX = (windowW > 0) ? static_cast<float>(fbW) / static_cast<float>(windowW) : 1.0f;
        float dpiScaleY = (windowH > 0) ? static_cast<float>(fbH) / static_cast<float>(windowH) : 1.0f;

        static int prevW = 0, prevH = 0;
        if (windowW != prevW || windowH != prevH) {
            float fW = static_cast<float>(windowW);
            float fH = static_cast<float>(windowH);
            winX = fW * 0.025f;
            winY = fH * 0.04f;
            winW = fW * 0.465f;
            winH = fH * 0.92f;

            vpX = fW * 0.51f;
            vpY = fH * 0.04f;
            vpW = fW * 0.465f;
            vpH = fH * 0.92f;

            prevW = windowW;
            prevH = windowH;
        }

        g_Context.SetRootDimensions(static_cast<float>(windowW), static_cast<float>(windowH));
        g_Context.BeginFrame();
        glfwPollEvents();

        g_Backend.SetDisplayScale(dpiScaleX, dpiScaleY);
        g_Backend.BeginFrame(static_cast<uint32_t>(fbW), static_cast<uint32_t>(fbH));

        glClearColor(0.07f, 0.08f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (forge::BeginWindow("Engine Configuration & Parameters", &winX, &winY, &winW, &winH)) {
            forge::TabBar("MainSettingsTabs", tabs, 4, &activeTab);

            if (activeTab == 0) {
                forge::Label("=== Live 3D Viewport Controls ===", 14.0f, forge::ForgeColor(0.20f, 0.70f, 1.0f, 1.0f));
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

                if (forge::Button("Capture Showcase Screenshot (F12)", forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(32.0f))) {
                    s_RequestScreenshot = true;
                }
            } else if (activeTab == 1) {
                forge::Label("=== Reactive Audio Equalizer ===", 14.0f, forge::ForgeColor(0.25f, 0.85f, 0.65f, 1.0f));
                forge::Checkbox("Mute Equalizer Output", &muteAll);
                forge::SliderFloat("Master Wave Amplitude", &masterVol, 0.0f, 1.0f, forge::ForgeDimension::Px(winW - 32.0f));

                forge::Label("Live Audio Buffer Output:", 13.0f, forge::ForgeColor(0.85f, 0.88f, 0.92f, 1.0f));
                float audioLoad = muteAll ? 0.0f : masterVol;
                forge::ProgressBar(audioLoad, forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(16.0f), themeAccent);

                if (forge::Button("Capture Showcase Screenshot (F12)", forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(32.0f))) {
                    s_RequestScreenshot = true;
                }
            } else if (activeTab == 2) {
                forge::Label("=== Simulation Dynamics ===", 14.0f, forge::ForgeColor(1.0f, 0.70f, 0.25f, 1.0f));
                forge::SliderFloat("Cube Rotation Speed", &rotSpeed, 0.0f, 5.0f, forge::ForgeDimension::Px(winW - 32.0f));
                forge::SliderFloat("Audio Wave Frequency", &waveFreq, 1.0f, 10.0f, forge::ForgeDimension::Px(winW - 32.0f));

                if (forge::Button("Reset Rotation Speed to Normal", forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(30.0f))) {
                    rotSpeed = 1.25f;
                    waveFreq = 4.0f;
                }

                if (forge::Button("Capture Showcase Screenshot (F12)", forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(32.0f))) {
                    s_RequestScreenshot = true;
                }
            } else {
                forge::Label("=== Graphics Quality Presets ===", 14.0f, forge::ForgeColor(0.85f, 0.55f, 0.95f, 1.0f));
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
                    themeAccent = forge::ForgeColor(0.12f, 0.62f, 0.98f, 1.0f);
                }

                if (forge::Button("Capture Showcase Screenshot (F12)", forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(32.0f))) {
                    s_RequestScreenshot = true;
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
            float canvasH = 210.0f;

            // Viewport canvas
            uint32_t canvasBg = forge::ForgeColor(0.04f, 0.06f, 0.09f, 1.0f).ToRGBA8();
            uint32_t canvasBorder = forge::ForgeColor(0.18f, 0.24f, 0.32f, 1.0f).ToRGBA8();
            drawList.AddRoundedRect(forge::ForgeVec2(canvasX, canvasY), forge::ForgeVec2(canvasX + canvasW, canvasY + canvasH), 6.0f, canvasBg);
            drawList.AddBorder(forge::ForgeVec2(canvasX, canvasY), forge::ForgeVec2(canvasX + canvasW, canvasY + canvasH), 6.0f, 1.0f, canvasBorder);

            // Viewport grid
            uint32_t gridCol = forge::ForgeColor(0.09f, 0.12f, 0.18f, 1.0f).ToRGBA8();
            for (float gx = canvasX + 30.0f; gx < canvasX + canvasW; gx += 30.0f) {
                drawList.AddRect(forge::ForgeVec2(gx, canvasY + 1.0f), forge::ForgeVec2(gx + 1.0f, canvasY + canvasH - 1.0f), gridCol);
            }
            for (float gy = canvasY + 30.0f; gy < canvasY + canvasH; gy += 30.0f) {
                drawList.AddRect(forge::ForgeVec2(canvasX + 1.0f, gy), forge::ForgeVec2(canvasX + canvasW - 1.0f, gy + 1.0f), gridCol);
            }

            float centerX = canvasX + canvasW * 0.5f;
            float centerY = canvasY + canvasH * 0.5f;

            // Crosshair
            uint32_t crosshairCol = forge::ForgeColor(0.20f, 0.28f, 0.38f, 0.8f).ToRGBA8();
            drawList.AddRect(forge::ForgeVec2(centerX - 12.0f, centerY), forge::ForgeVec2(centerX + 12.0f, centerY + 1.0f), crosshairCol);
            drawList.AddRect(forge::ForgeVec2(centerX, centerY - 12.0f), forge::ForgeVec2(centerX + 1.0f, centerY + 12.0f), crosshairCol);

            float fovScale = (fov / 90.0f) * renderScale;
            float rot = simTime * rotSpeed;
            float cosR = std::cos(rot);
            float sinR = std::sin(rot);

            float boxSize = 42.0f * fovScale;
            float hx = boxSize * 0.707f;
            float hy = boxSize * 0.707f;

            forge::ForgeVec2 p0(centerX + (-hx * cosR - -hy * sinR), centerY + (-hx * sinR + -hy * cosR));
            forge::ForgeVec2 p1(centerX + ( hx * cosR - -hy * sinR), centerY + ( hx * sinR + -hy * cosR));
            forge::ForgeVec2 p2(centerX + ( hx * cosR -  hy * sinR), centerY + ( hx * sinR +  hy * cosR));
            forge::ForgeVec2 p3(centerX + (-hx * cosR -  hy * sinR), centerY + (-hx * sinR +  hy * cosR));

            if (!wireframeMode) {
                uint32_t solidFill = forge::ForgeColor(themeAccent.r * 0.25f, themeAccent.g * 0.35f, themeAccent.b * 0.50f, 0.85f).ToRGBA8();
                drawList.AddRoundedRect(forge::ForgeVec2(centerX - hx, centerY - hy), forge::ForgeVec2(centerX + hx, centerY + hy), 4.0f, solidFill);
            }
            uint32_t geomBorder = rayTracing ? forge::ForgeColor(0.30f, 0.85f, 1.0f, 1.0f).ToRGBA8() : 0xFFFFFFFF;
            drawList.AddBorder(forge::ForgeVec2(centerX - hx, centerY - hy), forge::ForgeVec2(centerX + hx, centerY + hy), 4.0f, 2.0f, geomBorder);

            // Specular vertices
            uint32_t specCol = rayTracing ? 0xFF00FFFF : 0xFFE2E8F0;
            drawList.AddCircle(p0, 3.5f, specCol);
            drawList.AddCircle(p1, 3.5f, specCol);
            drawList.AddCircle(p2, 3.5f, specCol);
            drawList.AddCircle(p3, 3.5f, specCol);

            float telY = canvasY + canvasH + 16.0f;
            if (showTelemetry) {
                const auto& stats = g_Backend.GetBackendStats();
                char buf1[128], buf2[128], buf3[128], buf4[128];
                snprintf(buf1, sizeof(buf1), "FPS: %.1f | Frame: %.2f ms | Layout: %.1f us", fps, (fps > 0.0 ? 1000.0 / fps : 16.6), layoutUs);
                snprintf(buf2, sizeof(buf2), "Draw Calls: %u | Vertices: %u | Indices: %u", stats.drawCallCount, stats.vertexCount, stats.indexCount);
                snprintf(buf3, sizeof(buf3), "VBO Alloc: %.1f KB | DPI: %.2fx | FB: %dx%d", (float)stats.vboBytes / 1024.0f, dpiScaleX, fbW, fbH);
                snprintf(buf4, sizeof(buf4), "Glow Specular: %s | Shading: %s | Speed: %.2fx", rayTracing ? "ENABLED" : "OFF", wireframeMode ? "WIREFRAME" : "SOLID", rotSpeed);

                auto& font = g_Context.GetFont();
                font.AppendTextToDrawList(&drawList, buf1, forge::ForgeVec2(canvasX, telY), 13.0f, forge::ForgeColor(0.25f, 0.90f, 0.55f, 1.0f).ToRGBA8());
                font.AppendTextToDrawList(&drawList, buf2, forge::ForgeVec2(canvasX, telY + 18.0f), 13.0f, forge::ForgeColor(0.25f, 0.75f, 1.0f, 1.0f).ToRGBA8());
                font.AppendTextToDrawList(&drawList, buf3, forge::ForgeVec2(canvasX, telY + 36.0f), 13.0f, forge::ForgeColor(0.85f, 0.88f, 0.92f, 1.0f).ToRGBA8());
                font.AppendTextToDrawList(&drawList, buf4, forge::ForgeVec2(canvasX, telY + 54.0f), 13.0f, forge::ForgeColor(0.95f, 0.75f, 0.25f, 1.0f).ToRGBA8());
            }

            float eqY = telY + (showTelemetry ? 78.0f : 16.0f);
            auto& font = g_Context.GetFont();
            font.AppendTextToDrawList(&drawList, "Reactive Audio Frequency Equalizer (18 Bands):", forge::ForgeVec2(canvasX, eqY), 13.0f, forge::ForgeColor(0.88f, 0.91f, 0.96f, 1.0f).ToRGBA8());

            const int numBands = 18;
            float bandW = (canvasW - (numBands - 1) * 4.0f) / numBands;
            float effVol = muteAll ? 0.0f : masterVol;
            float eqBarsY = eqY + 22.0f;
            float maxBarH = 55.0f;

            uint32_t slotBg = forge::ForgeColor(0.10f, 0.13f, 0.18f, 1.0f).ToRGBA8();

            for (int b = 0; b < numBands; ++b) {
                float freqAmp = std::abs(std::sin(simTime * waveFreq + b * 0.45f)) * 0.7f + std::abs(std::cos(simTime * 2.0f + b * 0.28f)) * 0.3f;
                float bandH = std::max(4.0f, freqAmp * maxBarH * effVol);
                float bx = canvasX + b * (bandW + 4.0f);
                float by = eqBarsY + (maxBarH - bandH);

                drawList.AddRoundedRect(forge::ForgeVec2(bx, eqBarsY), forge::ForgeVec2(bx + bandW, eqBarsY + maxBarH), 2.0f, slotBg);

                float t = static_cast<float>(b) / static_cast<float>(numBands - 1);
                float barR = 0.06f + t * 0.60f;
                float barG = 0.75f - t * 0.38f;
                float barB = 0.96f;
                forge::ForgeColor barCol(barR, barG, barB, 1.0f);

                drawList.AddRoundedRect(forge::ForgeVec2(bx, by), forge::ForgeVec2(bx + bandW, eqBarsY + maxBarH), 2.0f, barCol.ToRGBA8());
            }
        }
        forge::EndWindow();

        auto tLayoutStart = std::chrono::high_resolution_clock::now();
        g_Context.EndFrame();
        auto tLayoutEnd = std::chrono::high_resolution_clock::now();
        layoutUs = std::chrono::duration<double, std::micro>(tLayoutEnd - tLayoutStart).count();

        g_Backend.RenderDrawList(&g_Context.GetDrawList());
        g_Backend.EndFrame();

        if (autoScreenshot && frameCount >= 10) {
            s_RequestScreenshot = true;
        }

        if (s_RequestScreenshot) {
            s_RequestScreenshot = false;
            SaveScreenshotTGA("forge_showcase.tga", fbW, fbH);
            SaveScreenshotBMP("docs/images/forge_showcase.bmp", fbW, fbH);
            if (autoScreenshot) {
                break;
            }
        }

        glfwSwapBuffers(window);
    }

    g_Backend.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
