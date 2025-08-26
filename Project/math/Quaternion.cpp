#include "Quaternion.h"
#include "Vector3.h"
#include <cfloat>
#include <cmath>


Quaternion Quaternion::operator-() const {
    return Quaternion(-x, -y, -z, -w);
}


Quaternion Quaternion::operator*(const Quaternion& obj) const {
    return Quaternion(
        w * obj.x + x * obj.w + y * obj.z - z * obj.y,
        w * obj.y - x * obj.z + y * obj.w + z * obj.x,
        w * obj.z + x * obj.y - y * obj.x + z * obj.w,
        w * obj.w - x * obj.x - y * obj.y - z * obj.z);
}

Quaternion Quaternion::operator*(float scalar) const {
    Quaternion result;

    result.w = this->w * scalar;
    result.x = this->x * scalar;
    result.y = this->y * scalar;
    result.z = this->z * scalar;

    return result;
}


Quaternion operator*(float scalar, const Quaternion& q) {
    return Quaternion(q.x * scalar, q.y * scalar, q.z * scalar, q.w * scalar);
}

Quaternion Quaternion::operator+(const Quaternion& obj) const {
    return Quaternion(
        x + obj.x,
        y + obj.y,
        z + obj.z,
        w + obj.w);
}

Quaternion Quaternion::Identity() {
    return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion Quaternion::Conjugate() const {
    return Quaternion(-x, -y, -z, w);
}

float Quaternion::Norm() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}


Quaternion Quaternion::Normalize() const {
    float norm = Norm();
    if (norm == 0) {
        return Identity();
    }
    return Quaternion(x / norm, y / norm, z / norm, w / norm);
}

Quaternion Quaternion::Inverse() const {
    float norm = Norm();
    if (norm == 0) {
        return Identity();
    }
    Quaternion conjugate = Conjugate();
    float normSquared    = norm * norm;
    return Quaternion(conjugate.x / normSquared, conjugate.y / normSquared, conjugate.z / normSquared, conjugate.w / normSquared);
}


Quaternion Quaternion::MakeRotateAxisAngle(const Vector3& axis, const float& angle) {
    // 正規化された軸を使用
    Vector3 normalizedAxis = (axis).Normalize();
    float halfAngle        = angle / 2.0f;
    float sinHalfAngle     = std::sin(halfAngle);

    return Quaternion(
        normalizedAxis.x * sinHalfAngle,
        normalizedAxis.y * sinHalfAngle,
        normalizedAxis.z * sinHalfAngle,
        std::cos(halfAngle));
}

Vector3 Quaternion::RotateVector(const Vector3& vector) {
    Quaternion vectorQuat(vector.x, vector.y, vector.z, 0.0f);
    Quaternion inverseQuat = this->Inverse();

    // 回転の計算
    Quaternion rotatedQuat = (*this) * vectorQuat * inverseQuat;

    return Vector3(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z);
}


float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2) {
    return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}


Quaternion Quaternion::Lerp(const Quaternion& start, const Quaternion& end, const float& t) {
 
    Quaternion result = (start * (1.0f - t)) + (end * t);
    return (result).Normalize();
}


Quaternion Quaternion::Slerp(const Quaternion& start, Quaternion end, const float& t) {

    float dot = Quaternion::Dot(start, end);

    // 内積が負の場合、最短経路を取るためにEndを反転
    if (dot < 0.0f) {
        end = end * -1.0f;
        dot = -dot;
    }

    const float DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        return Lerp(start, end, t); 
    }

    float theta_0 = std::acos(dot);
    float theta   = theta_0 * t;

    float sin_theta   = std::sin(theta);
    float sin_theta_0 = std::sin(theta_0);

    float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    return (start * s0) + (end * s1);
}

Quaternion Quaternion::EulerToQuaternion(const Vector3& Euler) {
    Quaternion result;
    // オイラー角からクォータニオンを生成
    Quaternion qx = Quaternion::MakeRotateAxisAngle(Vector3(1, 0, 0), Euler.x);
    Quaternion qy = Quaternion::MakeRotateAxisAngle(Vector3(0, 1, 0), Euler.y);
    Quaternion qz = Quaternion::MakeRotateAxisAngle(Vector3(0, 0, 1), Euler.z);

    // XYZ順で回転を合成
    result      = qx * qy * qz;
    result = result.Normalize();

    return result;
}
