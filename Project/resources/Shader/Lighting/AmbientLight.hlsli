
struct AmbientLight
{
    float4 color;
    float intensity;
};

// 環境ライトの計算
float3 CalculateAmbientLight(AmbientLight light, float3 materialColor, float3 textureColor)
{
    return materialColor * textureColor * light.color.rgb * light.intensity;
}

// 環境ライトの色のみ取得
float3 GetAmbientLightColor(AmbientLight light)
{
    return light.color.rgb * light.intensity;
}

// 環境ライトをマテリアルに適用
float3 ApplyAmbientLight(AmbientLight light, float3 baseColor)
{
    return baseColor * light.color.rgb * light.intensity;
}
