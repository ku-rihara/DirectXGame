#include "RibbonTrailDistortion.hlsli"

// 歪みノイズテクスチャ（法線マップなどを使用）
Texture2D<float4> gDistortionMap : register(t0);
SamplerState      gSampler       : register(s0);

struct DistortionParam {
    float strength; // 歪み強度（例: 0.02f）
    float3 padding;
};

ConstantBuffer<DistortionParam> gParam : register(b1);

// 出力はR16G16F形式の歪みRT（UVオフセットXY）へ書き込む
// alpha を出力してブレンドステート(SRC_ALPHA + ONE)でトレイル末端を自然にフェード
float4 main(VSOutput input) : SV_TARGET {
    // ノイズマップから-1〜1のオフセットを取得
    float2 noise = gDistortionMap.Sample(gSampler, input.uv).rg * 2.0f - 1.0f;

    // RG = UVオフセット（ブレンドでalphaが掛けられる）、A = トレイルの不透明度
    return float4(noise * gParam.strength, 0.0f, input.color.a);
}
