#include "Vector3.h"
#include<math.h>


const float kColumnWidth = 60;

//内積
float Vector3::Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}



//正規化
Vector3 Vector3::Normalize(const Vector3& v){
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

Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;

	result.x = (v1.y * v2.z) - (v1.z * v2.y);
	result.y = (v1.z * v2.x) - (v1.x * v2.z);
	result.z = (v1.x * v2.y) - (v1.y * v2.x);
	return result;
}

Vector3 Vector3::Multiply(const Vector3& v1, const float& v2) {
	Vector3 result;
	result.x = v1.x * v2;
	result.y = v1.y * v2;
	result.z = v1.z * v2;
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


// 上方向の単位ベクトルを返す
Vector3 Vector3::ToUp() {
	return Vector3(0.0f, 1.0f, 0.0f);
}

// 前方向の単位ベクトルを返す
Vector3 Vector3::ToForward() {
	return Vector3(0.0f, 0.0f, 1.0f);
}

// 右方向の単位ベクトルを返す
Vector3 Vector3::ToRight() {
	return Vector3(1.0f, 0.0f, 0.0f);
}

// ゼロベクトル
Vector3 Vector3::ZeroVector() {
	return Vector3(0, 0, 0);
}

// 単位ベクトル
Vector3  Vector3::UnitVector() {
	return Vector3(1.0f, 1.0f, 1.0f);
}