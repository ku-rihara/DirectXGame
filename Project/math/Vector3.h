#pragma once
#include <cmath>
#include"Matrix4x4.h"

class Vector3 {
public:
	float x;
	float y;
	float z;

public:
	// コンストラクタ
	Vector3() : x(0), y(0), z(0) {}
	Vector3(float numX, float numY, float numZ) : x(numX), y(numY), z(numZ) {}

     // 演算子オーバーロード
    Vector3 operator+(const Vector3& obj) const;
    Vector3 operator-(const Vector3& obj) const;
    Vector3 operator*(const Vector3& obj) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(const Vector3& obj) const;
    Vector3 operator/(float scalar) const;
    Vector3 operator*(const Matrix4x4& obj) const;
    Vector3 operator-() const; 

    void operator+=(const Vector3& obj);
    void operator+=(float scalar);
    void operator*=(float scalar);
    void operator-=(const Vector3& obj);

    // 比較
    friend bool operator!=(const Vector3& lhs, const Vector3& rhs);

    // スカラー,ベクトル
    friend Vector3 operator*(float scalar, const Vector3& vec);

	// 静的メソッドの宣言
	static Vector3 Normalize(const Vector3& v);
	static Vector3 Multiply(const Vector3& v1, float v2);
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);
	static float Dot(const Vector3& v1, const Vector3& v2);
	/*static float Length(const Vector3& v);*/

	static Vector3 ToUp();      // 上方向
	static Vector3 ToForward(); // 前方向
	static Vector3 ToRight();   // 右方向
	static Vector3 OneVector();// 単位ベクトル
	static Vector3 ZeroVector();


	Vector3 Normalize()const;
	float Length()const;
};

