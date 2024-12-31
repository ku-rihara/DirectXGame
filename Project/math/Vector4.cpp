#include"Vector4.h"
#include<cmath>

//長さ（ノルム）
float Vector4::Length()const {
	return sqrtf(x * x + y * y + z * z + w * w);
}

//正規化
Vector4 Vector4::Normalize(const Vector4& v) {
	Vector4 result;

	float length = v.Length();
	if (length != 0) {
		result.x = v.x / length;
		result.y = v.y / length;
		result.z = v.z / length;
		result.w = v.w / length;
	}
	else {
		result.x = 0;
		result.y = 0;
		result.z = 0;
		result.w = 0;
	}
	return result;
}

Vector4  Vector4::Normalize()const {
	Vector4 result;

	float length = sqrtf(x * x + y * y + z * z+w*w);
	if (length != 0) {
		result.x = x / length;
		result.y = y / length;
		result.z = z / length;
		result.w = w / length;
	}
	else {
		result.x = 0;
		result.y = 0;
		result.z = 0;
		result.w = 0;
	}
	return result;
}

// ベクトル加算
Vector4 Vector4::operator+(const Vector4& other) const {
	return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
}

// color
  Vector4 Vector4::kRED() {
	  return Vector4(1.0f, 0.0f, 0.0f,1.0f);
}

  Vector4 Vector4::kGREEN() {
	  return Vector4(0.0f, 1.0f, 0.0f, 1.0f);
  }

  Vector4 Vector4::kBLUE() {
	  return Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  Vector4 Vector4::kBLACK() {
	  return Vector4(0.0f, 0.0f, 0.0f, 1.0f);
  }

  Vector4 Vector4::kWHITE() {
	  return Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  }

  Vector4 Vector4::UnitVector() {
	  return Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  }

  Vector4 Vector4::ZeroVector() {
	  return Vector4(0.0f, 0.0f, 0.0f, 0.0f);
  }