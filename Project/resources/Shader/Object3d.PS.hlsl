#include "object3d.hlsli"
#include "Lighting/Light.hlsli"
#include"ShadowMap/ShadowMap.hlsli"

SamplerState gSampler : register(s0);
SamplerComparisonState gShadowSampler : register(s1);

Texture2D<float4> gTexture : register(t0);
TextureCube<float4> gEnvironmentTexture : register(t1);
StructuredBuffer<PointLight> gPointLights : register(t2);
StructuredBuffer<SpotLight> gSpotLights : register(t3);
Texture2D<float> gShadowMap : register(t4);
Texture2D<float> gMaskTexture : register(t5);


struct Material
{
    float4 color;
    int enableLighting;

    float4x4 uvTransform;
    float shininess;
    float environmentCoefficient;
    float dissolveThreshold;
    float dissolveEdgeWidth;
    float3 dissolveEdgeColor;
    int enableDissolve;
    int enableDither;    
    float3 _ditherPad;   
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Camera
{
    float3 worldPosition;
};

struct LightCountData
{
    int pointLightCount;
    int spotLightCount;
};

struct PlayerOcclusion
{
    float2 screenPos;    
    float  depth;        
    float  screenRadius; 
    float  ditherAlpha;  
    int    enabled;
    float2 _pad;
};

// 各種ライトの構造体と定数バッファ
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<AreaLight> gAreaLight : register(b3);
ConstantBuffer<AmbientLight> gAmbientLight : register(b4);
ConstantBuffer<LightCountData> gLightCountData : register(b5);
ConstantBuffer<PlayerOcclusion> gPlayerOcclusion : register(b6);


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

        // UVConversion
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);

    // スクリーンスペース ディザ抜き処理
    //対象オブジェクトより手前にある場合のみ適用
    if (gMaterial.enableDither != 0 && gPlayerOcclusion.enabled != 0)
    {
        if (input.position.z < gPlayerOcclusion.depth)
        {
            float2 diff = input.position.xy - gPlayerOcclusion.screenPos;
            float  dist = length(diff);

            // 距離に応じて ditherAlpha を中心→外周でグラデーション
            float t = saturate(dist / gPlayerOcclusion.screenRadius);
            float effectiveAlpha = lerp(gPlayerOcclusion.ditherAlpha, 1.0f, t);

            // Bayer 4x4 オーダードディザリング行列
            static const float bayerMatrix[16] = {
                 0.0 / 16.0,  8.0 / 16.0,  2.0 / 16.0, 10.0 / 16.0,
                12.0 / 16.0,  4.0 / 16.0, 14.0 / 16.0,  6.0 / 16.0,
                 3.0 / 16.0, 11.0 / 16.0,  1.0 / 16.0,  9.0 / 16.0,
                15.0 / 16.0,  7.0 / 16.0, 13.0 / 16.0,  5.0 / 16.0
            };
            int px = (int)input.position.x & 3;
            int py = (int)input.position.y & 3;
            if (effectiveAlpha < bayerMatrix[py * 4 + px])
            {
                discard;
            }
        }
    }

        // テクスチャカラー
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    // Dissolve処理
    if (gMaterial.enableDissolve != 0)
    {
        float mask = gMaskTexture.Sample(gSampler, transformedUV.xy);
        
        // マスクの値がthreshold以下の場合はdiscard
        if (mask <= gMaterial.dissolveThreshold)
        {
            discard;
        }
        
        // エッジ効果の計算
        float edge = 1.0f - smoothstep(gMaterial.dissolveThreshold,
                                      gMaterial.dissolveThreshold + gMaterial.dissolveEdgeWidth,
                                      mask);
    }

        // カメラからピクセルへの方向ベクトル
    float3 toEye = CalculateViewDirection(gCamera.worldPosition, input.worldPosition);

        // 環境マッピング
    float3 reflectedVector = CalculateReflectionVector(toEye, input.normal);
    float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);

        // 基本色（テクスチャカラーベース）
    float3 baseColor = gMaterial.color.rgb * textureColor.rgb;

        // ライティング計算
    LightingResult lightingResult = (LightingResult) 0;
        
    if (gMaterial.enableLighting != 0)
    {
            // Lambert
        if (gMaterial.enableLighting == 1)
        {
            lightingResult.diffuse += CalculateDirectionalLightDiffuse(gDirectionalLight, input.normal, gMaterial.color.rgb, textureColor.rgb);
            lightingResult.specular += CalculateDirectionalLightSpecular(gDirectionalLight, input.normal, toEye, gMaterial.shininess);
            output.color.rgb = CombineLightingResults(lightingResult);
        }
            // halfLambert
        else if (gMaterial.enableLighting == 2)
        {
            lightingResult.diffuse += CalculateDirectionalLightDiffuse(gDirectionalLight, input.normal, gMaterial.color.rgb, textureColor.rgb);
            lightingResult.specular += CalculateDirectionalLightSpecular(gDirectionalLight, input.normal, toEye, gMaterial.shininess);
            output.color.rgb = CombineLightingResults(lightingResult);
        }
            // 鏡面反射
        else if (gMaterial.enableLighting == 3)
        {
            lightingResult.diffuse += CalculateDirectionalLightDiffuse(gDirectionalLight, input.normal, gMaterial.color.rgb, textureColor.rgb);
            lightingResult.specular += CalculateDirectionalLightSpecular(gDirectionalLight, input.normal, toEye, gMaterial.shininess);
            output.color.rgb = CombineLightingResults(lightingResult);
        }
            // ポイントライト
        else if (gMaterial.enableLighting == 4)
        {
            for (int i = 0; i < gLightCountData.pointLightCount; ++i)
            {
                lightingResult.diffuse += CalculatePointLightDiffuse(gPointLights[i], input.worldPosition, input.normal, gMaterial.color.rgb, textureColor.rgb);
                lightingResult.specular += CalculatePointLightSpecular(gPointLights[i], input.worldPosition, input.normal, toEye, gMaterial.shininess);
            }
            output.color.rgb = CombineLightingResults(lightingResult);
        }
            // スポットライト
        else if (gMaterial.enableLighting == 5)
        {
            for (int i = 0; i < gLightCountData.spotLightCount; ++i)
            {
                lightingResult.diffuse += CalculateSpotLightDiffuse(gSpotLights[i], input.worldPosition, input.normal, gMaterial.color.rgb, textureColor.rgb);
                lightingResult.specular += CalculateSpotLightSpecular(gSpotLights[i], input.worldPosition, input.normal, toEye, gMaterial.shininess);
            }
            output.color.rgb = CombineLightingResults(lightingResult);
        }
            // エリアライト
        else if (gMaterial.enableLighting == 6)
        {
            lightingResult.diffuse += CalculateAreaLightDiffuse(gAreaLight, input.worldPosition, input.normal, gMaterial.color.rgb, textureColor.rgb);
            lightingResult.specular += CalculateAreaLightSpecular(gAreaLight, input.worldPosition, input.normal, toEye, gMaterial.shininess);
            output.color.rgb = CombineLightingResults(lightingResult);
        }
            // 環境ライト
        else if (gMaterial.enableLighting == 7)
        {
                // 環境ライトの計算
            output.color.rgb = CalculateAmbientLight(gAmbientLight, gMaterial.color.rgb, textureColor.rgb);
                
            float3 diffuseDirectionalLight = CalculateDirectionalLightDiffuse(gDirectionalLight, input.normal, gMaterial.color.rgb, textureColor.rgb);
            float3 specularDirectionalLight = CalculateDirectionalLightSpecular(gDirectionalLight, input.normal, toEye, gMaterial.shininess);
            output.color.rgb += diffuseDirectionalLight + specularDirectionalLight;
        }
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }

        // 環境マッピングの適用
    output.color.rgb += environmentColor.rgb * gMaterial.environmentCoefficient;
    
    // Dissolveエッジ効果の適用
    if (gMaterial.enableDissolve != 0)
    {
        float mask = gMaskTexture.Sample(gSampler, transformedUV.xy);
        float edge = 1.0f - smoothstep(gMaterial.dissolveThreshold,
                                      gMaterial.dissolveThreshold + gMaterial.dissolveEdgeWidth,
                                      mask);
        output.color.rgb += edge * gMaterial.dissolveEdgeColor;
    }

    output.color.a = gMaterial.color.a * textureColor.a;

        // textureのα値が0の時にPixelを破棄
    if (textureColor.a == 0.0)
    {
        discard;
    }
        // textureのα値が0.5以下の時にPixelを棄却
    if (textureColor.a <= 0.5)
    {
        discard;
    }
        
        // textureのα値が0.5以下の時にPixelを棄却
    if (output.color.a == 0.0)
    {
        discard;
    }
    
   // 影の計算、適用
    float3 posFromLightVP = input.tpos.xyz / input.tpos.w;
    float2 shadowUV = (posFromLightVP.xy + float2(1, -1)) * float2(0.5f, -0.5f);
    
    float depthFromLight = gShadowMap.SampleCmp(gShadowSampler, shadowUV, posFromLightVP.z - 0.005f);
    
    float shadowWeight = lerp(0.5f, 1.0f, depthFromLight);
    
    output.color.rgb *= shadowWeight;
    
    return output;
}