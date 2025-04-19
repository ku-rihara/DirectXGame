#include "Particle.hlsli"

struct ParticleForGPU
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
    float4x4 UVTransform; 
    float4 color;
};

StructuredBuffer<ParticleForGPU> gParticle : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;

    // 頂点座標の変換
    output.position = mul(input.position, gParticle[instanceID].WVP);

    // UVTransform を適用
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gParticle[instanceID].UVTransform);
    output.texcoord = transformedUV.xy;

    // 色と法線の設定
    output.color = gParticle[instanceID].color;
    output.normal = normalize(mul(input.normal, (float3x3) gParticle[instanceID].WorldInverseTranspose));

    return output;
}