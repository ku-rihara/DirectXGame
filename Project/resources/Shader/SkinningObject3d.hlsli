

struct VertexShaderInput
{
    float4 position : POSITIONT0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMALT0;
    float4 weight : WEIGHTT0;
    int4 index : INDEXT0;
};


struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
};

