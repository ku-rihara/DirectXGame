#include "RibbonTrail.hlsli"

struct ViewProjection {
    matrix viewProjection;
};

ConstantBuffer<ViewProjection> gViewProjection : register(b0);

VSOutput main(VSInput input) {
    VSOutput output;
    output.pos   = mul(float4(input.pos, 1.0f), gViewProjection.viewProjection);
    output.color = input.color;
    output.uv    = input.uv;
    return output;
}
