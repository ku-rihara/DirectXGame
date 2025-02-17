#include"MathFunction.h"
///std
#include<numbers>
#include <cmath>



float EaseInElasticAmplitude(float t, const float& totaltime, const float& amplitude, const float& period) {

	if (t <= 0.0f) {
		return 0.0f;
	}
	if (t >= totaltime) {
		return 0.0f;
	}
	float s = period / (2.0f * std::numbers::pi_v<float>) * std::asinf(1.0f);
	t /= totaltime;

	return -amplitude * std::powf(2.0f, 10.0f * (t - 1.0f)) * std::sinf((t - 1.0f - s) * (2.0f * std::numbers::pi_v<float>) / period);
}

float EaseOutElasticAmplitude(float t, float totaltime, float amplitude, float period) {
	if (t <= 0.0f)
		return 0.0f;
	if (t >= totaltime)
		return 0.0f;

	float s = period / (2.0f * std::numbers::pi_v<float>) * std::asin(1.0f);
	t /= totaltime;

	return amplitude * std::pow(2.0f, -10.0f * t) * std::sin((t - s) * (2.0f * std::numbers::pi_v<float>) / period);
}

float EaseInOutElasticAmplitude(float t, float totaltime, float amplitude, float period) {
	if (t <= 0.0f)
		return 0.0f;
	if (t >= totaltime)
		return 0.0f;

	float backPoint = 0.5f;
	t /= totaltime;

	if (t < backPoint) {
		return EaseOutElasticAmplitude(t, totaltime, amplitude, period);
	} else {

		return EaseInElasticAmplitude(t - backPoint, totaltime - backPoint, amplitude, period);
	}
}

template<typename T> T EaseAmplitudeScale(const T& initScale, const float& easeT, const float& totalTime, const float& amplitude, const float& period) {
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
template<typename T> T EaseInSine(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::cosf((t * std::numbers::pi_v<float>) / 2.0f);
	return Lerp(start, end, easeT);
}

// EaseOutSine 関数
template<typename T> T EaseOutSine(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = std::sinf((t * std::numbers::pi_v<float>) / 2.0f);
	return Lerp(start, end, easeT);
}

// EaseInOutSine 関数
template<typename T> T EaseInOutSine(const T& start, const T& end, float x, float totalX) {
	float t = x / (totalX / 2.0f);
	float easeT = 0.5f * (1.0f - std::cosf(t * std::numbers::pi_v<float>));
	return Lerp(start, end, easeT);
}

// EaseInQuint 関数
template<typename T> T EaseInQuint(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t * t * t * t;
	return Lerp(start, end, easeT);
}

// EaseOutQuint 関数
template<typename T> T EaseOutQuint(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::powf(1.0f - t, 5);
	return Lerp(start, end, easeT);
}

// EaseInOutQuint 関数
template<typename T> T EaseInOutQuint(const T& start, const T& end, float x, float totalX) {
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
template<typename T> T EaseInCirc(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::sqrtf(1.0f - std::powf(t, 2));
	return Lerp(start, end, easeT);
}

// EaseOutCirc 関数
template<typename T> T EaseOutCirc(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = std::sqrtf(1.0f - std::powf(t - 1.0f, 2));
	return Lerp(start, end, easeT);
}

// EaseInOutCirc 関数
template<typename T> T EaseInOutCirc(const T& start, const T& end, float x, float totalX) {
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
template<typename T> T EaseInExpo(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = (t == 0.0f) ? 0.0f : std::powf(2.0f, 10.0f * (t - 1.0f));
	return Lerp(start, end, easeT);
}

// EaseOutExpo 関数
template<typename T> T EaseOutExpo(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = (t == 1.0f) ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * t);
	return Lerp(start, end, easeT);
}

// EaseInOutExpo 関数
template<typename T> T EaseInOutExpo(const T& start, const T& end, float x, float totalX) {
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
template<typename T> T EaseInCubic(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t * t;
	return Lerp(start, end, easeT);
}

// EaseOutCubic 関数
template<typename T> T EaseOutCubic(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::powf(1.0f - t, 3);
	return Lerp(start, end, easeT);
}

// EaseInOutCubic 関数
template<typename T> T EaseInOutCubic(const T& start, const T& end, float x, float totalX) {
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
template<typename T> T EaseInQuad(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t;
	return Lerp(start, end, easeT);
}

// EaseOutQuad 関数
template<typename T> T EaseOutQuad(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - (1.0f - t) * (1.0f - t);
	return Lerp(start, end, easeT);
}

// EaseInOutQuad 関数
template<typename T> T EaseInOutQuad(const T& start, const T& end, float x, float totalX) {
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
template<typename T> T EaseInQuart(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = t * t * t * t;
	return Lerp(start, end, easeT);
}

// EaseOutQuart 関数
template<typename T> T EaseOutQuart(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = 1.0f - std::powf(1.0f - t, 4);
	return Lerp(start, end, easeT);
}

// EaseInOutQuart 関数
template<typename T> T EaseInOutQuart(const T& start, const T& end, float x, float totalX) {
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
T EaseInBack(const T& start, const T& end, float x, float totalX) {
	const float s = 1.70158f;
	float t = x / totalX;
	float easeT = t * t * ((s + 1) * t - s);
	return Lerp(start, end, easeT);
}

/// EaseOutBack
template<typename T>
T EaseOutBack(const T& start, const T& end, float x, float totalX) {
	const float s = 1.70158f;
	float t = x / totalX - 1;
	float easeT = (t * t * ((s + 1) * t + s)) + 1;
	return Lerp(start, end, easeT);
}


/// EaseInOutBack
template<typename T>
T EaseInOutBack(const T& start, const T& end, float x, float totalX) {
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
template<typename T> T EaseInBounce(const T& start, const T& end, float x, float totalX) {
	float t = 1.0f - (x / totalX);
	float easeT = 1.0f - BounceEaseOut(t);
	return Lerp(start, end, easeT);
}

// EaseOutBounce 関数
template<typename T> T EaseOutBounce(const T& start, const T& end, float x, float totalX) {
	float t = x / totalX;
	float easeT = BounceEaseOut(t);
	return Lerp(start, end, easeT);
}

// EaseInOutBounce 関数
template<typename T> T EaseInOutBounce(const T& start, const T& end, float x, float totalX) {
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


// #define _USE_MATH_DEFINES // M_PI を使う場合、必要な定義 (コンパイラによっては必要。std::numbers::pi_vを使う場合は不要)

namespace Back {

	template<typename T> T  InSineZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		float delta = end - start;
		return delta * (1 - cosf(t * std::numbers::pi_v<float> *0.5f) * (1 - b) + b * sinf(t * std::numbers::pi_v<float> *0.5f)) + start;
	}

	template<typename T> T OutSineZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		float delta = end - start;
		return delta * sinf(t * std::numbers::pi_v<float> *0.5f) * (1 - b) + delta * b * (1 - cosf(t * std::numbers::pi_v<float> *0.5f)) + start;
	}

	template<typename T> T InOutSineZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime * 0.5f;
		if (t < 1)
		{
			float delta = end - start;
			return delta * 0.5f * (1 - cosf(t * std::numbers::pi_v<float> *0.5f) * (1 - b) + b * sinf(t * std::numbers::pi_v<float> *0.5f)) + start;
		} else
		{
			t--;
			float delta = end - start;
			return delta * 0.5f * (sinf(t * std::numbers::pi_v<float> *0.5f) * (1 - b) + b * (1 - cosf(t * std::numbers::pi_v<float> *0.5f))) + delta * 0.5f + start;
		}
	}

	template<typename T> T InQuadZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		float delta = end - start;
		return delta * t * t * (1 - b) + delta * b * t + start;
	}

	template<typename T> T OutQuadZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		float delta = end - start;
		return -delta * t * (t - 2) * (1 - b) + delta * b * t + start;
	}

	template<typename T> T InOutQuadZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime * 0.5f;
		if (t < 1)
		{
			float delta = end - start;
			return delta * 0.5f * t * t * (1 - b) + delta * 0.5f * b * t + start;
		} else
		{
			t--;
			float delta = end - start;
			return -delta * 0.5f * (t * (t - 2) - 1) * (1 - b) + delta * 0.5f * b * (t + 1) + start;
		}
	}

	template<typename T> T InCubicZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		float delta = end - start;
		return delta * t * t * t * (1 - b) + delta * b * t + start;
	}

	template<typename T> T OutCubicZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		t--;
		float delta = end - start;
		return delta * (t * t * t + 1) * (1 - b) + delta * b * (t + 1) + start;
	}

	template<typename T> T InOutCubicZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime * 0.5f;
		if (t < 1)
		{
			float delta = end - start;
			return delta * 0.5f * t * t * t * (1 - b) + delta * 0.5f * b * t + start;
		} else
		{
			t -= 2;
			float delta = end - start;
			return delta * 0.5f * (t * t * t + 2) * (1 - b) + delta * 0.5f * b * (t + 2) + start;
		}
	}

	template<typename T> T InQuartZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		float delta = end - start;
		return delta * t * t * t * t * (1 - b) + delta * b * t + start;
	}

	template<typename T> T OutQuartZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		t--;
		float delta = end - start;
		return -delta * (t * t * t * t - 1) * (1 - b) + delta * b * (t + 1) + start;
	}

	template<typename T> T InOutQuartZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime * 0.5f;
		if (t < 1)
		{
			float delta = end - start;
			return delta * 0.5f * t * t * t * t * (1 - b) + delta * 0.5f * b * t + start;
		} else
		{
			t -= 2;
			float delta = end - start;
			return -delta * 0.5f * (t * t * t * t - 2) * (1 - b) + delta * 0.5f * b * (t + 2) + start;
		}
	}

	template<typename T> T InQuintZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		float delta = end - start;
		return delta * t * t * t * t * t * (1 - b) + delta * b * t + start;
	}

	template<typename T> T OutQuintZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		t--;
		float delta = end - start;
		return delta * (t * t * t * t * t + 1) * (1 - b) + delta * b * (t + 1) + start;
	}

	template<typename T> T InOutQuintZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime * 0.5f;
		if (t < 1)
		{
			float delta = end - start;
			return delta * 0.5f * t * t * t * t * t * (1 - b) + delta * 0.5f * b * t + start;
		} else
		{
			t -= 2;
			float delta = end - start;
			return delta * 0.5f * (t * t * t * t * t + 2) * (1 - b) + delta * 0.5f * b * (t + 2) + start;
		}
	}

	template<typename T> T InExpoZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		float delta = end - start;

		if (t == 0) return start;
		t /= totaltime;
		return delta * powf(2, 10 * (t - 1)) * (1 - b) + delta * b * t + start;
	}

	template<typename T> T OutExpoZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		float delta = end - start;

		if (t == totaltime) return end;
		t /= totaltime;
		return delta * (-powf(2, -10 * t) + 1) * (1 - b) + delta * b * t + start;
	}

	template<typename T> T InOutExpoZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		float delta = end - start;

		if (t == 0) return start;
		if (t == totaltime) return end;

		t /= totaltime * 0.5f;

		if (t < 1)
		{
			return delta * 0.5f * powf(2, 10 * (t - 1)) * (1 - b) + delta * 0.5f * b * t + start;
		}

		t--;

		return delta * 0.5f * (-powf(2, -10 * t) + 2) * (1 - b) + delta * 0.5f * b * (t + 1) + start;
	}

	template<typename T> T InCircZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		float delta = end - start;

		return -delta * (sqrtf(1 - t * t) - 1) * (1 - b) + delta * b * t + start;
	}

	template<typename T> T OutCircZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		t--;
		float delta = end - start;

		return delta * sqrtf(1 - t * t) * (1 - b) + delta * b * (t + 1) + start;
	}

	template<typename T> T InOutCircZero(const T& start, const T& end, float t, float totaltime, float backRaito)
	{
		float b = backRaito;
		t /= totaltime * 0.5f;
		if (t < 1)
		{
			float delta = end - start;
			return -delta * 0.5f * (sqrtf(1 - t * t) - 1) * (1 - b) + delta * 0.5f * b * t + start;
		}

		t -= 2;
		float delta = end - start;
		return delta * 0.5f * (sqrtf(1 - t * t) + 1) * (1 - b) + delta * 0.5f * b * (t + 2) + start;
	}

	template<typename T> T InBackZero(const T& start, const T& end, float t, float totaltime, float s, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		float delta = end - start;
		return delta * t * t * ((s + 1) * t - s) * (1 - b) + delta * b * t + start;
	}

	template<typename T> T OutBackZero(const T& start, const T& end, float t, float totaltime, float s, float backRaito)
	{
		float b = backRaito;
		t /= totaltime;
		t--;
		float delta = end - start;
		return delta * (t * t * ((s + 1) * t + s) + 1) * (1 - b) + delta * b * (t + 1) + start;
	}

	template<typename T> T InOutBackZero(const T& start, const T& end, float t, float totaltime, float s, float backRaito)
	{
		float b = backRaito;
		float s_modified = s * 1.525f; // sを修正
		t /= totaltime * 0.5f;

		if (t < 1)
		{
			float delta = end - start;
			return delta * 0.5f * (t * t * ((s_modified + 1) * t - s_modified)) * (1 - b) + delta * 0.5f * b * t + start;
		}
		t -= 2;
		float delta = end - start;
		return delta * 0.5f * (t * t * ((s_modified + 1) * t + s_modified) + 2) * (1 - b) + delta * 0.5f * b * (t + 2) + start;
	}


}