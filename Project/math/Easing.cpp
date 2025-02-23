
#include "Easing.h"
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
namespace Back {



	template<typename T>
	T OutSineZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;


		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			return Lerp(min, max, sinf((t / backPoint) * std::numbers::pi_v<float> *0.5f)); // OutSine from min to max
		} else
		{
			return Lerp(max, min, 1.0f - cosf(((t - backPoint) / (totaltime - backPoint)) * std::numbers::pi_v<float> *0.5f)); // InSine from max to min
		}
	}

	template<typename T>
	T InOutSineZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			return Lerp(min, max, -0.5f * (cosf((t / backPoint) * std::numbers::pi_v<float>) - 1.0f)); // InOutSine from min to max
		} else
		{
			return Lerp(max, min, -0.5f * (cosf(((t - backPoint) / (totaltime - backPoint)) * std::numbers::pi_v<float>) - 1.0f)); // InOutSine from max to min
		}
	}

	template<typename T>
	T InQuadZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint;
			return Lerp(min, max, u * u); // InQuad from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint);
			return Lerp(max, min, -u * (u - 2.0f)); // OutQuad from max to min
		}
	}

	template<typename T>
	T OutQuadZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint;
			return Lerp(min, max, -u * (u - 2.0f)); // OutQuad from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint);
			return Lerp(max, min, u * u); // InQuad from max to min
		}
	}

	template<typename T>
	T InOutQuadZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / (backPoint * 0.5f);
			if (u < 1.0f) return Lerp(min, max, 0.5f * u * u);
			u--;
			return Lerp(min, max, -0.5f * (u * (u - 2.0f) - 1.0f)); // InOutQuad from min to max
		} else
		{
			float u = (t - backPoint) / ((totaltime - backPoint) * 0.5f);
			if (u < 1.0f) return Lerp(max, min, 0.5f * u * u);
			u--;
			return Lerp(max, min, -0.5f * (u * (u - 2.0f) - 1.0f)); // InOutQuad from max to min
		}
	}

	template<typename T>
	T InCubicZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint;
			return Lerp(min, max, u * u * u); // InCubic from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint) - 1.0f;
			return Lerp(max, min, u * u * u + 1.0f); // OutCubic from max to min
		}
	}

	template<typename T>
	T OutCubicZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint - 1.0f;
			return Lerp(min, max, u * u * u + 1.0f); // OutCubic from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint);
			return Lerp(max, min, u * u * u); // InCubic from max to min
		}
	}

	template<typename T>
	T InOutCubicZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / (backPoint * 0.5f);
			if (u < 1.0f) return Lerp(min, max, 0.5f * u * u * u);
			u -= 2.0f;
			return Lerp(min, max, 0.5f * (u * u * u + 2.0f)); // InOutCubic from min to max
		} else
		{
			float u = (t - backPoint) / ((totaltime - backPoint) * 0.5f);
			if (u < 1.0f) return Lerp(max, min, 0.5f * u * u * u);
			u -= 2.0f;
			return Lerp(max, min, 0.5f * (u * u * u + 2.0f)); // InOutCubic from max to min
		}
	}

	template<typename T>
	T InQuartZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint;
			return Lerp(min, max, u * u * u * u); // InQuart from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint) - 1.0f;
			return Lerp(max, min, -(u * u * u * u - 1.0f)); // OutQuart from max to min
		}
	}

	template<typename T>
	T OutQuartZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint - 1.0f;
			return Lerp(min, max, -(u * u * u * u - 1.0f)); // OutQuart from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint);
			return Lerp(max, min, u * u * u * u); // InQuart from max to min
		}
	}

	template<typename T>
	T InOutQuartZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / (backPoint * 0.5f);
			if (u < 1.0f) return Lerp(min, max, 0.5f * u * u * u * u);
			u -= 2.0f;
			return Lerp(min, max, -0.5f * (u * u * u * u - 2.0f)); // InOutQuart from min to max
		} else
		{
			float u = (t - backPoint) / ((totaltime - backPoint) * 0.5f);
			if (u < 1.0f) return Lerp(max, min, 0.5f * u * u * u * u);
			u -= 2.0f;
			return Lerp(max, min, -0.5f * (u * u * u * u - 2.0f)); // InOutQuart from max to min
		}
	}

	template<typename T>
	T InQuintZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint;
			return Lerp(min, max, u * u * u * u * u); // InQuint from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint) - 1.0f;
			return Lerp(max, min, u * u * u * u * u + 1.0f); // OutQuint from max to min
		}
	}

	template<typename T>
	T OutQuintZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint - 1.0f;
			return Lerp(min, max, u * u * u * u * u + 1.0f); // OutQuint from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint);
			return Lerp(max, min, u * u * u * u * u); // InQuint from max to min
		}
	}

	template<typename T>
	T InOutQuintZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / (backPoint * 0.5f);
			if (u < 1.0f) return Lerp(min, max, 0.5f * u * u * u * u * u);
			u -= 2.0f;
			return Lerp(min, max, 0.5f * (u * u * u * u * u + 2.0f)); // InOutQuint from min to max
		} else
		{
			float u = (t - backPoint) / ((totaltime - backPoint) * 0.5f);
			if (u < 1.0f) return Lerp(max, min, 0.5f * u * u * u * u * u);
			u -= 2.0f;
			return Lerp(max, min, 0.5f * (u * u * u * u * u + 2.0f)); // InOutQuint from max to min
		}
	}

	template<typename T>
	T InExpoZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			return Lerp(min, max, (powf(2.0f, 10.0f * (t / backPoint - 1.0f)) - 0.001f)); // InExpo from min to max
		} else
		{
			return Lerp(max, min, (1.0f - powf(2.0f, -10.0f * ((t - backPoint) / (totaltime - backPoint))))); // OutExpo from max to min
		}
	}

	template<typename T>
	T OutExpoZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			return Lerp(min, max, (1.0f - powf(2.0f, -10.0f * (t / backPoint)))); // OutExpo from min to max
		} else
		{
			return Lerp(max, min, (powf(2.0f, 10.0f * ((t - backPoint) / (totaltime - backPoint) - 1.0f)))); // InExpo from max to min
		}
	}

	template<typename T>
	T InOutExpoZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / (backPoint * 0.5f);
			if (u < 1.0f) return Lerp(min, max, 0.5f * powf(2.0f, 10.0f * (u - 1.0f)));
			u--;
			return Lerp(min, max, 0.5f * (2.0f - powf(2.0f, -10.0f * u))); // InOutExpo from min to max
		} else
		{
			float u = (t - backPoint) / ((totaltime - backPoint) * 0.5f);
			if (u < 1.0f) return Lerp(max, min, 0.5f * powf(2.0f, 10.0f * (u - 1.0f)));
			u--;
			return Lerp(max, min, 0.5f * (2.0f - powf(2.0f, -10.0f * u))); // InOutExpo from max to min
		}
	}

	template<typename T>
	T InCircZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint;
			return Lerp(min, max, -(sqrtf(1.0f - u * u) - 1.0f)); // InCirc from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint) - 1.0f;
			return Lerp(max, min, sqrtf(1.0f - u * u)); // OutCirc from max to min
		}
	}

	template<typename T>
	T OutCircZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint - 1.0f;
			return Lerp(min, max, sqrtf(1.0f - u * u)); // OutCirc from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint);
			return Lerp(max, min, -(sqrtf(1.0f - u * u) - 1.0f)); // InCirc from max to min
		}
	}

	template<typename T>
	T InOutCircZero(float t, float totaltime, const T& min, const T& max, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / (backPoint * 0.5f);
			if (u < 1.0f) return Lerp(min, max, -0.5f * (sqrtf(1.0f - u * u) - 1.0f));
			u -= 2.0f;
			return Lerp(min, max, 0.5f * (sqrtf(1.0f - u * u) + 1.0f)); // InOutCirc from min to max
		} else
		{
			float u = (t - backPoint) / ((totaltime - backPoint) * 0.5f);
			if (u < 1.0f) return Lerp(max, min, -0.5f * (sqrtf(1.0f - u * u) - 1.0f));
			u -= 2.0f;
			return Lerp(max, min, 0.5f * (sqrtf(1.0f - u * u) + 1.0f)); // InOutCirc from max to min
		}
	}

	template<typename T>
	T InBackZero(float t, float totaltime, const T& min, const T& max, float s, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint;
			return Lerp(min, max, u * u * ((s + 1.0f) * u - s)); // InBack from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint) - 1.0f;
			return Lerp(max, min, u * u * ((s + 1.0f) * u + s) + 1.0f); // OutBack from max to min
		}
	}

	template<typename T>
	T OutBackZero(float t, float totaltime, const T& min, const T& max, float s, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;

		if (t < backPoint)
		{
			float u = t / backPoint - 1.0f;
			return Lerp(min, max, u * u * ((s + 1.0f) * u + s) + 1.0f); // OutBack from min to max
		} else
		{
			float u = (t - backPoint) / (totaltime - backPoint);
			return Lerp(max, min, u * u * ((s + 1.0f) * u - s)); // InBack from max to min
		}
	}

	template<typename T>
	T InOutBackZero(float t, float totaltime, const T& min, const T& max, float s, float backRatio)
	{
		if (t <= 0.0f) return min;
		if (t >= totaltime) return min;
		if (backRatio < 0.0f) backRatio = 0.0f;
		if (backRatio > 1.0f) backRatio = 1.0f;

		float backPoint = totaltime * backRatio;
		float s_modified = s * 1.525f;

		if (t < backPoint)
		{
			float u = t / (backPoint * 0.5f);
			if (u < 1.0f) return Lerp(min, max, 0.5f * (u * u * ((s_modified + 1.0f) * u - s_modified)));
			u -= 2.0f;
			return Lerp(min, max, 0.5f * (u * u * ((s_modified + 1.0f) * u + s_modified) + 2.0f)); // InOutBack from min to max
		} else
		{
			float u = (t - backPoint) / ((totaltime - backPoint) * 0.5f);
			if (u < 1.0f) return Lerp(max, min, 0.5f * (u * u * ((s_modified + 1.0f) * u - s_modified)));
			u -= 2.0f;
			return Lerp(max, min, 0.5f * (u * u * ((s_modified + 1.0f) * u + s_modified) + 2.0f)); // InOutBack from max to min
		}
	}
}

// ぷにぷに
template Vector3 EaseAmplitudeScale<Vector3>(const Vector3& initScale, const float& easeT, const float& easeTime, const float& amplitude, const float& period);
template Vector2 EaseAmplitudeScale<Vector2>(const Vector2& initScale, const float& easeT, const float& easeTime, const float& amplitude, const float& period);
template float EaseAmplitudeScale<float>(const float& initScale, const float& easeT, const float& easeTime, const float& amplitude, const float& period);
//*******************************************************************************************************************************************************************
// Sine**************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************

template Vector3 EaseInSine<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInSine<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInSine<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseOutSine<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseOutSine<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseOutSine<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseInOutSine<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInOutSine<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInOutSine<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseInQuint<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInQuint<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInQuint<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseOutQuint<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseOutQuint<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseOutQuint<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseInOutQuint<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInOutQuint<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInOutQuint<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseInCirc<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInCirc<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInCirc<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseOutCirc<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseOutCirc<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseOutCirc<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseInOutCirc<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInOutCirc<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInOutCirc<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseInExpo<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInExpo<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInExpo<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseOutExpo<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseOutExpo<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseOutExpo<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseInOutExpo<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInOutExpo<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInOutExpo<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseOutCubic<Vector3>(const Vector3& start, const Vector3& end, float x, float totalx);
template Vector2 EaseOutCubic<Vector2>(const Vector2& start, const Vector2& end, float x, float totalx);
template float EaseOutCubic<float>(const float& start, const float& end, float x, float totalx);

template Vector3 EaseInCubic<Vector3>(const Vector3& start, const Vector3& end, float x, float totalx);
template Vector2 EaseInCubic<Vector2>(const Vector2& start, const Vector2& end, float x, float totalx);
template float EaseInCubic<float>(const float& start, const float& end, float x, float totalx);

template Vector3 EaseInOutCubic<Vector3>(const Vector3& start, const Vector3& end, float x, float totalx);
template Vector2 EaseInOutCubic<Vector2>(const Vector2& start, const Vector2& end, float x, float totalx);
template float EaseInOutCubic<float>(const float& start, const float& end, float x, float totalx);

template Vector3 EaseInQuad<Vector3>(const Vector3& start, const Vector3& end, float x, float totalx);
template Vector2 EaseInQuad<Vector2>(const Vector2& start, const Vector2& end, float x, float totalx);
template float EaseInQuad<float>(const float& start, const float& end, float x, float totalx);

template Vector3 EaseOutQuad<Vector3>(const Vector3& start, const Vector3& end, float x, float totalx);
template Vector2 EaseOutQuad<Vector2>(const Vector2& start, const Vector2& end, float x, float totalx);
template float EaseOutQuad<float>(const float& start, const float& end, float x, float totalx);

template Vector3 EaseInOutQuad<Vector3>(const Vector3& start, const Vector3& end, float x, float totalx);
template Vector2 EaseInOutQuad<Vector2>(const Vector2& start, const Vector2& end, float x, float totalx);
template float EaseInOutQuad<float>(const float& start, const float& end, float x, float totalx);

template Vector3 EaseInQuart<Vector3>(const Vector3& start, const Vector3& end, float x, float totalx);
template Vector2 EaseInQuart<Vector2>(const Vector2& start, const Vector2& end, float x, float totalx);
template float EaseInQuart<float>(const float& start, const float& end, float x, float totalx);

template Vector3 EaseOutQuart<Vector3>(const Vector3& start, const Vector3& end, float x, float totalx);
template Vector2 EaseOutQuart<Vector2>(const Vector2& start, const Vector2& end, float x, float totalx);
template float EaseOutQuart<float>(const float& start, const float& end, float x, float totalx);

// バウンス補助関数
float BounceEaseOut(float x);

template Vector3 EaseInBounce<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInBounce<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInBounce<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseOutBounce<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseOutBounce<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseOutBounce<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseInOutBounce<Vector3>(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInOutBounce<Vector2>(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInOutBounce<float>(const float& start, const float& end, float x, float totalX);

template Vector3 EaseInBack(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInBack(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInBack(const float& start, const float& end, float x, float totalX);

template Vector3 EaseOutBack(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseOutBack(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseOutBack(const float& start, const float& end, float x, float totalX);

template Vector3 EaseInOutBack(const Vector3& start, const Vector3& end, float x, float totalX);
template Vector2 EaseInOutBack(const Vector2& start, const Vector2& end, float x, float totalX);
template float EaseInOutBack(const float& start, const float& end, float x, float totalX);

namespace Back {

	template float InSineZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InSineZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito);
	template Vector3 InSineZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito);

	template float OutSineZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 OutSineZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 OutSineZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InOutSineZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InOutSineZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InOutSineZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InQuadZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InQuadZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InQuadZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float OutQuadZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 OutQuadZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 OutQuadZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InOutQuadZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InOutQuadZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InOutQuadZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InCubicZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InCubicZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InCubicZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float OutCubicZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 OutCubicZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 OutCubicZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InOutCubicZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InOutCubicZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InOutCubicZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InQuartZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InQuartZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InQuartZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float OutQuartZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 OutQuartZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 OutQuartZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InOutQuartZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InOutQuartZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InOutQuartZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InQuintZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InQuintZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InQuintZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float OutQuintZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 OutQuintZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 OutQuintZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InOutQuintZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InOutQuintZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InOutQuintZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InExpoZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InExpoZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InExpoZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float OutExpoZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 OutExpoZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 OutExpoZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InOutExpoZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InOutExpoZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InOutExpoZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InCircZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InCircZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InCircZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float OutCircZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 OutCircZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 OutCircZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InOutCircZero(const float& start, const float& end, float t, float totaltime, float backRaito);
	template Vector2 InOutCircZero(const Vector2& start, const Vector2& end, float t, float totaltime, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InOutCircZero(const Vector3& start, const Vector3& end, float t, float totaltime, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InBackZero(const float& start, const float& end, float t, float totaltime, float s, float backRaito);
	template Vector2 InBackZero(const Vector2& start, const Vector2& end, float t, float totaltime, float s, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InBackZero(const Vector3& start, const Vector3& end, float t, float totaltime, float s, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float OutBackZero(const float& start, const float& end, float t, float totaltime, float s, float backRaito);
	template Vector2 OutBackZero(const Vector2& start, const Vector2& end, float t, float totaltime, float s, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 OutBackZero(const Vector3& start, const Vector3& end, float t, float totaltime, float s, float backRaito); //Vector3に対する特殊化（必要に応じて）

	template float InOutBackZero(const float& start, const float& end, float t, float totaltime, float s, float backRaito);
	template Vector2 InOutBackZero(const Vector2& start, const Vector2& end, float t, float totaltime, float s, float backRaito); // Vector2型に対する特殊化（必要に応じて）
	template Vector3 InOutBackZero(const Vector3& start, const Vector3& end, float t, float totaltime, float s, float backRaito); //Vector3に対する特殊化（必要に応じて）


}