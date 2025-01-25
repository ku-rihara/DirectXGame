#pragma once

#include "Vector4.h"
/// <summary>
/// 4x4行列
/// </summary>
class Vector3;
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
Matrix4x4 MakeRotateXMatrix(float radian);

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

// 回転行列
Matrix4x4 MakeRotateMatrix(Vector3 rotate);

//トランスフォーム
Vector3 MatrixTransform(const Vector3& vector, const Matrix4x4& matrix);
Vector4 MatrixTransform(const Vector4& vector, const Matrix4x4& matrix);

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3 translate);

Matrix4x4 Inverse(const Matrix4x4& m);

Matrix4x4 Transpose(const Matrix4x4& m);

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

Vector3 ScreenTransform(Vector3 worldPos, const ViewProjection& viewProjection);

Matrix4x4 NormalizeMatrixRow(const Matrix4x4& matrix, int row);
