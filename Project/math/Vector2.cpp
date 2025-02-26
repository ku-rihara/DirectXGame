// Vector2.cpp
#include "Vector2.h"
#include <math.h>

//内積
float Vector2::Dot(const Vector2& v1, const Vector2& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

//正規化
Vector2 Vector2::Normalize(const Vector2& v) {
	Vector2 result;

	float length = sqrtf(v.x * v.x + v.y * v.y);
	if (length != 0) {
		result.x = v.x / length;
		result.y = v.y / length;
	} else {
		result.x = 0;
		result.y = 0;
	}
	return result;
}

//スカラー倍
Vector2 Vector2::Multiply(const Vector2& v1, const float& v2) {
	Vector2 result;
	result.x = v1.x * v2;
	result.y = v1.y * v2;
	return result;
}

// 長さを取得する
float Vector2::Length() const {
	return std::sqrt(x * x + y * y);
}

//正規化
Vector2 Vector2::Normalize() const {
	Vector2 result;

	float length = sqrtf(x * x + y * y);
	if (length != 0) {
		result.x = x / length;
		result.y = y / length;
	} else {
		result.x = 0;
		result.y = 0;
	}
	return result;
}

// ゼロベクトル
Vector2 Vector2::ZeroVector() {
	return Vector2(0, 0);
}

// 単位ベクトル
Vector2 Vector2::UnitVector() {
	return Vector2(1.0f, 1.0f);
}