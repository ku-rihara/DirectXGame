#include"object3d.hlsli"


struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);



VertexShaderOutput main(VertexShaderInput input){
VertexShaderOutput output;
    output.normal = normalize(mul(input.normal, (float3x3)gTransformationMatrix.WorldInverseTranspose));
    output.position = mul(input.position,gTransformationMatrix.WVP);
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
    output.texcoord = input.texcoord;
    return output;
}