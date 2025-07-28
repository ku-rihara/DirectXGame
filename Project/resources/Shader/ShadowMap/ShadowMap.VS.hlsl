#include"ShadowMap.hlsli"

// シャドウマップ生成用の入力構造体
struct ShadowVertexInput
{
    float4 position : POSITION0;
};

// シャドウマップ生成用の出力構造体
struct ShadowVertexOutput
{
    float4 position : SV_POSITION;
};

// シャドウマップ生成用の変換行列
struct ShadowTransformMatrix
{
    float4x4 World;
};

ConstantBuffer<ShadowTransformMatrix> gShadowTransformMatrix : register(b0);
ConstantBuffer<ShadowTransformBuffer> gShadowTransformBuffer : register(b1);

ShadowVertexOutput main(ShadowVertexInput input)
{
    ShadowVertexOutput output;
    
      // ワールド座標に変換
    float4 worldPos = mul(gShadowTransformMatrix.World, input.position);
    
    // ライトカメラ座標系に変換
    output.position = mul(gShadowTransformBuffer.lightCamera, worldPos);
    
    return output;
}