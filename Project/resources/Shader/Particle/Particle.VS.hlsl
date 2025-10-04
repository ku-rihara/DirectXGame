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
};

struct PerView
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};

StructuredBuffer<ParticleForGPU> gParticle : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

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
    Particle particle = gParticle[instanceID];
    float4x4 worldMatrix = gPerView.billboardMatrix;
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3] = particle.scale.xyz;
    output.position = mul(input.position, mul(worldMatrix, gPerView.viewProjection));

    // UVTransform を適用
    float4 transformedUV = mul(float4(texcoord, 0.0f, 1.0f), gParticle[instanceID].UVTransform);
    output.texcoord = transformedUV.xy;

    // 色と法線の設定
    output.color = gParticle[instanceID].color;
    output.normal = normalize(mul(input.normal, (float3x3) gParticle[instanceID].WorldInverseTranspose));

    return output;
}