#pragma once

#include "ForgeUI/Core/ForgeTypes.h"
#include "ForgeUI/Core/ForgeStorage.h"

namespace forge {

class ForgeLayoutEngine {
public:
    static void ComputeLayout(ForgeStorage& storage, float rootWidth, float rootHeight) noexcept;

private:
    static void MeasurePass(ForgeStorage& storage, float rootWidth, float rootHeight) noexcept;
    static void ArrangePass(ForgeStorage& storage, float rootWidth, float rootHeight) noexcept;
};

}
