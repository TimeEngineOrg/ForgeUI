#include "ForgeUI/Render/ForgePipelines.h"

namespace forge {

static const char* s_SDFVertexShader = R"(#version 330 core
layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in uint a_Color;
layout(location = 3) in vec4 a_ClipRect;
layout(location = 4) in vec4 a_Params;

uniform mat4 u_Proj;

out vec2 v_Pos;
out vec2 v_UV;
out vec4 v_Color;
out vec4 v_ClipRect;
out vec4 v_Params;

void main() {
    v_Pos = a_Pos;
    v_UV = a_UV;
    float r = float((a_Color >> 0u) & 0xFFu) / 255.0;
    float g = float((a_Color >> 8u) & 0xFFu) / 255.0;
    float b = float((a_Color >> 16u) & 0xFFu) / 255.0;
    float a = float((a_Color >> 24u) & 0xFFu) / 255.0;
    v_Color = vec4(r, g, b, a);
    v_ClipRect = a_ClipRect;
    v_Params = a_Params;
    gl_Position = u_Proj * vec4(a_Pos, 0.0, 1.0);
}
)";

static const char* s_SDFFragmentShader = R"(#version 330 core
in vec2 v_Pos;
in vec2 v_UV;
in vec4 v_Color;
in vec4 v_ClipRect;
in vec4 v_Params;

out vec4 o_Color;

float RoundedBoxSDF(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + vec2(r);
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

void main() {
    vec2 clipMin = v_ClipRect.xy;
    vec2 clipMax = v_ClipRect.zw;
    vec2 insideMin = step(clipMin, v_Pos);
    vec2 insideMax = step(v_Pos, clipMax);
    float clipMask = insideMin.x * insideMin.y * insideMax.x * insideMax.y;

    float shapeType = v_Params.w;
    float cornerRadius = v_Params.x;
    float borderWidth = v_Params.y;
    float shadowBlur = v_Params.z;

    vec2 p = v_UV - vec2(0.5);
    float alpha = 1.0;

    if (shapeType < 0.5) {
        alpha = 1.0;
    } else if (shapeType > 0.5 && shapeType < 1.5) {
        if (cornerRadius <= 0.5) {
            alpha = 1.0;
        } else {
            vec2 halfSize = abs(dFdx(v_Pos)) + abs(dFdy(v_Pos));
            vec2 quadSize = 1.0 / max(vec2(0.0001), vec2(dFdx(v_UV.x), dFdy(v_UV.y)));
            float rNorm = clamp(cornerRadius / max(quadSize.x, quadSize.y), 0.0, 0.5);
            vec2 size = vec2(0.5);
            float d = RoundedBoxSDF(p, size, rNorm);
            alpha = 1.0 - smoothstep(-0.01, 0.01, d);
        }
    } else if (shapeType > 1.5 && shapeType < 2.5) {
        float d = length(p) - 0.5;
        alpha = 1.0 - smoothstep(-0.01, 0.01, d);
    } else if (shapeType > 2.5 && shapeType < 3.5) {
        vec2 size = vec2(0.5);
        float rNorm = clamp(cornerRadius * 0.01, 0.0, 0.5);
        float d = abs(RoundedBoxSDF(p, size, rNorm)) - max(0.005, borderWidth * 0.005);
        alpha = 1.0 - smoothstep(0.0, 0.02, d);
    } else if (shapeType > 3.5 && shapeType < 4.5) {
        float d = length(p);
        alpha = exp(-d * d * (10.0 / max(shadowBlur, 0.01)));
    }

    o_Color = v_Color * alpha * clipMask;
}
)";

static const char* s_MSDFVertexShader = R"(#version 330 core
layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in uint a_Color;
layout(location = 3) in vec4 a_ClipRect;
layout(location = 4) in vec4 a_Params;

uniform mat4 u_Proj;

out vec2 v_Pos;
out vec2 v_UV;
out vec4 v_Color;
out vec4 v_ClipRect;

void main() {
    v_Pos = a_Pos;
    v_UV = a_UV;
    float r = float((a_Color >> 0u) & 0xFFu) / 255.0;
    float g = float((a_Color >> 8u) & 0xFFu) / 255.0;
    float b = float((a_Color >> 16u) & 0xFFu) / 255.0;
    float a = float((a_Color >> 24u) & 0xFFu) / 255.0;
    v_Color = vec4(r, g, b, a);
    v_ClipRect = a_ClipRect;
    gl_Position = u_Proj * vec4(a_Pos, 0.0, 1.0);
}
)";

static const char* s_MSDFFragmentShader = R"(#version 330 core
in vec2 v_Pos;
in vec2 v_UV;
in vec4 v_Color;
in vec4 v_ClipRect;

uniform sampler2D u_Texture;

out vec4 o_Color;

void main() {
    vec2 clipMin = v_ClipRect.xy;
    vec2 clipMax = v_ClipRect.zw;
    vec2 insideMin = step(clipMin, v_Pos);
    vec2 insideMax = step(v_Pos, clipMax);
    float clipMask = insideMin.x * insideMin.y * insideMax.x * insideMax.y;

    vec4 texColor = texture(u_Texture, v_UV);
    float alpha = texColor.a;
    if (alpha <= 0.001) {
        discard;
    }

    o_Color = vec4(v_Color.rgb, v_Color.a * alpha * clipMask);
}
)";

static const char* s_TexturedVertexShader = R"(#version 330 core
layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in uint a_Color;
layout(location = 3) in vec4 a_ClipRect;
layout(location = 4) in vec4 a_Params;

uniform mat4 u_Proj;

out vec2 v_Pos;
out vec2 v_UV;
out vec4 v_Color;
out vec4 v_ClipRect;

void main() {
    v_Pos = a_Pos;
    v_UV = a_UV;
    float r = float((a_Color >> 0u) & 0xFFu) / 255.0;
    float g = float((a_Color >> 8u) & 0xFFu) / 255.0;
    float b = float((a_Color >> 16u) & 0xFFu) / 255.0;
    float a = float((a_Color >> 24u) & 0xFFu) / 255.0;
    v_Color = vec4(r, g, b, a);
    v_ClipRect = a_ClipRect;
    gl_Position = u_Proj * vec4(a_Pos, 0.0, 1.0);
}
)";

static const char* s_TexturedFragmentShader = R"(#version 330 core
in vec2 v_Pos;
in vec2 v_UV;
in vec4 v_Color;
in vec4 v_ClipRect;

uniform sampler2D u_Texture;

out vec4 o_Color;

void main() {
    vec2 clipMin = v_ClipRect.xy;
    vec2 clipMax = v_ClipRect.zw;
    vec2 insideMin = step(clipMin, v_Pos);
    vec2 insideMax = step(v_Pos, clipMax);
    float clipMask = insideMin.x * insideMin.y * insideMax.x * insideMax.y;

    vec4 texColor = texture(u_Texture, v_UV);
    o_Color = texColor * v_Color * clipMask;
}
)";

ForgeShaderSource ForgePipelines::GetShaderSource(ForgePipelineType pipeline) {
    ForgeShaderSource src;
    switch (pipeline) {
        case ForgePipelineType::SDF_Shapes:
            src.vertexSource = s_SDFVertexShader;
            src.fragmentSource = s_SDFFragmentShader;
            break;
        case ForgePipelineType::MSDF_Text:
            src.vertexSource = s_MSDFVertexShader;
            src.fragmentSource = s_MSDFFragmentShader;
            break;
        case ForgePipelineType::Textured_Quad:
        default:
            src.vertexSource = s_TexturedVertexShader;
            src.fragmentSource = s_TexturedFragmentShader;
            break;
    }
    return src;
}

}
