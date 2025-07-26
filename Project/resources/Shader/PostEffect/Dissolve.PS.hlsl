#include"Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
Texture2D<float> gMaskTexture : register(t1);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

struct ParamData
{
    float thresholdValue;
    float3 color;
};

ConstantBuffer<ParamData> gParamData:register(b0);

PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;
  
    float mask = gMaskTexture.Sample(gSampler, input.texcoord);
    //maskの値が0.5以下の場合はdiscard
    if (mask <= gParamData.thresholdValue)
    {
        discard;
    }
    
    float edge = 1.0f - smoothstep(gParamData.thresholdValue, gParamData.thresholdValue+0.03f, mask);
    output.color = gTexture.Sample(gSampler, input.texcoord);
    output.color.rgb += edge * gParamData.color;
    
    return output;
}