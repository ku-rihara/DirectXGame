#pragma once
#include"Vector3.h"
#include"Vector2.h"

float Lerp(const float& start, const float& end, float t);

Vector3 Lerp(const Vector3& start, const Vector3& end, float t);

Vector2 Lerp(const Vector2& start, const Vector2& end, float t);

Vector3 SLerp(const Vector3& start, const Vector3& end, float t);

float InElasticAmplitude( float t, const float& totaltime, const float& amplitude, const float& period);

float OutElasticAmplitude(float t, float totaltime, float amplitude, float period);

float InOutElasticAmplitude(float t, float totaltime, float amplitude, float period);

template<typename T> T EaseAmplitudeScale(const T& initScale, const float& easeT, const float& totalTime, const float& amplitude, const float& period);