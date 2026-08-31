#include "ForgeUI/Core/ForgeJobSystem.h"
#include "ForgeUI/Core/ForgeContext.h"
#include <string>
#include <functional>
#include <atomic>
#include <thread>

extern void RegisterTest(const std::string& name, std::function<bool()> func);
#define FORGE_TEST(name) \
    bool Test_##name(); \
    struct Register_##name { \
        Register_##name() { RegisterTest(#name, Test_##name); } \
    } g_register_##name; \
    bool Test_##name()

FORGE_TEST(JobSystem_ParallelForAndPrefixSum) {
    forge::ForgeJobSystem jobSystem;

    const uint32_t count = 1000;
    uint32_t results[count] = {0};

    struct UserData {
        uint32_t* arr;
    } data { results };

    jobSystem.ParallelFor(count, 64, [](void* userData, uint32_t sliceIdx, uint32_t start, uint32_t cnt) {
        (void)sliceIdx;
        UserData* u = static_cast<UserData*>(userData);
        for (uint32_t i = 0; i < cnt; ++i) {
            u->arr[start + i] = (start + i) * 2;
        }
    }, &data);

    for (uint32_t i = 0; i < count; ++i) {
        if (results[i] != i * 2) return false;
    }

    uint32_t counts[4] = {10, 20, 30, 40};
    uint32_t offsets[4] = {0};
    forge::ForgeJobSystem::ComputeParallelPrefixSum(counts, offsets, 4);

    if (offsets[0] != 0) return false;
    if (offsets[1] != 10) return false;
    if (offsets[2] != 30) return false;
    if (offsets[3] != 60) return false;

    return true;
}

FORGE_TEST(JobSystem_CustomThreadIsolation) {
    std::atomic<bool> workerFinished{false};
    std::atomic<bool> workerPassed{false};

    std::thread worker([&]() {
        forge::ForgeContext threadLocalCtx;
        forge::SetCurrentContext(&threadLocalCtx);

        threadLocalCtx.BeginFrame();

        forge::ForgeElementConfig cfg{};
        cfg.layout.width = forge::ForgeDimension::Px(200.0f);
        cfg.layout.height = forge::ForgeDimension::Px(100.0f);

        uint32_t el = threadLocalCtx.Element("ThreadWidget", cfg);
        (void)el;

        threadLocalCtx.EndFrame();

        const forge::ForgeStorage& storage = threadLocalCtx.GetStorage();
        if (storage.Count() == 1 && storage.GetWidth(0) == 200.0f) {
            workerPassed = true;
        }

        workerFinished = true;
    });

    worker.join();

    if (!workerFinished.load()) return false;
    if (!workerPassed.load()) return false;

    return true;
}

void RegisterJobSystemTests() {
    RegisterTest("JobSystem_ParallelForAndPrefixSum", Test_JobSystem_ParallelForAndPrefixSum);
    RegisterTest("JobSystem_CustomThreadIsolation", Test_JobSystem_CustomThreadIsolation);
}
