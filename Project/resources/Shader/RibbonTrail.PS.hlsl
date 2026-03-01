#include "RibbonTrail.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState      gSampler : register(s0);

float4 main(VSOutput input) : SV_TARGET {
    return gTexture.Sample(gSampler, input.uv) * input.color;
}
