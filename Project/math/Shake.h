#include "Vector2.h"
#include "Vector3.h"
#include "Random.h"

template<typename T>
T Shake(const float& t, const float& strength) {
    // t を [0, 1] に正規化
    float normalizedT = std::clamp(t, 0.0f, 1.0f); // t が 0 から 1 の範囲に収まるように
    float adjustedStrength = normalizedT * strength;

    if constexpr (std::is_same<T, float>::value) {
        return Random::Range(-adjustedStrength, adjustedStrength);
    }
    else if constexpr (std::is_same<T, Vector2>::value) {
        return Vector2(
            Random::Range(-adjustedStrength, adjustedStrength),
            Random::Range(-adjustedStrength, adjustedStrength)
        );
    }
    else if constexpr (std::is_same<T, Vector3>::value) {
        return Vector3(
            Random::Range(-adjustedStrength, adjustedStrength),
            Random::Range(-adjustedStrength, adjustedStrength),
            Random::Range(-adjustedStrength, adjustedStrength)
        );
    }
    else {
        static_assert(false, "Shake cannot cast");
    }
}

// 明示的なテンプレートインスタンス化
template float Shake<float>(const float& t, const float& strength);
template Vector2 Shake<Vector2>(const float& t, const float& strength);
template Vector3 Shake<Vector3>(const float& t, const float& strength);
