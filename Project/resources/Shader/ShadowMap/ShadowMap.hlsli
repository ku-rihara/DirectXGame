
struct ShadowTransformBuffer
{
    float4x4 lightCamera;
};

float CalculateShadow(float4 lightSpacePosition, Texture2D<float> ShadowMap, SamplerComparisonState shadowSampler)
{
    // NDC座標に変換
    float3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5; // [0,1]範囲に変換
    projCoords.y = 1.0 - projCoords.y; // Y座標を反転
    
    float currentDepth = projCoords.z;
    
    // シャドウバイアスを適用
    float bias = 0.005;
    float depthWithBias = currentDepth - bias;
    
    // SampleCmpを使用して深度比較を行う
    // SampleCmpは比較結果を返す
    float shadowFactor = ShadowMap.SampleCmp(shadowSampler, projCoords.xy, depthWithBias);
    
    return lerp(0.3, 1.0, shadowFactor);
}
