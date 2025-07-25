#include"Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
    
};

PixelShaderOutPut main(VertexShaderOutput input)
{
    uint width, height;
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), (rcp(float(height))));
    
    PixelShaderOutPut output;
    
    output.color.rbg = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            //現在のtexcoordを算出
            float2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            //色に1/9を掛けてたす
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kKernel3x3[x][y];
        }
    }
    return output;
}