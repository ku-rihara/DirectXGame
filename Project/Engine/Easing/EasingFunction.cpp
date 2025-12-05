
#include "EasingFunction.h"
#include"MathFunction.h"
///std
#include<numbers>
#include <cmath>



float EaseInElasticAmplitude(float t, float totalTime, float amplitude, float period) {

	if (t <= 0.0f) {
		return 0.0f;
	}
    if (t >= totalTime) {
		return 0.0f;
	}
	float s = period / (2.0f * std::numbers::pi_v<float>) * std::asinf(1.0f);
    t /= totalTime;

	return -amplitude * std::powf(2.0f, 10.0f * (t - 1.0f)) * std::sinf((t - 1.0f - s) * (2.0f * std::numbers::pi_v<float>) / period);
}

float EaseOutElasticAmplitude(float t, float totalTime, float amplitude, float period) {
	if (t <= 0.0f)
		return 0.0f;
    if (t >= totalTime)
		return 0.0f;

	float s = period / (2.0f * std::numbers::pi_v<float>) * std::asin(1.0f);
    t /= totalTime;

	return amplitude * std::pow(2.0f, -10.0f * t) * std::sin((t - s) * (2.0f * std::numbers::pi_v<float>) / period);
}

float EaseInOutElasticAmplitude(float t, float totalTime, float amplitude, float period) {
	if (t <= 0.0f)
		return 0.0f;
    if (t >= totalTime)
		return 0.0f;

	float backPoint = 0.5f;
    t /= totalTime;

	if (t < backPoint) {
        return EaseOutElasticAmplitude(t, totalTime, amplitude, period);
	} else {

		return EaseInElasticAmplitude(t - backPoint, totalTime - backPoint, amplitude, period);
	}
}

template<typename T> T EaseAmplitudeScale(T initScale, float easeT, float totalTime, float amplitude, float period) {
	T newScale = initScale; // T型のnewScaleを宣言

	if constexpr (std::is_same<T, float>::value) {
		newScale = initScale + -EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
	} else if constexpr (std::is_same<T, Vector2>::value) {
		newScale.x = initScale.x + -EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
		newScale.y = initScale.y + EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
	} else if constexpr (std::is_same<T, Vector3>::value) {
		newScale.x = initScale.x + -EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
		newScale.y = initScale.y + EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
		newScale.z = initScale.z + -EaseOutElasticAmplitude(easeT, totalTime, amplitude, period);
	}

	return newScale;
}

// EaseInSine 関数
template<typename T> T EaseInSine(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::cosf((t * std::numbers::pi_v<float>) / 2.0f);
	return Lerp(start, end, easeT);
}

// EaseOutSine 関数
template<typename T> T EaseOutSine(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = std::sinf((t * std::numbers::pi_v<float>) / 2.0f);
	return Lerp(start, end, easeT);
}

// EaseInOutSine 関数
template<typename T> T EaseInOutSine(T start, T end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT = 0.5f * (1.0f - std::cosf(t * std::numbers::pi_v<float>));
	return Lerp(start, end, easeT);
}

// EaseInQuint 関数
template<typename T> T EaseInQuint(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t * t * t * t;
	return Lerp(start, end, easeT);
}

// EaseOutQuint 関数
template<typename T> T EaseOutQuint(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::powf(1.0f - t, 5);
	return Lerp(start, end, easeT);
}

// EaseInOutQuint 関数
template<typename T> T EaseInOutQuint(T start, T end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * t * t * t * t * t;
	} else {
		t -= 2.0f;
		easeT = 0.5f * (std::powf(t, 5) + 2.0f);
	}
	return Lerp(start, end, easeT);
}

// EaseInCirc 関数
template<typename T> T EaseInCirc(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::sqrtf(1.0f - std::powf(t, 2));
	return Lerp(start, end, easeT);
}

// EaseOutCirc 関数
template<typename T> T EaseOutCirc(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = std::sqrtf(1.0f - std::powf(t - 1.0f, 2));
	return Lerp(start, end, easeT);
}

// EaseInOutCirc 関数
template<typename T> T EaseInOutCirc(T start, T end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = -0.5f * (std::sqrtf(1.0f - std::powf(t, 2)) - 1.0f);
	} else {
		t -= 2.0f;
		easeT = 0.5f * (std::sqrtf(1.0f - std::powf(t, 2)) + 1.0f);
	}
	return Lerp(start, end, easeT);
}

// EaseInExpo 関数
template<typename T> T EaseInExpo(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = (t == 0.0f) ? 0.0f : std::powf(2.0f, 10.0f * (t - 1.0f));
	return Lerp(start, end, easeT);
}

// EaseOutExpo 関数
template<typename T> T EaseOutExpo(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = (t == 1.0f) ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * t);
	return Lerp(start, end, easeT);
}

// EaseInOutExpo 関数
template<typename T> T EaseInOutExpo(T start, T end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * std::powf(2.0f, 10.0f * (t - 1.0f));
	} else {
		t -= 1.0f;
		easeT = 0.5f * (2.0f - std::powf(2.0f, -10.0f * t));
	}
	return Lerp(start, end, easeT);
}

// EaseInCubic 関数
template<typename T> T EaseInCubic(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t * t;
	return Lerp(start, end, easeT);
}

// EaseOutCubic 関数
template<typename T> T EaseOutCubic(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::powf(1.0f - t, 3);
	return Lerp(start, end, easeT);
}

// EaseInOutCubic 関数
template<typename T> T EaseInOutCubic(T start, T end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * t * t * t;
	} else {
		t -= 2.0f;
		easeT = 0.5f * (std::powf(t, 3) + 2.0f);
	}
	return Lerp(start, end, easeT);
}

// EaseInQuad 関数
template<typename T> T EaseInQuad(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t;
	return Lerp(start, end, easeT);
}

// EaseOutQuad 関数
template<typename T> T EaseOutQuad(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - (1.0f - t) * (1.0f - t);
	return Lerp(start, end, easeT);
}

// EaseInOutQuad 関数
template<typename T> T EaseInOutQuad(T start, T end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * t * t;
	} else {
		t -= 1.0f;
		easeT = -0.5f * (t * (t - 2.0f) - 1.0f);
	}
	return Lerp(start, end, easeT);
}

// EaseInQuart 関数
template<typename T> T EaseInQuart(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t * t * t;
	return Lerp(start, end, easeT);
}

// EaseOutQuart 関数
template<typename T> T EaseOutQuart(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::powf(1.0f - t, 4);
	return Lerp(start, end, easeT);
}

// EaseInOutQuart 関数
template<typename T> T EaseInOutQuart(T start, T end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * t * t * t * t;
	} else {
		t -= 2.0f;
		easeT = -0.5f * (std::powf(t, 4) - 2.0f);
	}
	return Lerp(start, end, easeT);
}

/// EaseInBack
template<typename T>
T EaseInBack(T start, T end, float x, float totalX) {
	const float s = 1.70158f;
	float t = x / totalX;
	float easeT = t * t * ((s + 1) * t - s);
	return Lerp(start, end, easeT);
}

/// EaseOutBack
template<typename T>
T EaseOutBack(T start, T end, float x, float totalX) {
	const float s = 1.70158f;
	float t = x / totalX - 1;
	float easeT = (t * t * ((s + 1) * t + s)) + 1;
	return Lerp(start, end, easeT);
}


/// EaseInOutBack
template<typename T>
T EaseInOutBack(T start, T end, float x, float totalX) {
	const float s = 1.70158f * 1.525f;
	float t = x / (totalX / 2.0f);
	float easeT;

	if (t < 1) {
		easeT = 0.5f * (t * t * ((s + 1) * t - s));
	} else {
		t -= 2;
		easeT = 0.5f * ((t * t * ((s + 1) * t + s)) + 2);
	}

	return Lerp(start, end, easeT);
}


//バウンス補助関数
float BounceEaseOut(float x) {

	const float n1 = 7.5625f;
	const float d1 = 2.75f;
	float easeT = 0.0f;

	if (x < 1.0f / d1) {
		easeT = n1 * x * x;
	} else if (x < 2.0f / d1) {
		x -= 1.5f / d1;
		easeT = n1 * x * x + 0.75f;
	} else if (x < 2.5f / d1) {
		x -= 2.25f / d1;
		easeT = n1 * x * x + 0.9375f;
	} else {
		x -= 2.625f / d1;
		easeT = n1 * x * x + 0.984375f;
	}
	return easeT;
}

// EaseInBounce 関数
template<typename T> T EaseInBounce(T start, T end, float x, float totalX) {
	float t = 1.0f - (x / totalX);
	float easeT = 1.0f - BounceEaseOut(t);
	return Lerp(start, end, easeT);
}

// EaseOutBounce 関数
template<typename T> T EaseOutBounce(T start, T end, float x, float totalX) {
	float t = x / totalX;
	float easeT = BounceEaseOut(t);
	return Lerp(start, end, easeT);
}

// EaseInOutBounce 関数
template<typename T> T EaseInOutBounce(T start, T end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT;
	if (t < 1.0f) {
		easeT = 0.5f * (1.0f - BounceEaseOut(1.0f - t));
	} else {
		t -= 1.0f;
		easeT = 0.5f * BounceEaseOut(t) + 0.5f;
	}
	return Lerp(start, end, easeT);
}
namespace Back {

	template<typename T> T Lerp(T start, T end, float t)
	{
		return start + (end - start) * t;
	}

	template<typename T> T  InSineZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			return Lerp(start, end, sinf((t / backPoint) * std::numbers::pi_v<float> *0.5f));
		} else
		{
			return Lerp(end, start, 1.0f - cosf(((t - backPoint) / (totalTime - backPoint)) * std::numbers::pi_v<float> *0.5f));
		}
	}

	template<typename T> T OutSineZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			return Lerp(start, end, 1.0f - cosf((t / backPoint) * std::numbers::pi_v<float> *0.5f));
		} else
		{
			return Lerp(end, start, sinf(((t - backPoint) / (totalTime - backPoint)) * std::numbers::pi_v<float> *0.5f));
		}
	}

	template<typename T> T InOutSineZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			return Lerp(start, end, -0.5f * (cosf(std::numbers::pi_v<float> *(t / backPoint)) - 1.0f));
		} else
		{
			return Lerp(end, start, 0.5f * (1.0f - cosf(std::numbers::pi_v<float> *((t - backPoint) / (totalTime - backPoint)))));
		}
	}

	template<typename T> T InQuadZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			return Lerp(start, end, normalizedTime * normalizedTime);
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			return Lerp(end, start, -normalizedTime * (normalizedTime - 2));
		}
	}

	template<typename T> T OutQuadZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			return Lerp(start, end, -normalizedTime * (normalizedTime - 2));
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			return Lerp(end, start, normalizedTime * normalizedTime);
		}
	}

	template<typename T> T InOutQuadZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / (backPoint * 0.5f);
			if (normalizedTime < 1) return Lerp(start, end, 0.5f * normalizedTime * normalizedTime);
			normalizedTime--;
			return Lerp(start, end, -0.5f * (normalizedTime * (normalizedTime - 2) - 1));
		} else
		{
			float normalizedTime = (t - backPoint) / ((totalTime - backPoint) * 0.5f);
			if (normalizedTime < 1) return Lerp(end, start, 0.5f * normalizedTime * normalizedTime);
			normalizedTime--;
			return Lerp(end, start, -0.5f * (normalizedTime * (normalizedTime - 2) - 1));
		}
	}

	template<typename T> T InCubicZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			return Lerp(start, end, normalizedTime * normalizedTime * normalizedTime);
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			normalizedTime--;
			return Lerp(end, start, normalizedTime * normalizedTime * normalizedTime + 1);
		}
	}

	template<typename T> T OutCubicZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			normalizedTime--;
			return Lerp(start, end, normalizedTime * normalizedTime * normalizedTime + 1);
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			return Lerp(end, start, normalizedTime * normalizedTime * normalizedTime);
		}
	}

	template<typename T> T InOutCubicZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / (backPoint * 0.5f);
			if (normalizedTime < 1) return Lerp(start, end, 0.5f * normalizedTime * normalizedTime * normalizedTime);
			normalizedTime -= 2;
			return Lerp(start, end, 0.5f * (normalizedTime * normalizedTime * normalizedTime + 2));
		} else
		{
			float normalizedTime = (t - backPoint) / ((totalTime - backPoint) * 0.5f);
			if (normalizedTime < 1) return Lerp(end, start, 0.5f * normalizedTime * normalizedTime * normalizedTime);
			normalizedTime -= 2;
			return Lerp(end, start, 0.5f * (normalizedTime * normalizedTime * normalizedTime + 2));
		}
	}

	template<typename T> T InQuartZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			return Lerp(start, end, normalizedTime * normalizedTime * normalizedTime * normalizedTime);
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			normalizedTime--;
			return Lerp(end, start, -(normalizedTime * normalizedTime * normalizedTime * normalizedTime - 1));
		}
	}

	template<typename T> T OutQuartZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			normalizedTime--;
			return Lerp(start, end, -(normalizedTime * normalizedTime * normalizedTime * normalizedTime - 1));
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			return Lerp(end, start, normalizedTime * normalizedTime * normalizedTime * normalizedTime);
		}
	}

	template<typename T> T InOutQuartZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / (backPoint * 0.5f);
			if (normalizedTime < 1) return Lerp(start, end, 0.5f * normalizedTime * normalizedTime * normalizedTime * normalizedTime);
			normalizedTime -= 2;
			return Lerp(start, end, -0.5f * (normalizedTime * normalizedTime * normalizedTime * normalizedTime - 2));
		} else
		{
			float normalizedTime = (t - backPoint) / ((totalTime - backPoint) * 0.5f);
			if (normalizedTime < 1) return Lerp(end, start, 0.5f * normalizedTime * normalizedTime * normalizedTime * normalizedTime);
			normalizedTime -= 2;
			return Lerp(end, start, -0.5f * (normalizedTime * normalizedTime * normalizedTime * normalizedTime - 2));
		}
	}

	template<typename T> T InQuintZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			return Lerp(start, end, normalizedTime * normalizedTime * normalizedTime * normalizedTime * normalizedTime);
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			normalizedTime--;
			return Lerp(end, start, normalizedTime * normalizedTime * normalizedTime * normalizedTime * normalizedTime + 1);
		}
	}

	template<typename T> T OutQuintZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			normalizedTime--;
			return Lerp(start, end, normalizedTime * normalizedTime * normalizedTime * normalizedTime * normalizedTime + 1);
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			return Lerp(end, start, normalizedTime * normalizedTime * normalizedTime * normalizedTime * normalizedTime);
		}
	}

	template<typename T> T InOutQuintZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / (backPoint * 0.5f);
			if (normalizedTime < 1) return Lerp(start, end, 0.5f * normalizedTime * normalizedTime * normalizedTime * normalizedTime * normalizedTime);
			normalizedTime -= 2;
			return Lerp(start, end, 0.5f * (normalizedTime * normalizedTime * normalizedTime * normalizedTime * normalizedTime + 2));
		} else
		{
			float normalizedTime = (t - backPoint) / ((totalTime - backPoint) * 0.5f);
			if (normalizedTime < 1) return Lerp(end, start, 0.5f * normalizedTime * normalizedTime * normalizedTime * normalizedTime * normalizedTime);
			normalizedTime -= 2;
			return Lerp(end, start, 0.5f * (normalizedTime * normalizedTime * normalizedTime * normalizedTime * normalizedTime + 2));
		}
	}

	template<typename T> T InExpoZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			return Lerp(start, end, (normalizedTime == 0) ? 0 : powf(2, 10 * (normalizedTime - 1)));
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			return Lerp(end, start, (normalizedTime == 1) ? 1 : (-powf(2, -10 * normalizedTime) + 1));
		}
	}

	template<typename T> T OutExpoZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			return Lerp(start, end, (normalizedTime == 1) ? 1 : (-powf(2, -10 * normalizedTime) + 1));
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			return Lerp(end, start, (normalizedTime == 0) ? 0 : powf(2, 10 * (normalizedTime - 1)));
		}
	}

	template<typename T> T InOutExpoZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / (backPoint * 0.5f);
			if (normalizedTime == 0) return start;
			if (normalizedTime == 2) return end;
			if (normalizedTime < 1) return Lerp(start, end, 0.5f * powf(2, 10 * (normalizedTime - 1)));
			normalizedTime--;
			return Lerp(start, end, 0.5f * (-powf(2, -10 * normalizedTime) + 2));
		} else
		{
			float normalizedTime = (t - backPoint) / ((totalTime - backPoint) * 0.5f);
			if (normalizedTime == 0) return end;
			if (normalizedTime == 2) return start;
			if (normalizedTime < 1) return Lerp(end, start, 0.5f * powf(2, 10 * (normalizedTime - 1)));
			normalizedTime--;
			return Lerp(end, start, 0.5f * (-powf(2, -10 * normalizedTime) + 2));
		}
	}

	template<typename T> T InCircZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			return Lerp(start, end, -(sqrtf(1 - normalizedTime * normalizedTime) - 1));
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			normalizedTime--;
			return Lerp(end, start, sqrtf(1 - normalizedTime * normalizedTime));
		}
	}

	template<typename T> T OutCircZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			normalizedTime--;
			return Lerp(start, end, sqrtf(1 - normalizedTime * normalizedTime));
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			return Lerp(end, start, -(sqrtf(1 - normalizedTime * normalizedTime) - 1));
		}
	}

	template<typename T> T InOutCircZero(T start, T end, float t, float totalTime, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / (backPoint * 0.5f);
			if (normalizedTime < 1) return Lerp(start, end, -0.5f * (sqrtf(1 - normalizedTime * normalizedTime) - 1));
			normalizedTime -= 2;
			return Lerp(start, end, 0.5f * (sqrtf(1 - normalizedTime * normalizedTime) + 1));
		} else
		{
			float normalizedTime = (t - backPoint) / ((totalTime - backPoint) * 0.5f);
			if (normalizedTime < 1) return Lerp(end, start, -0.5f * (sqrtf(1 - normalizedTime * normalizedTime) - 1));
			normalizedTime -= 2;
			return Lerp(end, start, 0.5f * (sqrtf(1 - normalizedTime * normalizedTime) + 1));
		}
	}

	template<typename T> T InBackZero(T start, T end, float t, float totalTime, float s, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			return Lerp(start, end, normalizedTime * normalizedTime * ((s + 1) * normalizedTime - s));
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			normalizedTime--;
			return Lerp(end, start, normalizedTime * normalizedTime * ((s + 1) * normalizedTime + s) + 1);
		}
	}

	template<typename T> T OutBackZero(T start, T end, float t, float totalTime, float s, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;

		if (t < backPoint)
		{
			float normalizedTime = t / backPoint;
			normalizedTime--;
			return Lerp(start, end, normalizedTime * normalizedTime * ((s + 1) * normalizedTime + s) + 1);
		} else
		{
			float normalizedTime = (t - backPoint) / (totalTime - backPoint);
			return Lerp(end, start, normalizedTime * normalizedTime * ((s + 1) * normalizedTime - s));
		}
	}

	template<typename T> T InOutBackZero(T start, T end, float t, float totalTime, float s, float backRaito)
	{
		if (t <= 0.0f) return start;
		if (t >= totalTime) return start;

		float backPoint = totalTime * backRaito;
		float s_modified = s * 1.525f;

		if (t < backPoint)
		{
			float normalizedTime = t / (backPoint * 0.5f);
			if (normalizedTime < 1) return Lerp(start, end, 0.5f * (normalizedTime * normalizedTime * ((s_modified + 1) * normalizedTime - s_modified)));
			normalizedTime -= 2;
			return Lerp(start, end, 0.5f * (normalizedTime * normalizedTime * ((s_modified + 1) * normalizedTime + s_modified) + 2));
		} else
		{
			float normalizedTime = (t - backPoint) / ((totalTime - backPoint) * 0.5f);
			if (normalizedTime < 1) return Lerp(end, start, 0.5f * (normalizedTime * normalizedTime * ((s_modified + 1) * normalizedTime - s_modified)));
			normalizedTime -= 2;
			return Lerp(end, start, 0.5f * (normalizedTime * normalizedTime * ((s_modified + 1) * normalizedTime + s_modified) + 2));
		}
	}
}

template Vector3 EaseAmplitudeScale<Vector3>(Vector3 initScale, float easeT, float easeTime, float amplitude, float period);
template Vector2 EaseAmplitudeScale<Vector2>(Vector2 initScale, float easeT, float easeTime, float amplitude, float period);
template float EaseAmplitudeScale<float>(float initScale, float easeT, float easeTime, float amplitude, float period);

template Vector3 EaseInSine<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInSine<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInSine<float>(float start, float end, float x, float totalX);

template Vector3 EaseOutSine<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseOutSine<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseOutSine<float>(float start, float end, float x, float totalX);

template Vector3 EaseInOutSine<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInOutSine<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInOutSine<float>(float start, float end, float x, float totalX);

template Vector3 EaseInQuint<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInQuint<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInQuint<float>(float start, float end, float x, float totalX);

template Vector3 EaseOutQuint<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseOutQuint<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseOutQuint<float>(float start, float end, float x, float totalX);

template Vector3 EaseInOutQuint<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInOutQuint<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInOutQuint<float>(float start, float end, float x, float totalX);

template Vector3 EaseInCirc<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInCirc<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInCirc<float>(float start, float end, float x, float totalX);

template Vector3 EaseOutCirc<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseOutCirc<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseOutCirc<float>(float start, float end, float x, float totalX);

template Vector3 EaseInOutCirc<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInOutCirc<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInOutCirc<float>(float start, float end, float x, float totalX);

template Vector3 EaseInExpo<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInExpo<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInExpo<float>(float start, float end, float x, float totalX);

template Vector3 EaseOutExpo<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseOutExpo<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseOutExpo<float>(float start, float end, float x, float totalX);

template Vector3 EaseInOutExpo<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInOutExpo<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInOutExpo<float>(float start, float end, float x, float totalX);

template Vector3 EaseOutCubic<Vector3>(Vector3 start, Vector3 end, float x, float totalx);
template Vector2 EaseOutCubic<Vector2>(Vector2 start, Vector2 end, float x, float totalx);
template float EaseOutCubic<float>(float start, float end, float x, float totalx);

template Vector3 EaseInCubic<Vector3>(Vector3 start, Vector3 end, float x, float totalx);
template Vector2 EaseInCubic<Vector2>(Vector2 start, Vector2 end, float x, float totalx);
template float EaseInCubic<float>(float start, float end, float x, float totalx);

template Vector3 EaseInOutCubic<Vector3>(Vector3 start, Vector3 end, float x, float totalx);
template Vector2 EaseInOutCubic<Vector2>(Vector2 start, Vector2 end, float x, float totalx);
template float EaseInOutCubic<float>(float start, float end, float x, float totalx);

template Vector3 EaseInQuad<Vector3>(Vector3 start, Vector3 end, float x, float totalx);
template Vector2 EaseInQuad<Vector2>(Vector2 start, Vector2 end, float x, float totalx);
template float EaseInQuad<float>(float start, float end, float x, float totalx);

template Vector3 EaseOutQuad<Vector3>(Vector3 start, Vector3 end, float x, float totalx);
template Vector2 EaseOutQuad<Vector2>(Vector2 start, Vector2 end, float x, float totalx);
template float EaseOutQuad<float>(float start, float end, float x, float totalx);

template Vector3 EaseInOutQuad<Vector3>(Vector3 start, Vector3 end, float x, float totalx);
template Vector2 EaseInOutQuad<Vector2>(Vector2 start, Vector2 end, float x, float totalx);
template float EaseInOutQuad<float>(float start, float end, float x, float totalx);

template Vector3 EaseInQuart<Vector3>(Vector3 start, Vector3 end, float x, float totalx);
template Vector2 EaseInQuart<Vector2>(Vector2 start, Vector2 end, float x, float totalx);
template float EaseInQuart<float>(float start, float end, float x, float totalx);

template Vector3 EaseOutQuart<Vector3>(Vector3 start, Vector3 end, float x, float totalx);
template Vector2 EaseOutQuart<Vector2>(Vector2 start, Vector2 end, float x, float totalx);
template float EaseOutQuart<float>(float start, float end, float x, float totalx);

template Vector3 EaseInBounce<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInBounce<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInBounce<float>(float start, float end, float x, float totalX);

template Vector3 EaseOutBounce<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseOutBounce<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseOutBounce<float>(float start, float end, float x, float totalX);

template Vector3 EaseInOutBounce<Vector3>(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInOutBounce<Vector2>(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInOutBounce<float>(float start, float end, float x, float totalX);

template Vector3 EaseInBack(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInBack(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInBack(float start, float end, float x, float totalX);

template Vector3 EaseOutBack(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseOutBack(Vector2 start, Vector2 end, float x, float totalX);
template float EaseOutBack(float start, float end, float x, float totalX);

template Vector3 EaseInOutBack(Vector3 start, Vector3 end, float x, float totalX);
template Vector2 EaseInOutBack(Vector2 start, Vector2 end, float x, float totalX);
template float EaseInOutBack(float start, float end, float x, float totalX);

namespace Back {

	template float InSineZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InSineZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito);
	template Vector3 InSineZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito);

	template float OutSineZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 OutSineZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 OutSineZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InOutSineZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InOutSineZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InOutSineZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InQuadZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InQuadZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InQuadZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float OutQuadZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 OutQuadZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 OutQuadZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InOutQuadZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InOutQuadZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InOutQuadZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InCubicZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InCubicZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InCubicZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float OutCubicZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 OutCubicZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 OutCubicZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InOutCubicZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InOutCubicZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InOutCubicZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InQuartZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InQuartZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InQuartZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float OutQuartZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 OutQuartZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 OutQuartZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InOutQuartZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InOutQuartZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InOutQuartZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InQuintZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InQuintZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InQuintZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float OutQuintZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 OutQuintZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 OutQuintZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InOutQuintZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InOutQuintZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InOutQuintZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InExpoZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InExpoZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InExpoZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float OutExpoZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 OutExpoZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 OutExpoZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InOutExpoZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InOutExpoZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InOutExpoZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InCircZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InCircZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InCircZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float OutCircZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 OutCircZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 OutCircZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InOutCircZero(float start, float end, float t, float totalTime, float backRaito);
	template Vector2 InOutCircZero(Vector2 start, Vector2 end, float t, float totalTime, float backRaito); 
	template Vector3 InOutCircZero(Vector3 start, Vector3 end, float t, float totalTime, float backRaito); 

	template float InBackZero(float start, float end, float t, float totalTime, float s, float backRaito);
	template Vector2 InBackZero(Vector2 start, Vector2 end, float t, float totalTime, float s, float backRaito); 
	template Vector3 InBackZero(Vector3 start, Vector3 end, float t, float totalTime, float s, float backRaito); 

	template float OutBackZero(float start, float end, float t, float totalTime, float s, float backRaito);
	template Vector2 OutBackZero(Vector2 start, Vector2 end, float t, float totalTime, float s, float backRaito); 
	template Vector3 OutBackZero(Vector3 start, Vector3 end, float t, float totalTime, float s, float backRaito); 

	template float InOutBackZero(float start, float end, float t, float totalTime, float s, float backRaito);
	template Vector2 InOutBackZero(Vector2 start, Vector2 end, float t, float totalTime, float s, float backRaito); 
	template Vector3 InOutBackZero(Vector3 start, Vector3 end, float t, float totalTime, float s, float backRaito); 


}