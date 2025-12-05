#pragma once
#include "Vector2.h"
#include "Vector3.h"


float EaseInElasticAmplitude(float t, float totaltime, float amplitude, float period);
float EaseOutElasticAmplitude(float t, float totalTime, float amplitude, float period);
float EaseInOutElasticAmplitude(float t, float totalTime, float amplitude, float period);

template<typename T> T EaseAmplitudeScale(T initScale, float easeT, float totalTime, float amplitude, float period);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Sine
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename T> T EaseInSine(T start, T end, float x, float totalX);
template<typename T> T EaseOutSine(T start, T end, float x, float totalX);
template<typename T> T EaseInOutSine(T start, T end, float x, float totalX);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Quint
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename T> T EaseInQuint(T start, T end, float x, float totalX);
template<typename T> T EaseOutQuint(T start, T end, float x, float totalX);
template<typename T> T EaseInOutQuint(T start, T end, float x, float totalX);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Circ
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename T> T EaseInCirc(T start, T end, float x, float totalX);
template<typename T> T EaseOutCirc(T start, T end, float x, float totalX);
template<typename T> T EaseInOutCirc(T start, T end, float x, float totalX);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Expo
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename T> T EaseInExpo(T start, T end, float x, float totalX);
template<typename T> T EaseOutExpo(T start, T end, float x, float totalX);
template<typename T> T EaseInOutExpo(T start, T end, float x, float totalX);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Cubic
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename T> T EaseOutCubic(T start, T end, float x, float totalx);
template<typename T> T EaseInCubic(T start, T end, float x, float totalx);
template<typename T> T EaseInOutCubic(T start, T end, float x, float totalx);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Quad
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename T> T EaseInQuad(T start, T end, float x, float totalx);
template<typename T> T EaseOutQuad(T start, T end, float x, float totalx);
template<typename T> T EaseInOutQuad(T start, T end, float x, float totalx);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Quart
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename T> T EaseInQuart(T start, T end, float x, float totalx);
template<typename T> T EaseOutQuart(T start, T end, float x, float totalx);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// back
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------


template<typename T> T EaseInBack(T start, T end, float x, float totalX);
template<typename T> T EaseOutBack(T start, T end, float x, float totalX);
template<typename T> T EaseInOutBack(T start, T end, float x, float totalX);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Bounce
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

// バウンス補助関数
float BounceEaseOut(float x);

template<typename T> T EaseInBounce(T start, T end, float x, float totalX);
template<typename T> T EaseOutBounce(T start, T end, float x, float totalX);
template<typename T> T EaseInOutBounce(T start, T end, float x, float totalX);

namespace Back {

	template<typename T> T  InSineZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T OutSineZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InOutSineZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InQuadZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T OutQuadZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InOutQuadZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InCubicZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T OutCubicZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InOutCubicZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InQuartZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T OutQuartZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InOutQuartZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InQuintZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T OutQuintZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InOutQuintZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InExpoZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T OutExpoZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InOutExpoZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InCircZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T OutCircZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InOutCircZero(T start, T end, float t, float totalTime, float backRaito);

	template<typename T> T InBackZero(T start, T end, float t, float totalTime, float s, float backRaito);

	template<typename T> T OutBackZero(T start, T end, float t, float totalTime, float s, float backRaito);

	template<typename T> T InOutBackZero(T start, T end, float t, float totalTime, float s, float backRaito);


}