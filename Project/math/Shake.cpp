//#include "shake.h"
//#include"random.h"
//
//template<typename float> float Shake(const float& t, const float& strength);
//template<typename Vector2> Vector2 Shake(const float& t, const float& strength);
//template<typename Vector3> Vector3 Shake(const float& t, const float& strength);
//
//
//template<typename T> T Shake(const float& t, const float& strength) {
//    float adjustedStrength = t * strength;
//  
//    if constexpr (std::is_same<T, float>::value) {
//		return Random::Range(-adjustedStrength, adjustedStrength);
//    }
//	else if constexpr (std::is_same<T, Vector2>::value) {
//		return Vector2(Random::Range(-adjustedStrength, adjustedStrength),
//			           Random::Range(-adjustedStrength, adjustedStrength)
//		);
//	}
//	else if constexpr (std::is_same<T, Vector3>::value) {
//		return Vector3(Random::Range(-adjustedStrength, adjustedStrength),
//			           Random::Range(-adjustedStrength, adjustedStrength),
//			           Random::Range(-adjustedStrength, adjustedStrength)
//		);
//	}
//	else {
//		static_assert(false, "shake Can not Cast ");
//	}
//}
