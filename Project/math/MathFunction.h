#pragma once
#include"Vector3.h"
#include"Vector2.h"
#include <vector>

class ViewProjection;

float Lerp(const float& start, const float& end, float t);

Vector2 Lerp(const Vector2& start, const Vector2& end, float t);

Vector3 Lerp(const Vector3& start, const Vector3& end, float t);

Vector3 SLerp(const Vector3& start, const Vector3& end, float t);

float Clamp(float n, float min, float max);
size_t Clamp(size_t n, size_t min, size_t max);



Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3 p2, const Vector3& p3, float t);
Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);

float LerpShortAngle(float a, float b, float t);



float ToRadian(float degrees);
float ToDegree(float radians);

// Vector2
Vector2 ToRadian(const Vector2& degrees);
Vector2 ToDegree(const Vector2& radians);

// Vector3
Vector3 ToRadian(const Vector3& degrees);
Vector3 ToDegree(const Vector3& radians);


 Vector3 DirectionToEulerAngles(const Vector3& direction, const ViewProjection& view);
