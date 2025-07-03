cbuffer ViewProjection : register(b0)
{
    matrix viewProj;
};

struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 svpos : SV_POSITION;
    float4 color : COLOR;
};

PSInput main(VSInput input)
{
    PSInput output;
    output.svpos = mul(float4(input.pos, 1.0f), viewProj);
    output.color = input.color;
    return output;
}
