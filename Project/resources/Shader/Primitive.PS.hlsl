struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color    : COLOR0;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct PrimitiveMaterial
{
    float4  color;
    int     useTexture;     // 1=テクスチャサンプリング有効
    float   angleMaskRatio; // 0〜1 角度マスク比率 (1.0=全体表示)
    float2  pad;
};

ConstantBuffer<PrimitiveMaterial> gMaterial : register(b0);
Texture2D<float4>                 gTexture  : register(t0);
SamplerState                      gSampler  : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // 角度マスク: UV.x が angleMaskRatio を超えた領域を非表示
    if (gMaterial.angleMaskRatio < 1.0f && input.texcoord.x > gMaterial.angleMaskRatio)
    {
        discard;
    }

    float4 textureColor = (gMaterial.useTexture != 0) ? gTexture.Sample(gSampler, input.texcoord) : float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.color = gMaterial.color * input.color * textureColor;

    if (output.color.a == 0.0f)
    {
        discard;
    }

    return output;
}
