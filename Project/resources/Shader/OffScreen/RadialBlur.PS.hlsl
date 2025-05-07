#include"Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
    
};

PixelShaderOutPut main(VertexShaderOutput input)
{
     
    const float2 kCenter = float2(0.5f, 0.5f);
    const int kNumSamples = 10;
    const float kBlurWidth = 0.01f;
    
    //中心から現在のUVに対しての方向を計算
    float2 direction = input.texcoord - kCenter;
    float3 outputColor = float3(0.0f, 0.0f, 0.0f);
    for (int sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
    {
        //現在のUVから先程計算した方向にサンプリング点を進めながらサンプリングしていく
        float2 texcoord = input.texcoord + direction * kBlurWidth * float(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
    }
    //平均化
    outputColor.rgb *= rcp(float(kNumSamples));
    
    PixelShaderOutPut output;
    
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
    
  

}