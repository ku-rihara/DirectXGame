#pragma once
struct Vector3 {
    float x;
    float y;
    float z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float numX, float numY, float numZ) : x(numX), y(numY), z(numZ) {}

    Vector3 operator+(const Vector3& obj)const;

    Vector3 operator-(const Vector3& obj)const;

    Vector3 operator*(const Vector3& obj)const;
    Vector3 operator*(const float& scalar)const;

    Vector3 operator/(const Vector3& obj)const;
};

Vector3 Normnalize(const Vector3& v);  //正規化

Vector3 Multiply(const Vector3&v1, const float& v2);

Vector3 Cross(const Vector3& v1, const Vector3& v2);  //クロス積

float Dot(const Vector3& v1, const Vector3& v2);  //内積

float Length(const Vector3& v);   //長さ（ノルム）

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);