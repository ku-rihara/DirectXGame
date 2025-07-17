#include "DirectionalLight.hlsli"
#include "PointLight.hlsli"
#include "SpotLight.hlsli"
#include "AreaLight.hlsli"
#include "AmbientLight.hlsli"

// 法線の正規化
float3 SafeNormalize(float3 v)
{
    float len = length(v);
    return len > 0.001f ? v / len : float3(0.0f, 0.0f, 1.0f);
}

// 反射ベクトルの計算
float3 CalculateReflectionVector(float3 incident, float3 normal)
{
    return reflect(incident, SafeNormalize(normal));
}

// 視線方向の計算
float3 CalculateViewDirection(float3 cameraPosition, float3 worldPosition)
{
   return SafeNormalize(cameraPosition - worldPosition);
}
 
// 複数ライトの組み合わせ計算
struct LightingResult
{
    float3 diffuse;
    float3 specular;
    float3 ambient;
};

// 全ライトの結果を合成
float3 CombineLightingResults(LightingResult result)
{
    return result.diffuse + result.specular + result.ambient;
}

float SafeLightIntensity(float intensity)
{
    return max(0.0f, intensity);
}

float3 SafeColor(float3 color)
{
    return max(float3(0.0f, 0.0f, 0.0f), color);
}
