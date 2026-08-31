#include "ForgeUI/Core/ForgeJobSystem.h"
#include <thread>
#include <vector>
#include <algorithm>

namespace forge {

ForgeJobSystem::ForgeJobSystem() noexcept {}

ForgeJobSystem::~ForgeJobSystem() noexcept {}

uint32_t ForgeJobSystem::GetWorkerThreadCount() const {
    uint32_t threads = std::thread::hardware_concurrency();
    return (threads > 0) ? threads : 1;
}

void ForgeJobSystem::ParallelFor(uint32_t totalCount, uint32_t minBatchSize, ForgeJobTaskFn taskFn, void* userData) {
    if (totalCount == 0 || !taskFn) return;

    uint32_t maxThreads = GetWorkerThreadCount();
    uint32_t batchSize = (minBatchSize > 0) ? minBatchSize : 64;
    uint32_t threadCount = (totalCount + batchSize - 1) / batchSize;
    if (threadCount > maxThreads) threadCount = maxThreads;
    if (threadCount == 0) threadCount = 1;

    if (threadCount == 1) {
        taskFn(userData, 0, 0, totalCount);
        return;
    }

    uint32_t itemsPerThread = totalCount / threadCount;
    uint32_t remainder = totalCount % threadCount;

    std::vector<std::thread> workers;
    workers.reserve(threadCount);

    uint32_t currentStart = 0;
    for (uint32_t i = 0; i < threadCount; ++i) {
        uint32_t count = itemsPerThread + (i < remainder ? 1 : 0);
        uint32_t start = currentStart;
        currentStart += count;

        workers.emplace_back([taskFn, userData, i, start, count]() {
            taskFn(userData, i, start, count);
        });
    }

    for (auto& t : workers) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void ForgeJobSystem::ComputeParallelPrefixSum(const uint32_t* inputCounts, uint32_t* outOffsets, uint32_t count) noexcept {
    if (!inputCounts || !outOffsets || count == 0) return;

    uint32_t runningSum = 0;
    for (uint32_t i = 0; i < count; ++i) {
        outOffsets[i] = runningSum;
        runningSum += inputCounts[i];
    }
}

}
