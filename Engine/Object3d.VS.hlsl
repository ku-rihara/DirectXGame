#include"object3d.hlsli"
//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//    return pos;
//}


struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);


VertexShaderOutput main(VertexShaderInput input){
VertexShaderOutput output;
    output.normal = normalize(mul(input.normal, (float3x3)gTransformationMatrix.World));
    output.position = mul(input.position,gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    return output;
}