#include"Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct UVStepSize
{
    float2 uvStepSize;
};

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
    
};

ConstantBuffer<UVStepSize> gUVStepSize : register(b0);

static const float kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};

static const float kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

float Luminance(float3 v)
{
    return dot(v, float3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutPut main(VertexShaderOutput input)
{
    float2 difference = float2(0.0f, 0.0f);
    
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            float2 texcoord = input.texcoord + kIndex3x3[x][y] * gUVStepSize.uvStepSize;
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            float luminance = Luminance(fetchColor);
            difference += luminance * kPrewittHorizontalKernel[x][y];
            difference += luminance * kPrewittVerticalKernel[x][y];
        }

    }

    float weight = length(difference);
    weight = saturate(weight);
    
    PixelShaderOutPut output;
    output.color.rgb = weight;
    output.color.a = 1.0f;
    
    return output;
}