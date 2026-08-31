#pragma once

#include "ForgeTypes.h"
#include <cstdint>
#include <cstddef>

namespace forge {

typedef void (*ForgeJobTaskFn)(void* userData, uint32_t sliceIndex, uint32_t startIndex, uint32_t count);

class IForgeJobDispatcher {
public:
    virtual ~IForgeJobDispatcher() {}
    virtual void ParallelFor(uint32_t totalCount, uint32_t minBatchSize, ForgeJobTaskFn taskFn, void* userData) = 0;
    virtual uint32_t GetWorkerThreadCount() const = 0;
};

class ForgeJobSystem : public IForgeJobDispatcher {
public:
    ForgeJobSystem() noexcept;
    virtual ~ForgeJobSystem() noexcept;

    virtual void ParallelFor(uint32_t totalCount, uint32_t minBatchSize, ForgeJobTaskFn taskFn, void* userData) override;
    virtual uint32_t GetWorkerThreadCount() const override;

    static void ComputeParallelPrefixSum(const uint32_t* inputCounts, uint32_t* outOffsets, uint32_t count) noexcept;
};

}
