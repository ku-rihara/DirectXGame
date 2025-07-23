#include"object3d.hlsli"
#include"ShadowMap/ShadowMap.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ShadowVertexBuffer> gShadowVertexBuffer : register(b1);


VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.WorldInverseTranspose));
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
    output.texcoord = input.texcoord;
    output.instanceID = instanceID; 
    
    if (output.instanceID == 1)
    {
        
    }
    
    return output;
}