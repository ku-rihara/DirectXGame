#include "Particle.hlsli"
#include"../Library/Matrix4x4.hlsli"

struct PerView
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
    int billboardMode;             
    int alignToVelocity;           
    float2 padding;
};

StructuredBuffer<Particle> gParticle : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    Particle particle = gParticle[instanceID];
    
    // スケール行列
    float4x4 scaleMatrix = MakeScaleMatrix(particle.scale);
    
    // 回転行列
    float4x4 rotationMatrix = MakeRotationMatrix(particle.rotate);
    
    // ビルボードモードに応じた処理
    float4x4 billboardMatrix = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
    
    if (gPerView.billboardMode == 1) // Billboard
    {
        billboardMatrix = gPerView.billboardMatrix;
    }
    else if (gPerView.billboardMode == 2) // YAxis Billboard
    {
        // Y軸ビルボード（Y軸回転のみ）
        float3 forward = float3(gPerView.billboardMatrix[2][0], 0, gPerView.billboardMatrix[2][2]);
        forward = normalize(forward);
        float3 right = cross(float3(0, 1, 0), forward);
        
        billboardMatrix[0].xyz = right;
        billboardMatrix[1].xyz = float3(0, 1, 0);
        billboardMatrix[2].xyz = forward;
    }
    
    // 進行方向に向く場合は、Z軸回転のみ適用
    float4x4 finalRotation = rotationMatrix;
    if (gPerView.alignToVelocity != 0)
    {
        // Z軸回転のみ使用
        float cosZ = cos(particle.rotate.z);
        float sinZ = sin(particle.rotate.z);
        finalRotation = float4x4(
            cosZ, -sinZ, 0, 0,
            sinZ, cosZ, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }
    
    // ワールド行列計算: Scale → Rotation → Billboard
    float4x4 worldMatrix = mul(scaleMatrix, finalRotation);
    worldMatrix = mul(worldMatrix, billboardMatrix);
    worldMatrix[3].xyz = particle.translate;
    
    output.position = mul(input.position, mul(worldMatrix, gPerView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;

    return output;
}
