#include "SkyBox.hlsli"

TextureCube<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
};
//マテリアル
ConstantBuffer<Material> gMaterial : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = textureColor * gMaterial.color;
    return output;
}