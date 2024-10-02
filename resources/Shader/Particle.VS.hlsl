#include"Particle.hlsli"
//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//    return pos;
//}

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4  WorldInverseTranspose;
};

StructuredBuffer<TransformationMatrix> gTransformationMatrices : register(t0);

VertexShaderOutput main(VertexShaderInput input,uint instanceID:SV_InstanceID)
{
   
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrices[instanceID].WVP);
    //output.wo = mul(input.position, gTransformationMatrices[instanceID].World).xyz;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrices[instanceID].WorldInverseTranspose));
    
    return output;
}