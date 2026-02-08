#pragma once
#include <cstdint>

namespace KetaEngine {

/// <summary>
/// ライティングモードの種類
/// </summary>
enum class LightingType : int32_t {
    None               = 0, // ライティングなし
    Lambert            = 1, // ランバート反射
    HalfLambert        = 2, // ハーフランバート反射
    SpecularReflection = 3, // 鏡面反射
    PointLight         = 4, // ポイントライト
    SpotLight          = 5, // スポットライト
    AreaLight          = 6, // エリアライト
    Ambient            = 7, // 環境光
};

} // namespace KetaEngine
