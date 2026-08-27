#pragma once

#include "ForgeDrawList.h"

namespace forge {

struct ForgeShaderSource {
    const char* vertexSource;
    const char* fragmentSource;
};

class ForgePipelines {
public:
    static ForgeShaderSource GetShaderSource(ForgePipelineType pipeline);
};

}
