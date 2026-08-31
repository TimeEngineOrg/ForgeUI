#include "ForgeUI.h"
#include <cstdio>
#include <chrono>

int main() {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    forge::ForgeBackend_OpenGL glBackend;
    glBackend.Initialize();

    forge::ForgeBackend_CPU cpuBackend;
    cpuBackend.Initialize();

    float sensitivity = 1.5f;
    int32_t maxFps = 144;
    bool vsync = true;
    bool showMetrics = true;
    char playerName[64] = "Player1";
    float scrollY = 0.0f;

    const char* tabs[] = {"Settings", "Inspector", "Diagnostics"};
    int32_t activeTab = 0;

    const char* antialiasingModes[] = {"Off", "FXAA", "MSAA 4x", "TAA"};
    int32_t aaIndex = 2;
    bool aaDropdownOpen = false;

    forge::ForgeColor themeAccent(0.18f, 0.52f, 0.92f, 1.0f);
    float loadingProgress = 0.72f;
    bool advancedSectionOpen = true;

    float winX = 50.0f;
    float winY = 50.0f;
    float winW = 380.0f;
    float winH = 560.0f;

    printf("Running ForgeUI Interactive Multi-Widget Showcase Simulation (60 frames)...\n");

    for (int frame = 0; frame < 60; ++frame) {
        auto start = std::chrono::high_resolution_clock::now();

        ctx.BeginFrame();

        forge::ForgeInputState& input = ctx.GetInput();
        input.mousePos = forge::ForgeVec2(180.0f, 160.0f);
        if (frame % 8 == 0) {
            input.mousePressed[0] = true;
            input.mouseDown[0] = true;
        }

        glBackend.BeginFrame(1920, 1080);
        cpuBackend.BeginFrame(1920, 1080);

        if (forge::BeginWindow("Forge Engine Control Center", &winX, &winY, &winW, &winH)) {
            forge::TabBar("MainTabs", tabs, 3, &activeTab);

            if (activeTab == 0) {
                forge::Label("=== Engine Graphics Configuration ===", 15.0f, forge::ForgeColor(0.35f, 0.75f, 0.95f, 1.0f));

                forge::TextInput("Player Profile", playerName, sizeof(playerName));
                forge::SliderFloat("Mouse Sensitivity", &sensitivity, 0.1f, 5.0f);
                forge::SliderInt("Target Frame Rate", &maxFps, 30, 240);
                forge::Checkbox("Enable V-Sync Synchronization", &vsync);
                forge::Checkbox("Display Performance Overlay", &showMetrics);

                forge::Dropdown("Anti-Aliasing Filter", &aaIndex, antialiasingModes, 4, &aaDropdownOpen);

                forge::Label("Asset Streaming Status:", 13.0f, forge::ForgeColor(0.8f, 0.8f, 0.8f, 1.0f));
                forge::ProgressBar(loadingProgress, forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(14.0f), themeAccent);

                if (forge::CollapsingHeader("Advanced Color Palette", &advancedSectionOpen)) {
                    forge::ColorPicker("Accent Color", &themeAccent, forge::ForgeDimension::Px(winW - 32.0f));
                }

                if (forge::Button("Apply Configuration", forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(32.0f))) {
                    printf("Applied Configuration on Frame %d!\n", frame);
                }
            } else if (activeTab == 1) {
                forge::Label("Inspector & Hierarchy View", 15.0f, forge::ForgeColor(0.9f, 0.9f, 0.9f, 1.0f));
                forge::BeginScrollView("InspectorScroll", forge::ForgeDimension::Px(winW - 32.0f), forge::ForgeDimension::Px(240.0f), &scrollY);
                for (int i = 0; i < 8; ++i) {
                    char labelBuf[32];
                    snprintf(labelBuf, sizeof(labelBuf), "Entity Node #%d (Transform)", i + 1);
                    forge::Label(labelBuf, 13.0f, forge::ForgeColor(0.7f, 0.8f, 0.9f, 1.0f));
                }
                forge::EndScrollView();
            } else {
                forge::Label("Backend Diagnostics & Memory", 15.0f, forge::ForgeColor(0.4f, 0.9f, 0.5f, 1.0f));
                forge::Label("Contiguous Memory (SoA): 0 Heap Allocations", 13.0f);
                forge::Label("Active Backend: Multi-Backend Ready", 13.0f);
            }
        }
        forge::EndWindow();

        ctx.EndFrame();

        glBackend.RenderDrawList(&ctx.GetDrawList());
        glBackend.EndFrame();

        cpuBackend.RenderDrawList(&ctx.GetDrawList());
        cpuBackend.EndFrame();

        auto end = std::chrono::high_resolution_clock::now();
        double frameMs = std::chrono::duration<double, std::milli>(end - start).count();

        if (frame == 0 || frame == 59) {
            printf("Frame %02d | Vertices: %u | Indices: %u | GPU Commands: %u | CPU Frame Time: %.4f ms\n",
                   frame,
                   ctx.GetDrawList().GetVertexCount(),
                   ctx.GetDrawList().GetIndexCount(),
                   ctx.GetDrawList().GetCommandCount(),
                   frameMs);
        }
    }

    printf("Showcase Simulation successfully completed with 0 errors.\n");

    glBackend.Shutdown();
    cpuBackend.Shutdown();
    return 0;
}
