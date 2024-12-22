#pragma once
#include <cmath>
#include"Matrix4x4.h"
struct Matrix4x4;
class Vector3 {
public:
	float x;
	float y;
	float z;

	// コンストラクタ
	Vector3() : x(0), y(0), z(0) {}
	Vector3(float numX, float numY, float numZ) : x(numX), y(numY), z(numZ) {}

	// 演算子のオーバーロードをinline化
	inline Vector3 operator+(const Vector3& obj) const {
		return { x + obj.x, y + obj.y, z + obj.z };
	}

	inline Vector3 operator-(const Vector3& obj) const {
		return { x - obj.x, y - obj.y, z - obj.z };
	}

	inline Vector3 operator*(const Vector3& obj) const {
		return { x * obj.x, y * obj.y, z * obj.z };
	}

	inline Vector3 operator*(const float& scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	Vector3 operator*(const Matrix4x4& obj) const;

	inline Vector3 operator/(const Vector3& obj) const {
		return { x / obj.x, y / obj.y, z / obj.z };
	}

	inline Vector3 operator/(const float& scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}

	inline void operator+=(const Vector3& obj) {
		x += obj.x; y += obj.y; z += obj.z;
	}

	inline void operator+=(const float& scalar) {
		x += scalar; y += scalar; z += scalar;
	}

	inline void operator-=(const Vector3& obj) {
		x -= obj.x; y -= obj.y; z -= obj.z;
	}

	friend inline bool operator!=(const Vector3& lhs, const Vector3& rhs) {
		return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
	}


	// 静的メソッドの宣言
	static Vector3 Normalize(const Vector3& v);
	static Vector3 Multiply(const Vector3& v1, const float& v2);
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);
	static float Dot(const Vector3& v1, const Vector3& v2);
	/*static float Length(const Vector3& v);*/

	static Vector3 ToUp();       // 上方向
	static Vector3 ToForward(); // 前方向
	static Vector3 ToRight();   // 右方向
	static Vector3 Zero();

	void Normalize();
	float Length()const;
};

// スカラーとベクトルの掛け算をフレンドとして定義
inline Vector3 operator*(const float& scalar, const Vector3& vec) {
	return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
}
