#include"Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
static const float PI = 3.14159265f;
struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
    
};

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;

    output.color = gTexture.Sample(gSampler, input.texcoord);
   
    //kenrnelを求める。weightは後で使う
    float weight = 0.0f;
    float kernel3x3[3][3];
    float2 kIndex3x3[3][3] =
    {
        { float2(-1, -1), float2(-1, 0), float2(-1, 1) },
        { float2(0, -1), float2(0, 0), float2(0, 1) },
        { float2(1, -1), float2(1, 0), float2(1, 1) }
    };
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            kernel3x3[x][y] = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, 2.0f);
            weight += kernel3x3[x][y];

        }
    }
   
    output.color.rgb *= rcp(weight);
    
    return output;
}

