#include "Particle.hlsli"

struct ParticleForGPU
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
    float4x4 UVTransform;
    float4 color;
    uint isFlipX;
    uint isFlipY;
    float dissolveThreshold;
};
StructuredBuffer<ParticleForGPU> gParticle : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    
    float2 texcoord = input.texcoord;

    // Flip 処理
    if (gParticle[instanceID].isFlipX != 0)
    {
        texcoord.x = 1.0f - texcoord.x;
    }
    if (gParticle[instanceID].isFlipY != 0)
    {
        texcoord.y = 1.0f - texcoord.y;
    }

    // 頂点座標の変換
    output.position = mul(input.position, gParticle[instanceID].WVP);

    // UVTransform を適用
    float4 transformedUV = mul(float4(texcoord, 0.0f, 1.0f), gParticle[instanceID].UVTransform);
    output.texcoord = transformedUV.xy;

    // 色と法線の設定
    output.color = gParticle[instanceID].color;
    output.normal = normalize(mul(input.normal, (float3x3) gParticle[instanceID].WorldInverseTranspose));

    // パーティクルごとの dissolve 閾値を渡す
    output.dissolveThreshold = gParticle[instanceID].dissolveThreshold;

    return output;
}