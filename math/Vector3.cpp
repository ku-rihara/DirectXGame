#include "Vector3.h"
#include"Matrix4x4.h"
#include<math.h>


const float kColumnWidth = 60;




Vector3 Vector3:: operator+(const Vector3& obj)  const {

	Vector3 result;

	result.x = x + obj.x;
	result.y = y + obj.y;
	result.z = z + obj.z;

	return result;

}

Vector3 Vector3:: operator-(const Vector3& obj) const {

	Vector3 result;

	result.x = x - obj.x;
	result.y = y - obj.y;
	result.z = z - obj.z;

	return result;

}

Vector3 Vector3:: operator*(const Vector3& obj) const {

	Vector3 result;

	result.x = x * obj.x;
	result.y = y * obj.y;
	result.z = z * obj.z;

	return result;

}
Vector3 operator*(const float& scalar, const Vector3& vec) {
	Vector3 result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	result.z = vec.z * scalar;
	return result;
}

Vector3 Vector3:: operator*(const float& obj)  const {
	Vector3 result;

	result.x = x * obj;
	result.y = y * obj;
	result.z = z * obj;

	return result;
}

Vector3 Vector3::operator*(const Matrix4x4& obj) const {
	Vector3 result;

	result.x = x * obj.m[0][0] + y * obj.m[1][0] + z * obj.m[2][0] + obj.m[3][0];
	result.y = x * obj.m[0][1] + y * obj.m[1][1] + z * obj.m[2][1] + obj.m[3][1];
	result.z = x * obj.m[0][2] + y * obj.m[1][2] + z * obj.m[2][2] + obj.m[3][2];

	return result;

};

Vector3 Vector3:: operator/(const Vector3& obj)  const {
	Vector3 result;

		result.x = x / obj.x;
		result.y = y / obj.y;
		result.z = z / obj.z;

	return result;
}

// スカラーでの除算のオーバーロード
Vector3 Vector3::operator/(const float& scalar) const {
	Vector3 result;

		result.x = x / scalar;
		result.y = y / scalar;
		result.z = z / scalar;
	
	return result;
}

void Vector3::operator+=(const Vector3& obj) {

	x += obj.x;
	y += obj.y;
	z += obj.z;

}

void Vector3::operator+=(const float& obj) {

	x += obj;
	y += obj;
	z += obj;
}

void Vector3::operator-=(const Vector3& obj) {

	x -= obj.x;
	y -= obj.y;
	z -= obj.z;
}



//内積
float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

//長さ（ノルム）
float Length(const Vector3& v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

//正規化
Vector3 Normalize(const Vector3& v) {
	Vector3 result;

	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length != 0) {
		result.x = v.x / length;
		result.y = v.y / length;
		result.z = v.z / length;
	}
	else {
		result.x = 0;
		result.y = 0;
		result.z = 0;
	}
	return result;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;

	result.x = (v1.y * v2.z) - (v1.z * v2.y);
	result.y = (v1.z * v2.x) - (v1.x * v2.z);
	result.z = (v1.x * v2.y) - (v1.y * v2.x);
	return result;
}

Vector3 Multiply(const Vector3& v1, const float& v2) {
	Vector3 result;
	result.x = v1.x * v2;
	result.y = v1.y * v2;
	result.z = v1.z * v2;
	return result;
}
