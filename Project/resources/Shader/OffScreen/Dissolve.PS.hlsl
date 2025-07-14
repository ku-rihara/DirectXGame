#include"Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
Texture2D<float> gMaskTexture : register(t1);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
    
};

PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;
  
    float mask = gMaskTexture.Sample(gSampler, input.texcoord);
    //maskの値が0.5以下の場合はdiscard
    if (mask <= 0.5f)
    {
        discard;
    }
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    return output;
}