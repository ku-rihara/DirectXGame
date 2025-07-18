struct SpotLight
{
    float4 color;     //ライトの色
    float3 position;  //ライトの位置
    float intensity;  //輝度
    float3 direction; //スポットライトの方向
    float distance;   //ライトの届く最大距離
    float decay;      //減衰率
    float cosAngle;   //スポットライトの余弦
    float cosFalloffStart;
};

// スポットライトの距離減衰計算
float CalculateSpotLightDistanceAttenuation(float3 lightPosition, float3 worldPosition, float maxDistance, float decay)
{
    float distance = length(lightPosition - worldPosition);
    return pow(saturate(-distance / maxDistance + 1.0f), decay);
}

// スポットライトの角度減衰計算
float CalculateSpotLightAngleAttenuation(float3 lightDirection, float3 spotDirection, float cosAngle, float cosFalloffStart)
{
    float currentCosAngle = dot(lightDirection, spotDirection);
    return saturate((currentCosAngle - cosAngle) / (cosFalloffStart - cosAngle));
}

// スポットライトの拡散反射計算
float3 CalculateSpotLightDiffuse(SpotLight light, float3 worldPosition, float3 normal, float3 materialColor, float3 textureColor)
{
    float3 spotLightDirectionSurface = normalize(worldPosition - light.position);
    
    // 距離減衰
    float attenuationFactor = CalculateSpotLightDistanceAttenuation(light.position, worldPosition, light.distance, light.decay);
    
    // 拡散反射
    float NdotLSpot = dot(normalize(normal), -spotLightDirectionSurface);
    float cosSpot = saturate(NdotLSpot);
    
    // 角度減衰
    float falloffFactor = CalculateSpotLightAngleAttenuation(spotLightDirectionSurface, light.direction, light.cosAngle, light.cosFalloffStart);
    
    return materialColor * textureColor * light.color.rgb * cosSpot * light.intensity * attenuationFactor * falloffFactor;
}

// スポットライトの鏡面反射計算
float3 CalculateSpotLightSpecular(SpotLight light, float3 worldPosition, float3 normal, float3 toEye, float shininess)
{
    float3 spotLightDirectionSurface = normalize(worldPosition - light.position);
    
    // 距離減衰
    float attenuationFactor = CalculateSpotLightDistanceAttenuation(light.position, worldPosition, light.distance, light.decay);
    
    // 鏡面反射
    float3 halfVectorSpot = normalize(-spotLightDirectionSurface + toEye);
    float NdotHSpot = dot(normalize(normal), halfVectorSpot);
    float specularPowSpot = pow(saturate(NdotHSpot), shininess);
    
    // 角度減衰
    float falloffFactor = CalculateSpotLightAngleAttenuation(spotLightDirectionSurface, light.direction, light.cosAngle, light.cosFalloffStart);
    
    return light.color.rgb * light.intensity * specularPowSpot * attenuationFactor * falloffFactor;
}

// スポットライトの計算
float3 CalculateSpotLightLighting(SpotLight light, float3 worldPosition, float3 normal, float3 toEye, float3 materialColor, float3 textureColor, float shininess)
{
    float3 diffuse = CalculateSpotLightDiffuse(light, worldPosition, normal, materialColor, textureColor);
    float3 specular = CalculateSpotLightSpecular(light, worldPosition, normal, toEye, shininess);
    return diffuse + specular;
}