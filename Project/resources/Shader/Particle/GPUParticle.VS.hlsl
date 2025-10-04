#include "Particle.hlsli"


struct PerView
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};

StructuredBuffer<Particle> gParticle : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    Particle particle = gParticle[instanceID];
 
    // 頂点座標の変換
    float4x4 worldMatrix = gPerView.billboardMatrix;
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    output.position = mul(input.position, mul(worldMatrix, gPerView.viewProjection));
    output.texcoord = input.texcoord;
    // 色の決定
    output.color = particle.color;

    return output;
}