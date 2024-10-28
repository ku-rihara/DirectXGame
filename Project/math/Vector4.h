#pragma once
/// <summary>
/// 4次元ベクトル
/// </summary>
struct Vector4{
	float x;
	float y;
	float z;
	float w;
};

Vector4 Normalize(const Vector4& v);  //正規化

float Length(const Vector4& v);