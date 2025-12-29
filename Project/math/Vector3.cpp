#include "Vector3.h"
#include<math.h>

// 二項演算子
Vector3 Vector3::operator+(const Vector3& obj) const {
    return {x + obj.x, y + obj.y, z + obj.z};
}

Vector3 Vector3::operator-(const Vector3& obj) const {
    return {x - obj.x, y - obj.y, z - obj.z};
}

Vector3 Vector3::operator*(const Vector3& obj) const {
    return {x * obj.x, y * obj.y, z * obj.z};
}

Vector3 Vector3::operator*(float scalar) const {
    return {x * scalar, y * scalar, z * scalar};
}

Vector3 Vector3::operator/(const Vector3& obj) const {
    return {x / obj.x, y / obj.y, z / obj.z};
}

Vector3 Vector3::operator/(float scalar) const {
    return {x / scalar, y / scalar, z / scalar};
}


Vector3 Vector3::operator-() const {
    return {-x, -y, -z};
}

void Vector3::operator+=(const Vector3& obj) {
    x += obj.x;
    y += obj.y;
    z += obj.z;
}

void Vector3::operator+=(float scalar) {
    x += scalar;
    y += scalar;
    z += scalar;
}

void Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
}


void Vector3::operator-=(const Vector3& obj) {
    x -= obj.x;
    y -= obj.y;
    z -= obj.z;
}

// 比較演算子
bool operator!=(const Vector3& lhs, const Vector3& rhs) {
    return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
}

// スカラー,ベクトル
Vector3 operator*(float scalar, const Vector3& vec) {
    return {vec.x * scalar, vec.y * scalar, vec.z * scalar};
}

//内積
float Vector3::Dot(const Vector3& v2) const {
	return x * v2.x + y * v2.y + z * v2.z;
}


Vector3 Vector3::Cross(const Vector3& v2) const {
	Vector3 result;

	result.x = (y * v2.z) - (z * v2.y);
	result.y = (z * v2.x) - (x * v2.z);
	result.z = (x * v2.y) - (y * v2.x);
	return result;
}


Vector3 Vector3::operator*(const Matrix4x4& obj) const {
	Vector3 result;

	result.x = x * obj.m[0][0] + y * obj.m[1][0] + z * obj.m[2][0] + obj.m[3][0];
	result.y = x * obj.m[0][1] + y * obj.m[1][1] + z * obj.m[2][1] + obj.m[3][1];
	result.z = x * obj.m[0][2] + y * obj.m[1][2] + z * obj.m[2][2] + obj.m[3][2];

	return result;

}

// 長さを取得する
float Vector3::Length() const {
	return std::sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::Normalize()const {
	Vector3 result;

	float length = sqrtf(x * x + y * y + z * z);
	if (length != 0) {
		result.x = x / length;
		result.y = y / length;
		result.z = z / length;
	}
	else {
		result.x = 0;
		result.y = 0;
		result.z = 0;
	}
	return result;
}



Vector3 Vector3::ToUp() {
	return Vector3(0.0f, 1.0f, 0.0f);
}

Vector3 Vector3::ToForward() {
	return Vector3(0.0f, 0.0f, 1.0f);
}

Vector3 Vector3::ToRight() {
	return Vector3(1.0f, 0.0f, 0.0f);
}

Vector3 Vector3::ToLeft() {
    return Vector3(-1.0f, 0.0f, 0.0f);
}

Vector3 Vector3::ToBack() {
    return Vector3(0.0f, 0.0f, -1.0f);
}

Vector3 Vector3::ZeroVector() {
	return Vector3(0, 0, 0);
}


Vector3  Vector3::OneVector() {
	return Vector3(1.0f, 1.0f, 1.0f);
}

