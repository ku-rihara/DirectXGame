#include"ShadowMap.hlsli"

// シャドウマップ生成用の入力構造体
struct ShadowVertexInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

// シャドウマップ生成用の出力構造体
struct ShadowVertexOutput
{
    float4 position : SV_POSITION;
};

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ShadowTransformBuffer> gShadowTransformBuffer : register(b1);

ShadowVertexOutput main(ShadowVertexInput input)
{
    ShadowVertexOutput output;
    
    // ワールド座標に変換
    float4 worldPos = mul(input.position, gTransformationMatrix.World);
    
    // ライトカメラ座標系に変換
    output.position = mul(worldPos, gShadowTransformBuffer.lightCamera);
  
    return output;
}