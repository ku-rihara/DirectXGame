#include"Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
static const float PI = 3.14159265f;
struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
    
};

struct ParamData
{
    float sigma;
};

ConstantBuffer<ParamData> gParamData : register(b0);

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}


PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;

    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    
    uint width, height;
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), (rcp(float(height))));
   
    //kenrnelを求める。weightは後で使う
    float weight = 0.0f;
    float kernel3x3[3][3];
   
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            kernel3x3[x][y] = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, gParamData.sigma);
            weight += kernel3x3[x][y];

        }
    }
    
    for (int row = 0; row < 3; ++row)
    {
        for (int column = 0; column < 3; ++column)
        {
            //現在のtexcoordを算出
            float2 texcoord = input.texcoord + kIndex3x3[row][column] * uvStepSize;
            //色に1/9を掛けてたす
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kernel3x3[row][column];
        }
    }
   
    output.color.rgb *= rcp(weight);
    
    return output;
}

