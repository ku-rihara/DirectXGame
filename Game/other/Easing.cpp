
#include "Easing.h"
#include "Pi.h"
#include <cmath>
//
template Vector3 EaseAmplitudeScale<Vector3>(const Vector3& initScale, const float& easeT, const float& easeTime, const float& amplitude, const float& period);
template Vector2 EaseAmplitudeScale<Vector2>(const Vector2& initScale, const float& easeT, const float& easeTime, const float& amplitude, const float& period);
template float EaseAmplitudeScale<float>(const float& initScale, const float& easeT, const float& easeTime, const float& amplitude, const float& period);

float Lerp(const float& start, const float& end, float t) { return (1.0f - t) * start + end * t; }

Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
	Vector3 result;
	result.x = (1.0f - t) * start.x + end.x * t;
	result.y = (1.0f - t) * start.y + end.y * t;
	result.z = (1.0f - t) * start.z + end.z * t;
	return result;
}

Vector2 Lerp(const Vector2& start, const Vector2& end, float t) {
	Vector2 result;
	result.x = (1.0f - t) * start.x + end.x * t;
	result.y = (1.0f - t) * start.y + end.y * t;

	return result;
}

Vector3 SLerp(const Vector3& start, const Vector3& end, float t) {
	Vector3 Nstart = Normalize(start);
	Vector3 Nend = Normalize(end);
	// 内積を求める
	float dot = Dot(Nstart, Nend);
	// 誤差により1.0fを超えるのを防ぐ
	if (dot > 1.0f) {
		dot = 1.0f;
	}
	// アークコサインでθの角度を求める
	float theta = std::acos(dot);
	// θの角度からsinθを求める
	float sinTheta = std::sin(theta);
	// サイン(θ(1-t))を求める
	float sinThetaFrom = std::sin((1 - t) * theta);
	// サインθtを求める
	float sinThetaTo = std::sin(t * theta);
	Vector3 NormalizeVector;
	if (sinTheta < 1.0e-5) {
		NormalizeVector = Nstart;
	} else {
		// 球面線形補間したベクトル(単位ベクトル)
		NormalizeVector.x = (sinThetaFrom * Nstart.x + sinThetaTo * Nend.x) / sinTheta;
		NormalizeVector.y = (sinThetaFrom * Nstart.y + sinThetaTo * Nend.y) / sinTheta;
		NormalizeVector.z = (sinThetaFrom * Nstart.z + sinThetaTo * Nend.z) / sinTheta;
	}
	// ベクトルの長さはstartとendの長さを線形補間
	float length1 = Length(start);
	float length2 = Length(end);
	// Lerpで補間ベクトルの長さを求める
	float length = Lerp(length1, length2, t);
	// 長さを反映
	return NormalizeVector * length;
}

float InElasticAmplitude(float t, const float& totaltime, const float& amplitude, const float& period) {

	if (t <= 0.0f) {
		return 0.0f;
	}
	if (t >= totaltime) {
		return 0.0f;
	}
	float s = period / (2.0f * pi) * std::asinf(1.0f);
	t /= totaltime;

	return -amplitude * std::powf(2.0f, 10.0f * (t - 1.0f)) * std::sinf((t - 1.0f - s) * (2.0f * pi) / period);
}

float OutElasticAmplitude(float t, float totaltime, float amplitude, float period) {
	if (t <= 0.0f)
		return 0.0f;
	if (t >= totaltime)
		return 0.0f;

	float s = period / (2.0f * pi) * std::asin(1.0f);
	t /= totaltime;

	return amplitude * std::pow(2.0f, -10.0f * t) * std::sin((t - s) * (2.0f * pi) / period);
}

float InOutElasticAmplitude(float t, float totaltime, float amplitude, float period) {
	if (t <= 0.0f)
		return 0.0f;
	if (t >= totaltime)
		return 0.0f;

	float backPoint = 0.5f;
	t /= totaltime;

	if (t < backPoint) {
		return OutElasticAmplitude(t, totaltime, amplitude, period);
	} else {

		return InElasticAmplitude(t - backPoint, totaltime - backPoint, amplitude, period);
	}
}

template<typename T> T EaseAmplitudeScale(const T& initScale, const float& easeT, const float& totalTime, const float& amplitude, const float& period) {
	T newScale = initScale; // T型のnewScaleを宣言

	if constexpr (std::is_same<T, float>::value) {
		newScale = initScale + -OutElasticAmplitude(easeT, totalTime, amplitude, period);
	} else if constexpr (std::is_same<T, Vector2>::value) {
		newScale.x = initScale.x + -OutElasticAmplitude(easeT, totalTime, amplitude, period);
		newScale.y = initScale.y + OutElasticAmplitude(easeT, totalTime, amplitude, period);
	} else if constexpr (std::is_same<T, Vector3>::value) {
		newScale.x = initScale.x + -OutElasticAmplitude(easeT, totalTime, amplitude, period);
		newScale.y = initScale.y + OutElasticAmplitude(easeT, totalTime, amplitude, period);
		newScale.z = initScale.z + -OutElasticAmplitude(easeT, totalTime, amplitude, period);
	}

	return newScale;
}