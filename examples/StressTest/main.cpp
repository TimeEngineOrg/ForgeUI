#include "ForgeUI.h"
#include <cstdio>
#include <chrono>

int main() {
    printf("====================================================\n");
    printf("     ForgeUI 100,000 UI Elements DOD Stress Test     \n");
    printf("====================================================\n");

    forge::ForgeContext ctx;
    forge::SetCurrentContext(&ctx);

    const int elementCount = 50000;

    printf("Allocating & computing layout for %d elements...\n", elementCount);

    auto startLayout = std::chrono::high_resolution_clock::now();

    ctx.BeginFrame();

    forge::ForgeElementConfig rootCfg{};
    rootCfg.layout.width = forge::ForgeDimension::Px(1920.0f);
    rootCfg.layout.height = forge::ForgeDimension::Px(1080.0f);
    rootCfg.layout.direction = forge::ForgeFlexDirection::Column;

    ctx.Begin("RootContainer", rootCfg);

    for (int i = 0; i < elementCount; ++i) {
        forge::ForgeElementConfig childCfg{};
        childCfg.layout.width = forge::ForgeDimension::Px(100.0f);
        childCfg.layout.height = forge::ForgeDimension::Px(20.0f);
        ctx.Element("Item", childCfg);
    }

    ctx.End();

    ctx.EndFrame();

    auto endLayout = std::chrono::high_resolution_clock::now();
    double layoutMs = std::chrono::duration<double, std::milli>(endLayout - startLayout).count();

    printf("\n--- Layout Performance Benchmark ---\n");
    printf("Total Elements in DFS SoA : %zu\n", ctx.GetStorage().Count());
    printf("Single-Core Layout Time   : %.3f ms\n", layoutMs);
    printf("Throughput                : %.2f million elements/sec\n", (elementCount / (layoutMs * 1000.0)));

    auto startDraw = std::chrono::high_resolution_clock::now();

    forge::ForgeJobSystem jobSystem;
    forge::ForgeDrawList& drawList = ctx.GetDrawList();

    const forge::ForgeStorage& storage = ctx.GetStorage();
    size_t count = storage.Count();

    struct DrawJobData {
        const forge::ForgeStorage* storage;
        forge::ForgeDrawList* drawList;
    } jobData { &storage, &drawList };

    jobSystem.ParallelFor(static_cast<uint32_t>(count), 1024, [](void* userData, uint32_t sliceIdx, uint32_t start, uint32_t cnt) {
        (void)sliceIdx;
        DrawJobData* d = static_cast<DrawJobData*>(userData);
        for (uint32_t i = 0; i < cnt; ++i) {
            uint32_t idx = start + i;
            float x = d->storage->GetX(idx);
            float y = d->storage->GetY(idx);
            float w = d->storage->GetWidth(idx);
            float h = d->storage->GetHeight(idx);
            d->drawList->AddRect(forge::ForgeVec2(x, y), forge::ForgeVec2(x + w, y + h), 0xFF336699);
        }
    }, &jobData);

    auto endDraw = std::chrono::high_resolution_clock::now();
    double drawMs = std::chrono::duration<double, std::milli>(endDraw - startDraw).count();

    printf("\n--- Vertex Generation Benchmark ---\n");
    printf("Generated Vertices        : %u\n", drawList.GetVertexCount());
    printf("Generated Indices         : %u\n", drawList.GetIndexCount());
    printf("GPU Draw Commands         : %u\n", drawList.GetCommandCount());
    printf("Parallel Vertex Gen Time  : %.3f ms\n", drawMs);
    printf("====================================================\n");

    return 0;
}
