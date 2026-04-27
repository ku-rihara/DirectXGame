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
    float   startAngle;      // 正規化角度 0-1 (0°-360°)
    float   endAngle;        // 正規化角度 0-1 (0°-360°)
    int     enableAngleClip;
    int     useTexture;      // 1=テクスチャサンプリング有効
};

ConstantBuffer<PrimitiveMaterial> gMaterial : register(b0);
Texture2D<float4>                 gTexture  : register(t0);
SamplerState                      gSampler  : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // 角度クリップ: texcoord.x が正規化角度 (0-1)
    if (gMaterial.enableAngleClip != 0)
    {
        float angle = input.texcoord.x;
        if (angle < gMaterial.startAngle || angle > gMaterial.endAngle)
        {
            discard;
        }
    }

    float4 textureColor = (gMaterial.useTexture != 0) ? gTexture.Sample(gSampler, input.texcoord) : float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.color = gMaterial.color * input.color * textureColor;

    if (output.color.a == 0.0f)
    {
        discard;
    }

    return output;
}
