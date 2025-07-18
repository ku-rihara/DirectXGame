#include "object3d.hlsli"
#include "Lighting/Light.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
    float environmentCoefficient;
    int pointLightCount;
    int spotLightCount;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Camera
{
    float3 worldPosition;
};

//struct LightCounts
//{
//    int pointLightCount;
//    int spotLightCount;
//};

//鏡面反射
ConstantBuffer<Camera> gCamera : register(b2);

ConstantBuffer<Material> gMaterial : register(b0);

// 各種ライトの構造体と定数バッファ
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
//ConstantBuffer<PointLight> gPointLight : register(b3);
//ConstantBuffer<SpotLight> gSpotLight : register(b4);
ConstantBuffer<AreaLight> gAreaLight : register(b5);
ConstantBuffer<AmbientLight> gAmbientLight : register(b6);

TextureCube<float4> gEnvironmentTexture : register(t1);

StructuredBuffer<PointLight> gPointLights : register(t2);
StructuredBuffer<SpotLight> gSpotLights : register(t3);

//ConstantBuffer<LightCounts> gLightCounts : register(b7);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // UV変換
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);

    // テクスチャカラー
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

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
            for (int i = 0; i < gMaterial.pointLightCount; ++i)
            {
                lightingResult.diffuse += CalculatePointLightDiffuse(gPointLights[i], input.worldPosition, input.normal, gMaterial.color.rgb, textureColor.rgb);
                lightingResult.specular += CalculatePointLightSpecular(gPointLights[i], input.worldPosition, input.normal, toEye, gMaterial.shininess);
            }
            output.color.rgb = CombineLightingResults(lightingResult);
        }
        // スポットライト
        else if (gMaterial.enableLighting == 5)
        {
            for (int i = 0; i < gMaterial.spotLightCount; ++i)
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
            
            // 方向ライトの計算も追加
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

    output.color.a = gMaterial.color.a * textureColor.a;

    //textureのα値が0の時にPixelを破棄
    if (textureColor.a == 0.0)
    {
        discard;
    }
    //textureのα値が0.5以下の時にPixelを棄却
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    
    //textureのα値が0.5以下の時にPixelを棄却
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    return output;
}