struct VertexShaderInput
{
    float3 position : POSITION;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
};

struct PixelShaderOutput
{
    float depth : SV_Depth;
};

struct LightMatrix
{
    float4x4 lightViewProjection;
};
