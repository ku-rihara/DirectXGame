#include "Particle.hlsli"


Texture2D<float4> gTexture : register(t0);
Texture2D<float> gMaskTexture : register(t1);
SamplerState gSampler : register(s0);

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
    float environmentCoefficient;
    float dissolveThreshold;
    float dissolveEdgeWidth;
    float3 dissolveEdgeColor;
    int enableDissolve;
};

ConstantBuffer<Material> gMaterial : register(b0);


struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    output.color = gMaterial.color * textureColor * input.color;

    if (output.color.a == 0.0)
    {
        discard;
    }

    // Dissolve処理 (閾値はパーティクルごとに独立)
    if (gMaterial.enableDissolve != 0 && input.dissolveThreshold < 1.0f)
    {
        float mask = gMaskTexture.Sample(gSampler, transformedUV.xy);

        if (mask <= input.dissolveThreshold)
        {
            discard;
        }

        float edge = 1.0f - smoothstep(input.dissolveThreshold,
                                       input.dissolveThreshold + gMaterial.dissolveEdgeWidth,
                                       mask);
        output.color.rgb = lerp(output.color.rgb, gMaterial.dissolveEdgeColor, edge);
    }

    return output;
}
