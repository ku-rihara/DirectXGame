#include "object3d.hlsli"

//float4 main() : SV_TARGET
//{
//    return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}

//struct PixelShaderOutput
//{
//    float4 color : SV_TARGET0;
//};

//PixelShaderOutput main()
//{
//    PixelShaderOutput output;
//    output.color = float4(1.0, 1.0, 1.0, 1.0);
//    return output;
//}

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);
struct DirectionalLight
{
    float4 color; //ライトの色
    float3 direction; //ライトの向き
    float intensity; //輝度
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;    
};


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    float NdotL = 0.0f;
    float cos = 0.0f;
   
    if (gMaterial.enableLighting != 0)
    {
        if (gMaterial.enableLighting == 1)
        {
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        }
        else if (gMaterial.enableLighting == 2)
        {
            NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        
        output.color.xyz = gMaterial.color.xyz * textureColor.xyz * gDirectionalLight.color.xyz * cos * gDirectionalLight.intensity;
        output.color.w = gMaterial.color.w * textureColor.w;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    
     //textureのα値が0の時にPixelを破棄
    if (textureColor.a == 0.0)
    {
        discard;
    }
    //textureのα値が0.5以下の時にPixelを棄却
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    
    //textureのα値が0.5以下の時にPixelを棄却
    if (output.color.a ==0.0)
    {
        discard;
    }
    
    return output;
}