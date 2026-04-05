#include "ParticleDistortion.hlsli"

// 歪みノイズテクスチャ（法線マップなど）
Texture2D<float4> gDistortionMap : register(t0);
SamplerState      gSampler       : register(s0);

cbuffer DistortionParam : register(b0) {
    float strength; // 歪み強度（例: 0.05f）
    float3 padding;
};

// 出力はR16G16F形式の歪みRT（UVオフセットXY）へ書き込む
// alphaをブレンドステート(SRC_ALPHA + ONE)でパーティクル末端を自然にフェード
float4 main(VertexShaderOutput input) : SV_TARGET {
    // ノイズマップから-1〜1のオフセットを取得
    float2 noise = gDistortionMap.Sample(gSampler, input.texcoord).rg * 2.0f - 1.0f;

    // RG = UVオフセット（ブレンドでalphaが掛けられる）、A = パーティクルの不透明度
    return float4(noise * strength, 0.0f, input.color.a);
}
