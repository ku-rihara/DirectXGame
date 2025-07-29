#include"object3d.hlsli"
#include"ShadowMap/ShadowMap.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ShadowTransformBuffer> gShadowTransformBuffer : register(b1);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.WorldInverseTranspose));
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
    output.texcoord = input.texcoord;
    
    // 影用の座標計算を修正
    float4 worldPos = mul(input.position, gTransformationMatrix.World);
    output.tpos = mul(worldPos, gShadowTransformBuffer.lightCamera);
    
    return output;
}