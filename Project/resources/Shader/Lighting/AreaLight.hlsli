struct AreaLight
{
    float4 color; // 色
    float3 position; // 座標
    float padding1;
    float3 normal;
    float intensity; // 輝度
    float width; // 横幅
    float height; // 高さ
    float decay; // 減衰率
    float3 up;
    float padding2;
};

struct AreaLightRangeInfo
{
    bool isInRange;
    float3 localPoint;
    float x;
    float y;
    float falloffFactor;
};

// エリアライトの距離減衰計算
float CalculateAreaLightDistanceAttenuation(float distance, float decay)
{
    return pow(saturate(-distance / 10.0f + 1.0f), decay);
}

// エリアライトの範囲チェック
AreaLightRangeInfo CheckAreaLightRange(AreaLight light, float3 worldPosition)
{
    AreaLightRangeInfo info;
    
    // ワールド座標からエリアライトの中心位置までのベクトル
    float3 toLight = light.position - worldPosition;
    
    // ライトの法線と逆方向のベクトルを計算
    float3 lightNormal = -normalize(light.normal);
    
    // 平面への投影
    float dist = dot(toLight, lightNormal);
    float3 projectedPoint = worldPosition + dist * lightNormal;
    
    // 矩形ライトのローカル座標系での座標を計算
    float3 right = normalize(cross(light.up, light.normal));
    float3 up = normalize(light.up);
    info.localPoint = projectedPoint - light.position;
    info.x = dot(info.localPoint, right);
    info.y = dot(info.localPoint, up);
    
    // エリアライトの範囲内かどうか
    info.isInRange = (abs(info.x) <= light.width * 0.5f) && (abs(info.y) <= light.height * 0.5f);
    
    if (!info.isInRange)
    {
        // 範囲外での減衰計算
        float falloffRange = 1.0f;
        float edgeDistance = max(abs(info.x) - light.width * 0.5f, abs(info.y) - light.height * 0.5f);
        info.falloffFactor = saturate(1.0f - edgeDistance / falloffRange);
    }
    else
    {
        info.falloffFactor = 1.0f;
    }
    
    return info;
}

// エリアライトの拡散反射計算
float3 CalculateAreaLightDiffuse(AreaLight light, float3 worldPosition, float3 normal, float3 materialColor, float3 textureColor)
{
    float3 toLight = light.position - worldPosition;
    float3 lightDir = normalize(light.position - worldPosition);
    float distance = length(toLight);
    
    float attenuationFactor = CalculateAreaLightDistanceAttenuation(distance, light.decay);
    float diffuse = saturate(dot(normalize(normal), -lightDir));
    
    return materialColor * textureColor * light.color.rgb * diffuse * light.intensity * attenuationFactor;
}

// エリアライトの鏡面反射計算
float3 CalculateAreaLightSpecular(AreaLight light, float3 worldPosition, float3 normal, float3 toEye, float shininess)
{
    float3 toLight = light.position - worldPosition;
    float3 lightDir = normalize(light.position - worldPosition);
    float distance = length(toLight);
    
    float attenuationFactor = CalculateAreaLightDistanceAttenuation(distance, light.decay);
    
    float3 halfVector = normalize(-lightDir + toEye);
    float specular = pow(saturate(dot(normalize(normal), halfVector)), shininess);
    
    return light.color.rgb * light.intensity * specular * attenuationFactor;
}

// エリアライトのライティング計算
float3 CalculateAreaLightLighting(AreaLight light, float3 worldPosition, float3 normal, float3 toEye, float3 materialColor, float3 textureColor, float shininess)
{
    AreaLightRangeInfo rangeInfo = CheckAreaLightRange(light, worldPosition);
    
    float3 diffuse = CalculateAreaLightDiffuse(light, worldPosition, normal, materialColor, textureColor);
    float3 specular = CalculateAreaLightSpecular(light, worldPosition, normal, toEye, shininess);
    
    return (diffuse + specular) * rangeInfo.falloffFactor;
}
