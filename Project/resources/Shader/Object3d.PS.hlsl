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
    float intenesity; //輝度
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
    float cosFalloffStart;
};
//スポットライト
ConstantBuffer<SpotLight> gSpotLight : register(b4);


struct AreaLight
{
    float4 color; // 16バイト
    float3 position; // 12バイト
    float padding1; // アライメント用パディング
    float3 normal; // 12バイト
    float intensity; // 4バイト
    float width; // 4バイト
    float height; // 4バイト
    float decay; // 4バイト
    float3 up; // 12バイト
    float padding2; // アライメント用パディング
};



//エリアライト
ConstantBuffer<AreaLight> gAreaLight : register(b5);



PixelShaderOutput main(VertexShaderOutput input)
{
    float3 diffuseDirectionalLight; //拡散反射
    float3 specularDirectionalLight; //鏡面反射
    float3 diffusePointLight; //拡散反射(Point)
    float3 specularPointLight; //鏡面反射(Point)
    float3 diffuseSpotLight; //拡散反射(Spot)
    float3 specularSpotLight; //鏡面反射(Spot)
    float3 diffuseAreaLight; //拡散反射(Area)
    float3 specularAreaLight; //鏡面反射(Area)
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
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cosDirrectional = pow(NdotL * 0.5f + 0.5f, 2.0f);
           //camera
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
            //ライトの反射ベクトル
        float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            //内積
        float NdotH = dot(normalize(input.normal), halfVector);
      
            
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
          // 入射光（スポットライトの方向ベクトル）
            float3 spotLightDirectionSurface = normalize(input.worldPosition - gSpotLight.position);

             // スポットライトへの距離
            float distanceSpot = length(gSpotLight.position - input.worldPosition);

            // 減衰係数
            float attenuationFactor = pow(saturate(-distanceSpot / gSpotLight.distance + 1.0f), gSpotLight.decay);

              // 法線と入射光の内積（拡散反射用）
            float NdotLSpot = dot(normalize(input.normal), -spotLightDirectionSurface);
            float cosSpot = saturate(NdotLSpot); // 拡散反射のための余弦

              // スポットライトの方向と角度の計算
            float cosAngle = dot(spotLightDirectionSurface, gSpotLight.direction);
            float falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));

             // 拡散反射の計算 (SpotLight)
            diffuseSpotLight = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cosSpot * gSpotLight.intensity * attenuationFactor * falloffFactor;

            // 鏡面反射を考慮しない
            specularSpotLight = float3(0.0f, 0.0f, 0.0f);

             // スポットライトと他のライトの結果を加算
            output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffuseSpotLight + specularSpotLight;
        }
        //AreaLight
        else if (gMaterial.enableLighting == 6)
        {
            // ワールド座標からエリアライトの中心位置までのベクトル
            float3 toLight = gAreaLight.position - input.worldPosition;

            // ライトの法線と逆方向のベクトルを計算
            float3 lightNormal = -normalize(gAreaLight.normal);

            // 平面への投影
            float dist = dot(toLight, lightNormal);
            float3 projectedPoint = input.worldPosition + dist * lightNormal;

           // 矩形ライトのローカル座標系での座標を計算
            float3 right = normalize(cross(gAreaLight.up, gAreaLight.normal));
            float3 up = normalize(gAreaLight.up);
            float3 localPoint = projectedPoint - gAreaLight.position;
            float x = dot(localPoint, right);
            float y = dot(localPoint, up);
    
            if (abs(x) <= gAreaLight.width * 0.5 && abs(y) <= gAreaLight.height * 0.5)
            {
                // 拡散反射の計算
                float3 lightDir = normalize(gAreaLight.position - input.worldPosition);
                float diffuse = saturate(dot(normalize(input.normal), -lightDir));
    
               //距離減衰
                float distance = length(toLight);
                float attenuationFactor = pow(saturate(-distance / 10 + 1.0f), gAreaLight.decay);
    
                diffuseAreaLight = gMaterial.color.rgb * textureColor.rgb * gAreaLight.color.rgb * diffuse * gAreaLight.intensity * attenuationFactor;
    
               // 鏡面反射の計算（仮）
                float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
                float3 halfVector = normalize(-lightDir + toEye);
                float specular = pow(saturate(dot(normalize(input.normal), halfVector)), gMaterial.shininess);
                specularAreaLight = gAreaLight.color.rgb * gAreaLight.intensity * specular;
    
                output.color.rgb = diffuseAreaLight + specularAreaLight; // 他ライトを排除してテスト

            }
            else
            {
                output.color.rgb = diffuseDirectionalLight + specularDirectionalLight;
            }

        }
        
        // 環境ライト (Ambient Light)
        else if (gMaterial.enableLighting == 7)
        {
             // 環境光の色を計算
            float3 ambientLight = gMaterial.color.rgb * textureColor.rgb * gAreaLight.color.rgb * gAreaLight.intensity;

            // 環境光のみを適用 (他のライトを除外してテスト)
            output.color.rgb = ambientLight;

         // 他のライトと組み合わせたい場合は、コメントを外す
            output.color.rgb += diffuseDirectionalLight + specularDirectionalLight;
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