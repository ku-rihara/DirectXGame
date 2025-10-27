#include "Particle.hlsli"
#include"../Library/Matrix4x4.hlsli"

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
    
    // スケール適応
    float4x4 scaleMatrix = MakeScaleMatrix(particle.scale);
 
    // 回転適用
    float4x4 rotationMatrix = MakeRotationMatrix(particle.rotate);
    
    // ビルボード行列と組み合わせ
    float4x4 worldMatrix = mul(mul(scaleMatrix, rotationMatrix), gPerView.billboardMatrix);
    worldMatrix[3].xyz = particle.translate;
    
    output.position = mul(input.position, mul(worldMatrix, gPerView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;

    return output;
}