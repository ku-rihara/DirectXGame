// Vector2.h
#pragma once
#include <cmath>

class Vector2 {
public:
	float x;
	float y;

	// コンストラクタ
	Vector2() : x(0), y(0) {}
	Vector2(float numX, float numY) : x(numX), y(numY) {}

	// 演算子のオーバーロードをinline化
	inline Vector2 operator+(const Vector2& obj) const {
		return { x + obj.x, y + obj.y };
	}

	inline Vector2 operator-(const Vector2& obj) const {
		return { x - obj.x, y - obj.y };
	}

	inline Vector2 operator*(const Vector2& obj) const {
		return { x * obj.x, y * obj.y };
	}

	inline Vector2 operator*(float scalar) const {
		return { x * scalar, y * scalar };
	}

	inline Vector2 operator/(const Vector2& obj) const {
		return { x / obj.x, y / obj.y };
	}

	inline Vector2 operator/(float scalar) const {
		return { x / scalar, y / scalar };
	}

	inline void operator+=(const Vector2& obj) {
		x += obj.x; y += obj.y;
	}

	inline void operator+=(float scalar) {
		x += scalar; y += scalar;
	}

	inline void operator-=(const Vector2& obj) {
		x -= obj.x; y -= obj.y;
	}

	friend inline bool operator!=(const Vector2& lhs, const Vector2& rhs) {
		return lhs.x != rhs.x || lhs.y != rhs.y;
	}

	// 静的メソッドの宣言
	static Vector2 Normalize(const Vector2& v);
	static Vector2 Multiply(const Vector2& v1, float v2);
	static float Dot(const Vector2& v1, const Vector2& v2);

	Vector2 Normalize() const;
	float Length() const;

	static Vector2 OneVector();
	static Vector2 ZeroVector();
};

// スカラーとベクトルの掛け算をフレンドとして定義
inline Vector2 operator*(float scalar, const Vector2& vec) {
	return { vec.x * scalar, vec.y * scalar };
}