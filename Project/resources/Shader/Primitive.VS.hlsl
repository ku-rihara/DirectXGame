struct VertexShaderInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal   : NORMAL0;
    float4 color    : COLOR0;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color    : COLOR0;
};

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    output.color    = input.color;
    return output;
}
