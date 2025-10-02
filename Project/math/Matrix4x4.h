#pragma once

#include "Vector4.h"


class Vector3;
class Vector2;
class Quaternion;
class ViewProjection;

struct Matrix4x4 {
	float m[4][4];

	Matrix4x4 operator+(const Matrix4x4& obj) const; // 加算
	Matrix4x4 operator-(const Matrix4x4& obj) const; // 減算
	Matrix4x4 operator*(const Matrix4x4& obj) const; // 積
	void operator*=(const Matrix4x4& obj) ; // 積
};

Matrix4x4 MakeIdentity4x4();

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(const float& radian);

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(const float& radian);

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(const float& radian);

// 回転行列
Matrix4x4 MakeRotateMatrix(Vector3 rotate);

//トランスフォーム
Vector3 TransformMatrix(const Vector3& vector, const Matrix4x4& matrix);
Vector4 TransformMatrix(const Vector4& vector, const Matrix4x4& matrix);

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3 translate);
Matrix4x4 MakeAffineMatrixQuaternion(const Vector3& scale, const Quaternion& rotate, const Vector3 translate);

Matrix4x4 Inverse(const Matrix4x4& m);

Matrix4x4 Transpose(const Matrix4x4& m);

Matrix4x4 MakePerspectiveFovMatrix(const float& fovY, const float& aspectRatio, const float& nearClip, const float& farClip);

Matrix4x4 MakeOrthographicMatrix(const float& left, const float& top, const float& right, const float& bottom, const float& nearClip, const float& farClip);

Matrix4x4 MakeViewportMatrix(const float& left, const float& top, const float& width, const float& height, const float& minDepth, const float& maxDepth);

Vector2 ScreenTransform(const Vector3& worldPos, const ViewProjection& viewProjection);


Matrix4x4 NormalizeMatrixRow(const Matrix4x4& matrix, const int& row);

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, const float& angle);

Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

Vector3 ExtractEulerAngles(const Matrix4x4& matrix);

Matrix4x4 MakeRotateMatrixFromQuaternion(const Quaternion& q);

Matrix4x4 MakeRootAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up);

Quaternion QuaternionFromMatrix(const Matrix4x4& m);