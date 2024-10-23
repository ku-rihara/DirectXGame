#include "Sprite.hlsli"

struct Sprite
{
    float4x4 WVP;
    float4x4 World;
};

ConstantBuffer<Sprite> gSprite : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.position = mul(input.position, gSprite.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gSprite.World));

    return output;
}
