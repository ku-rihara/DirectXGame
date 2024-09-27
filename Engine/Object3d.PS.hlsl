#include "object3d.hlsli"


Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
};
//マテリアル
ConstantBuffer<Material> gMaterial : register(b0);
struct DirectionalLight
{
    float4 color; //ライトの色
    float3 direction; //ライトの向き
    float intensity; //輝度
  
};
//ライト
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};
struct Camera
{
    float3 worldPosition;
};
//鏡面反射
ConstantBuffer<Camera> gCamera : register(b2);

struct PointLight
{
    float4 color; //ライトの色
    float3 position; //ライトの位置
    float intenesity;//輝度
    float radius; //ライトの幅
    float decay; //減衰率
};
//ポイントライト
ConstantBuffer<PointLight> gPointLight : register(b3);

struct SpotLight
{
    float4 color; //ライトの色
    float3 position; //ライトの位置
    float intensity; //輝度
    float3 direction; //スポットライトの方向
    float distance; //ライトの届く最大距離
    float decay; //減衰率
    float cosAngle; //スポットライトの余弦
};

ConstantBuffer<SpotLight> gSpotLight : register(b4);

PixelShaderOutput main(VertexShaderOutput input)
{
    float3 diffuseDirectionalLight; //拡散反射
    float3 specularDirectionalLight; //鏡面反射
    float3 diffusePointLight; //拡散反射(Point)
    float3 specularPointLight; //鏡面反射(Point)
    float3 diffuseSpotLight; //拡散反射(Spot)
    float3 specularSpotLight; //鏡面反射(Spot)
    PixelShaderOutput output; //output
    //uv
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    //color
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    float NdotL = 0.0f;
    float cos = 0.0f;
   
    //ライティングあり
    if (gMaterial.enableLighting != 0)
    {
           //camera
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
            //ライトの反射ベクトル
        float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            //内積
        float NdotH = dot(normalize(input.normal), halfVector);
        float cosDirrectional = saturate(NdotH); // 拡散反射のための余弦
            //反射強度
        float specularPow = pow(saturate(NdotH), gMaterial.shininess);
         //拡散反射
        diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cosDirrectional * gDirectionalLight.intensity;
            //鏡面反射
        specularDirectionalLight = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
    
        //Lambart
        if (gMaterial.enableLighting == 1)
        {
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
            output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        }
        //HalfLambart
        else if (gMaterial.enableLighting == 2)
        {
            NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        }
        //Specular Reflection
        else if (gMaterial.enableLighting == 3)
        {
                   //拡散反射・鏡面反射
            output.color.rgb = diffuseDirectionalLight + specularDirectionalLight;
        }
          //PointLight
        else if (gMaterial.enableLighting == 4)
        {
            // 入射光
            float3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
            //ポイントライトへの距離
            float distancePoint = length(gPointLight.position - input.worldPosition);
            //減衰係数
            float factorPoint = pow(saturate(-distancePoint / gPointLight.radius + 1.0f), gPointLight.decay);
            // ライトの反射ベクトル
            float3 halfVectorPoint = normalize(-pointLightDirection + toEye);
    
            // 法線と入射光の内積（拡散反射用）
            float NdotLPoint = dot(normalize(input.normal), -pointLightDirection);
            float cosPoint = saturate(NdotLPoint); // 拡散反射のための余弦
    
            // 反射ベクトルとの内積（鏡面反射用）
            float NdotHPoint = dot(normalize(input.normal), halfVectorPoint);
            float specularPowPoint = pow(saturate(NdotHPoint), gMaterial.shininess);
    
           // 拡散反射の計算 (PointLight)
            diffusePointLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cosPoint * gPointLight.intenesity * factorPoint;
    
            // 鏡面反射の計算 (PointLight)
            specularPointLight = gPointLight.color.rgb * gPointLight.intenesity * specularPowPoint * float3(1.0f, 1.0f, 1.0f);
    
            // PointLight と Directional Light の反射を足す
            output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight;
        }
           //SpotLight
        else if (gMaterial.enableLighting == 5)
        {
            float3 spotLightDirectionSurface = normalize(input.worldPosition - gSpotLight.position);
            //スポットライトへの距離
            float distanceSpot = length(gSpotLight.position - input.worldPosition);
            //減衰係数
            float attenuationFactor = pow(saturate(-distanceSpot / gSpotLight.distance + 1.0f), gPointLight.decay);
            // ライトの反射ベクトル
            float3 halfVectorSpot = normalize(-spotLightDirectionSurface + toEye);
    
            // 法線と入射光の内積（拡散反射用）
            float NdotLSpot = dot(normalize(input.normal), -spotLightDirectionSurface);
            float cosSpot = saturate(NdotLSpot); // 拡散反射のための余弦
            
            float cosAngle = dot(spotLightDirectionSurface, gSpotLight.direction);
            float falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (1.0f - gSpotLight.cosAngle));
    
            // 反射ベクトルとの内積（鏡面反射用）
            float NdotHSpot = dot(normalize(input.normal), halfVectorSpot);
            float specularPowSpot = pow(saturate(NdotHSpot), gMaterial.shininess);
    
           // 拡散反射の計算 (PointLight)
            diffuseSpotLight = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cosSpot * gSpotLight.intensity * attenuationFactor*falloffFactor;
    
            // 鏡面反射の計算 (PointLight)
            specularSpotLight = gPointLight.color.rgb * gPointLight.intenesity * specularPowSpot * float3(1.0f, 1.0f, 1.0f);
            
                   // PointLight と Directional Light の反射を足す
            output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffuseSpotLight + specularSpotLight;
    
    
        }
        output.color.a = gMaterial.color.a * textureColor.a;
   
    }
    else
    { //ライティングなし
        output.color = gMaterial.color * textureColor;
    }
    
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
