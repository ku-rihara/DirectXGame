#include "RibbonTrail.hlsli"

struct ViewProjection {
    matrix viewProjection;
};

struct UVScroll {
    float2 offset;
    float2 pad;
};

ConstantBuffer<ViewProjection> gViewProjection : register(b0);
ConstantBuffer<UVScroll>       gUVScroll       : register(b1);

VSOutput main(VSInput input) {
    VSOutput output;
    output.pos   = mul(float4(input.pos, 1.0f), gViewProjection.viewProjection);
    output.color = input.color;
    output.uv    = input.uv + gUVScroll.offset;
    return output;
}
