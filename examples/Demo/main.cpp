#include "ForgeUI.h"
#include <cstdio>
#include <chrono>

int main() {
    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    forge::ForgeBackend_OpenGL backend;
    backend.Initialize();

    float sensitivity = 1.5f;
    int32_t maxFps = 144;
    bool vsync = true;
    bool showMetrics = true;
    char playerName[64] = "Player1";
    float scrollY = 0.0f;

    printf("Running ForgeUI Standalone Interactive Demo Simulation (60 frames)...\n");

    for (int frame = 0; frame < 60; ++frame) {
        auto start = std::chrono::high_resolution_clock::now();

        ctx.BeginFrame();

        forge::ForgeInputState& input = ctx.GetInput();
        input.mousePos = forge::ForgeVec2(150.0f, 120.0f);
        if (frame % 10 == 0) {
            input.mousePressed[0] = true;
            input.mouseDown[0] = true;
        }

        backend.BeginFrame(1920, 1080);

        if (forge::BeginPanel("Settings Panel", forge::ForgeDimension::Px(360.0f), forge::ForgeDimension::Px(500.0f))) {
            forge::Label("=== ForgeUI Engine Settings ===", 16.0f, forge::ForgeColor(0.25f, 0.65f, 0.95f, 1.0f));

            forge::TextInput("Player Name", playerName, sizeof(playerName));
            forge::SliderFloat("Mouse Sensitivity", &sensitivity, 0.1f, 5.0f);
            forge::SliderInt("Target FPS", &maxFps, 30, 240);
            forge::Checkbox("Enable VSync", &vsync);
            forge::Checkbox("Show Performance Metrics", &showMetrics);

            if (forge::Button("Apply Configuration", forge::ForgeDimension::Px(200.0f))) {
                printf("[EVENT] Configuration applied on frame %d\n", frame);
            }

            if (forge::BeginScrollView("LogList", forge::ForgeDimension::Px(330.0f), forge::ForgeDimension::Px(150.0f), &scrollY)) {
                for (int i = 0; i < 20; ++i) {
                    char logItem[32];
                    std::snprintf(logItem, sizeof(logItem), "Diagnostic Entry #%d", i);
                    forge::Label(logItem, 12.0f);
                }
                forge::EndScrollView();
            }

            forge::EndPanel();
        }

        ctx.EndFrame();

        backend.RenderDrawList(&ctx.GetDrawList());
        backend.EndFrame();

        auto end = std::chrono::high_resolution_clock::now();
        double elapsedUs = std::chrono::duration<double, std::micro>(end - start).count();

        if (frame == 59) {
            printf("\n--- Standalone Demo Summary ---\n");
            printf("Elements in DFS Storage : %zu\n", ctx.GetStorage().Count());
            printf("Total Draw Vertices     : %u\n", ctx.GetDrawList().GetVertexCount());
            printf("Total Draw Indices      : %u\n", ctx.GetDrawList().GetIndexCount());
            printf("Consolidated GPU Batches: %u\n", ctx.GetDrawList().GetCommandCount());
            printf("Frame Processing Time   : %.2f us\n", elapsedUs);
            printf("-------------------------------\n");
        }
    }

    backend.Shutdown();
    return 0;
}
