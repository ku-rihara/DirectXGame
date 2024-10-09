#include "MathFunction.h"
#include <cmath>

float Lerp(const float& start, const float& end, float t) {
	return (1.0f - t) * start + end * t;
}

Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
	Vector3 result;
	result.x = (1.0f - t) * start.x + end.x * t;
	result.y = (1.0f - t) * start.y + end.y * t;
	result.z = (1.0f - t) * start.z + end.z * t;
	return result;
}


Vector3 SLerp(const Vector3& start, const Vector3& end, float t) {
    // ベクトルの正規化
    Vector3 Nstart = Normalize(start);
    Vector3 Nend = Normalize(end);

    // 内積を求める
    float dot = Dot(Nstart, Nend);

    // ドット積の誤差補正
    if (dot > 1.0f) {
        dot = 1.0f;
    }
    else if (dot < -1.0f) {
        dot = -1.0f;
    }

    // 角度θをアークコサインで求める
    float theta = std::acos(dot);

    // もしθが非常に小さい（つまりベクトルがほぼ同じ方向）なら、線形補間を使う
    if (std::abs(theta) < 1.0e-5) {
        return Lerp(start, end, t);  // 線形補間で十分
    }

    // sinθを求める
    float sinTheta = std::sin(theta);
    float sinThetaFrom = std::sin((1 - t) * theta);
    float sinThetaTo = std::sin(t * theta);

    // 球面線形補間したベクトル（単位ベクトル）
    Vector3 NormalizeVector = (sinThetaFrom * Nstart + sinThetaTo * Nend) / sinTheta;

    // ベクトルの長さを線形補間
    float length1 = Length(start);
    float length2 = Length(end);
    float length = Lerp(length1, length2, t);

    // 補間したベクトルに長さを掛けて返す
    return NormalizeVector * length;
}