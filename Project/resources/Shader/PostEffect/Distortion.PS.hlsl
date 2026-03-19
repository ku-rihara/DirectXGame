#include "Fullscreen.hlsli"

// t0: 通常シーンのレンダーターゲット
Texture2D<float4> gSceneTexture      : register(t0);
// t1: 歪みパスで書き込まれたUVオフセットテクスチャ（R16G16F）
Texture2D<float2> gDistortionTexture : register(t1);

SamplerState gSampler : register(s0);

float4 main(VertexShaderOutput input) : SV_TARGET {
    // 歪みテクスチャからUVオフセットを取得
    float2 offset = gDistortionTexture.Sample(gSampler, input.texcoord);

    // オフセットを加算してシーンをサンプリング
    float2 warpedUV = input.texcoord + offset;

    return gSceneTexture.Sample(gSampler, warpedUV);
}
