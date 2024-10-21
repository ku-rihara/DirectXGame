#pragma once

#include <cmath>
#include "Vector3.h"

/// <summary>
/// クォータニオン
/// </summary>
class Quaternion final {
public:

	Quaternion();
	Quaternion(float x, float y, float z, float w);

	float x;
	float y;
	float z;
	float w;

	static float Lenght(const Quaternion& q);
	static Quaternion Normalize(const Quaternion& q);
	static Vector3 Transform(const Vector3& v, const Quaternion& q);
	static Quaternion MakeFromAxis(const Vector3& axis, float theta);
	static Quaternion LockAt(const Vector3& position, const Vector3& target, const Vector3& up);
	// ドット積の計算
	static float Dot(const Quaternion& q1, const Quaternion& q2);
	// クロス積の計算
	static Vector3 Cross(const Quaternion& q1, const Quaternion& q2);

	// クォータニオンの共役
	Quaternion Conjugate() const;
	// クォータニオンのノルム
	float Lenght() const;
	// 逆クォータニオンを計算する関数
	Quaternion Inverse() const;

	Quaternion& operator*= (const Quaternion& other);

};

///==========================================================================
/// 演算子オーバーロード
/// =========================================================================


inline Quaternion operator* (const Quaternion& q1, const Quaternion& q2) {
	return {
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
	};
}

inline Quaternion operator/ (const Quaternion& q, float value) {
	Quaternion result;
	result.w = q.w / value;
	result.x = q.x / value;
	result.y = q.y / value;
	result.z = q.z / value;
	return result;
}


inline Quaternion& Quaternion::operator*=(const Quaternion& other) {
	*this = *this * other;
	return *this;
}

inline Quaternion operator- (const Quaternion& q1, const Quaternion& q2) {
	return {
		q1.x - q2.x,
		q1.y - q2.y,
		q1.z - q2.z,
		q1.w - q2.w
	};
}

inline Quaternion operator+ (const Quaternion& q1, const Quaternion& q2) {
	return {
		q1.x + q2.x,
		q1.y + q2.y,
		q1.z + q2.z,
		q1.w + q2.w
	};
}
inline Quaternion operator* (const Quaternion& q, float scalar) {
	return {
		q.x * scalar,
		q.y * scalar,
		q.z * scalar,
		q.w * scalar
	};
}