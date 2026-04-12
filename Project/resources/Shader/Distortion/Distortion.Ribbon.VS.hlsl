#include "Distortion.hlsli"

struct VSInput {
    float3 pos   : POSITION;
    float4 color : COLOR0;
    float2 uv    : TEXCOORD0;
};

struct ViewProjection {
    matrix viewProjection;
};

struct UVScroll {
    float2 offset;
    float2 pad; // 16byte align
};

ConstantBuffer<ViewProjection> gViewProjection : register(b0);
ConstantBuffer<UVScroll>       gUVScroll       : register(b1);

VSOutput main(VSInput input) {
    VSOutput output;
    output.position = mul(float4(input.pos, 1.0f), gViewProjection.viewProjection);
    output.color    = input.color;
    output.texcoord = input.uv + gUVScroll.offset;
    return output;
}
