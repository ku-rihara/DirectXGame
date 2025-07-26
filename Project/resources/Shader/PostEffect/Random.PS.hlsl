#include"Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
    
};

struct RandomParam
{
    float time;
};

ConstantBuffer<RandomParam> gRandomParam : register(b0);

//乱数
float rand2dTo1d(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;
    float random = rand2dTo1d(input.texcoord * gRandomParam.time);
    float4 baseColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = baseColor*float4(random, random, random, 1.0f);
    return output;
}