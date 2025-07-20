
Texture2D<float> shadowMap : register(t4);
SamplerState shadowSampler : register(s4);

struct LightViewProjection
{
    float4x4 lightViewProjection;
};

float CalculateShadow(float4 worldPosition, LightViewProjection lightViewProjection)
{
    float4 lightClip = mul(worldPosition, lightViewProjection.lightViewProjection);
    lightClip.xyz /= lightClip.w;

    float2 shadowUV = lightClip.xy * 0.5f + 0.5f;
    float shadowDepth = lightClip.z * 0.5f + 0.5f;

    float sceneDepth = shadowMap.Sample(shadowSampler, shadowUV);
    
    if (shadowDepth > sceneDepth + 0.005f)
    {
        return 0.3f; // 影の中
    }
    return 1.0f; // 光の下
}