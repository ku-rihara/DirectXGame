#include "Particle.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);


struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};

struct UVAnimation
{
    float2 uvPosition;
    float uvRotate;
    float2 uvScrollSpeed;
    float2 uvScale;
    int isFlipX;
    int isFlipY;
    float pixelStep;
    int isUVLoop;
    float currentTime;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<UVAnimation> gUVAnim : register(b1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

float2 RotateUV(float2 uv, float angle, float2 center)
{
    float2 shifted = uv - center;
    float cosA = cos(angle);
    float sinA = sin(angle);
    float2 rotated = float2(
        shifted.x * cosA - shifted.y * sinA,
        shifted.x * sinA + shifted.y * cosA
    );
    return rotated + center;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 基本UV
    float2 uv = input.texcoord;
    
    // UVスクロール
    float2 scrollOffset = gUVAnim.uvScrollSpeed * gUVAnim.currentTime;
    
    // ピクセルステップ処理
    if (gUVAnim.pixelStep > 0.0f)
    {
        scrollOffset = floor(scrollOffset / gUVAnim.pixelStep) * gUVAnim.pixelStep;
    }
    
    uv += scrollOffset;
    
    // UVループ
    if (gUVAnim.isUVLoop != 0)
    {
        uv = frac(uv);
    }
    
    // UV位置オフセット
    uv += gUVAnim.uvPosition;
    
    // UVスケール
    uv = (uv - 0.5f) / gUVAnim.uvScale + 0.5f;
    
    // UV回転
    if (abs(gUVAnim.uvRotate) > 0.001f)
    {
        uv = RotateUV(uv, gUVAnim.uvRotate, float2(0.5f, 0.5f));
    }
    
    // UV反転
    if (gUVAnim.isFlipX != 0)
    {
        uv.x = 1.0f - uv.x;
    }
    if (gUVAnim.isFlipY != 0)
    {
        uv.y = 1.0f - uv.y;
    }
    
    // テクスチャサンプリング
    float4 textureColor = gTexture.Sample(gSampler, uv);
    
    output.color = gMaterial.color * textureColor * input.color;
    
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    return output;
}