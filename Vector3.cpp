
#include "Vector3.h"
#include<math.h>


const float kColumnWidth = 60;




Vector3 Vector3:: operator+(const Vector3& obj)  const {

	Vector3 result;

	result.x = x + obj.x;
	result.y = y + obj.y;
	result.z = z + obj.z;

	return result;

}

Vector3 Vector3:: operator-(const Vector3& obj) const{

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

Vector3 Vector3:: operator*(const float& obj)  const {
	Vector3 result;

	result.x = x * obj;
	result.y = y * obj;
	result.z = z * obj;
	
	return result;
}

Vector3 Vector3:: operator/(const Vector3& obj)  const {

	Vector3 result;

	if (obj.x != 0) {
		result.x = x / obj.x;

	}
	else {
		result.x = 0;

	}

	if (obj.y != 0) {
		result.y = y / obj.y;

	}
	else {
		result.y = 0;

	}

	if (obj.z != 0) {
		result.z = z / obj.z;

	}
	else {
		result.z = 0;

	}

	return result;

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
Vector3 Normnalize(const Vector3& v) {
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
