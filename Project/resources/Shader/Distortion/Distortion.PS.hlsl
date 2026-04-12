#include "Distortion.hlsli"

// 歪みノイズテクスチャ
Texture2D<float4> gDistortionMap : register(t0);
SamplerState      gSampler       : register(s0);

cbuffer DistortionParam : register(b0) {
    float  strength; // 歪み強度（例: 0.05f）
    float3 padding;
};

// 出力は R16G16F 形式の歪みRT（UVオフセット XY）
// alpha をブレンドステート(SRC_ALPHA + ONE)でフェード
float4 main(VSOutput input) : SV_TARGET {
    float2 noise = gDistortionMap.Sample(gSampler, input.texcoord).rg * 2.0f - 1.0f;
    return float4(noise * strength, 0.0f, input.color.a);
}
