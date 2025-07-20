#include "ShadowMap.hlsli"


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // SV_Depth に書き込む
    output.depth = input.position.z / input.position.w;

    return output;
}
