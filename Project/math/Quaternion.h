#pragma once
class Vector3;
class Quaternion {
public:
    float x;
    float y;
    float z;
    float w;

    // コンストラクタ
    Quaternion() : x(0), y(0), z(0), w(0) {}
    Quaternion(float numX, float numY, float numZ, float numW) : x(numX), y(numY), z(numZ), w(numW) {}

    // 乗算
    Quaternion operator*(const Quaternion& obj) const;
    Quaternion operator*(float scalar) const;
    friend Quaternion operator*(float scalar, const Quaternion& q);
    Quaternion operator+(const Quaternion& obj) const;
    Quaternion operator-() const;

    static Quaternion Identity(); // 単位
    static Quaternion MakeRotateAxisAngle(const Vector3& axis, const float& angle);
    static float Dot(const Quaternion& q1, const Quaternion& q2);
    static Quaternion Lerp(const Quaternion& start, const Quaternion& end, const float& t);
    static Quaternion Slerp(const Quaternion& start, Quaternion end, const float& t);
    static Quaternion EulerToQuaternion(const Vector3& Euler);

    // 共役Quaternionを返す
    Quaternion Conjugate() const;

    // ノルム
    float Norm() const;

    // 正規化
    Quaternion Normalize() const;

    // 逆Quaternion
    Quaternion Inverse() const;

    Vector3 RotateVector(const Vector3& vector);

};
