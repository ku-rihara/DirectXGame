#include "Distortion.hlsli"

// 歪みノイズテクスチャ
Texture2D<float4> gDistortionMap : register(t0);
SamplerState      gSampler       : register(s0);

cbuffer DistortionParam : register(b0) {
    float  strength; // 歪み強度
    float3 padding;
};

float4 main(VSOutput input) : SV_TARGET {
    float2 noise = gDistortionMap.Sample(gSampler, input.texcoord).rg * 2.0f - 1.0f;
    return float4(noise * strength, 0.0f, input.color.a);
}
