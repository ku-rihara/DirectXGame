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
    float4 color;
    float3 position;
    float intenesity;
};
//ポイントライト
ConstantBuffer<PointLight> gPointLight : register(b3);


PixelShaderOutput main(VertexShaderOutput input)
{
    float3 diffuseDirectionalLight;
    float3 specularDirectionalLight;
    float3 diffusePointLight;
    float3 specularPointLight;
    PixelShaderOutput output;
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
            //反射強度
        float specularPow = pow(saturate(NdotH), gMaterial.shininess);   
         //拡散反射
        diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
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
            //入射光
            float3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
              //拡散反射
             diffusePointLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intenesity;
            //鏡面反射
             specularPointLight = gPointLight.color.rgb * gPointLight.intenesity * specularPow * float3(1.0f, 1.0f, 1.0f);
            //全部タス
            output.color.rgb = diffuseDirectionalLight + specularDirectionalLight+diffusePointLight+specularPointLight;
   
        }
        output.color.a = gMaterial.color.a * textureColor.a;
   
    }
    else
    {//ライティングなし
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
