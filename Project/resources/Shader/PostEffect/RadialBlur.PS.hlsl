#include"Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct BlurParam
{
    float2 center;
    float blurWidth;
};

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
    
};

ConstantBuffer<BlurParam> gBlurParam : register(b0);

PixelShaderOutPut main(VertexShaderOutput input)
{    
    const int kNumSamples = 10;
    
    //中心から現在のUVに対しての方向を計算
    float2 direction = input.texcoord - gBlurParam.center;
    float3 outputColor = float3(0.0f, 0.0f, 0.0f);
    for (int sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
    {
        //現在のUVから先程計算した方向にサンプリング点を進めながらサンプリングしていく
        float2 texcoord = input.texcoord + direction * gBlurParam.blurWidth * float(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
    }
    //平均化
    outputColor.rgb *= rcp(float(kNumSamples));
    
    PixelShaderOutPut output;
    
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
    
  

}