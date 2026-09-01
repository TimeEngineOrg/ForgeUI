#include <iostream>
#include <chrono>
#include "ForgeUI.h"
#include "imgui.h"

int main()
{
    std::cout << "====================================================\n";
    std::cout << "     ForgeUI vs Dear ImGui Head-to-Head Benchmark   \n";
    std::cout << "====================================================\n\n";

    const int ELEMENT_COUNT = 50000;
    const int WARMUP_FRAMES = 5;
    const int BENCHMARK_FRAMES = 30;

    std::cout << "Workload: " << ELEMENT_COUNT << " elements per frame across " << BENCHMARK_FRAMES << " frames.\n\n";

    IMGUI_CHECKVERSION();
    ImGuiContext* imCtx = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(1920.0f, 1080.0f);
    io.DeltaTime = 1.0f / 60.0f;
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    for (int f = 0; f < WARMUP_FRAMES; ++f)
    {
        ImGui::NewFrame();
        for (int i = 0; i < ELEMENT_COUNT; ++i)
        {
            ImGui::PushID(i);
            ImGui::Button("Test", ImVec2(100.0f, 25.0f));
            ImGui::PopID();
        }
        ImGui::Render();
    }

    auto imguiStart = std::chrono::high_resolution_clock::now();
    size_t imguiTotalVertices = 0;
    size_t imguiTotalIndices = 0;

    for (int f = 0; f < BENCHMARK_FRAMES; ++f)
    {
        ImGui::NewFrame();
        for (int i = 0; i < ELEMENT_COUNT; ++i)
        {
            ImGui::PushID(i);
            ImGui::Button("Test", ImVec2(100.0f, 25.0f));
            ImGui::PopID();
        }
        ImGui::Render();

        ImDrawData* drawData = ImGui::GetDrawData();
        imguiTotalVertices = drawData->TotalVtxCount;
        imguiTotalIndices = drawData->TotalIdxCount;
    }

    auto imguiEnd = std::chrono::high_resolution_clock::now();
    double imguiDurationMs = std::chrono::duration<double, std::milli>(imguiEnd - imguiStart).count() / BENCHMARK_FRAMES;
    ImGui::DestroyContext(imCtx);

    forge::ForgeContext forgeCtx;
    forge::SetCurrentContext(&forgeCtx);
    forge::ForgeBackend_Null nullBackend;
    nullBackend.Initialize();

    for (int f = 0; f < WARMUP_FRAMES; ++f)
    {
        forgeCtx.BeginFrame();
        for (int i = 0; i < ELEMENT_COUNT; ++i)
        {
            forge::Button("Test", forge::ForgeDimension::Px(100.0f), forge::ForgeDimension::Px(25.0f));
        }
        forgeCtx.EndFrame();
        nullBackend.RenderDrawList(&forgeCtx.GetDrawList());
    }

    auto forgeStart = std::chrono::high_resolution_clock::now();
    size_t forgeTotalVertices = 0;
    size_t forgeTotalIndices = 0;

    for (int f = 0; f < BENCHMARK_FRAMES; ++f)
    {
        forgeCtx.BeginFrame();
        for (int i = 0; i < ELEMENT_COUNT; ++i)
        {
            forge::Button("Test", forge::ForgeDimension::Px(100.0f), forge::ForgeDimension::Px(25.0f));
        }
        forgeCtx.EndFrame();
        nullBackend.RenderDrawList(&forgeCtx.GetDrawList());

        forgeTotalVertices = forgeCtx.GetDrawList().GetVertexCount();
        forgeTotalIndices = forgeCtx.GetDrawList().GetIndexCount();
    }

    auto forgeEnd = std::chrono::high_resolution_clock::now();
    double forgeDurationMs = std::chrono::duration<double, std::milli>(forgeEnd - forgeStart).count() / BENCHMARK_FRAMES;

    std::cout << "--- Performance Results (" << ELEMENT_COUNT << " elements) ---\n";
    std::cout << "Dear ImGui Frame Time : " << imguiDurationMs << " ms  (" << (ELEMENT_COUNT / (imguiDurationMs / 1000.0)) / 1000000.0 << " M elem/s)\n";
    std::cout << "  Vertices: " << imguiTotalVertices << " | Indices: " << imguiTotalIndices << "\n\n";

    std::cout << "ForgeUI Frame Time    : " << forgeDurationMs << " ms  (" << (ELEMENT_COUNT / (forgeDurationMs / 1000.0)) / 1000000.0 << " M elem/s)\n";
    std::cout << "  Vertices: " << forgeTotalVertices << " | Indices: " << forgeTotalIndices << "\n\n";

    double speedup = imguiDurationMs / forgeDurationMs;
    std::cout << "Throughput ratio (Dear ImGui / ForgeUI): " << speedup << "x\n";
    std::cout << "====================================================\n";

    return 0;
}
