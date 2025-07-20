#include"ShadowMap.hlsli"

struct WorldMatrix
{
    float4x4 World;
};

ConstantBuffer<WorldMatrix> gWorldMatrix : register(b0);
ConstantBuffer<LightMatrix> gLightMatrix : register(b1);


VertexShaderOutput main(VertexShaderInput input){
    VertexShaderOutput output;

    float4 worldPos = mul(float4(input.position, 1.0f), gWorldMatrix.World);

    // ライト視点のViewProjectionで変換
    output.position = mul(worldPos, gLightMatrix.lightViewProjection);

    return output;
}