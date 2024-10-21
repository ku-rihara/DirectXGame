#include "Quaternion.h"
#include <cmath>

Quaternion::Quaternion() : x(0), y(0), z(0), w(1) {}

Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

// クォータニオンの長さを計算
float Quaternion::Lenght(const Quaternion& q) {
    return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

// クォータニオンの正規化
Quaternion Quaternion::Normalize(const Quaternion& q) {
    float length = Lenght(q);
    return (length > 0.0f) ? q / length : Quaternion(0, 0, 0, 1);
}

// クォータニオンを使ってベクトルを変換
Vector3 Quaternion::Transform(const Vector3& v, const Quaternion& q) {
    Quaternion qv(v.x, v.y, v.z, 0);
    Quaternion invQ = q.Inverse();
    Quaternion result = q * qv * invQ;
    return { result.x, result.y, result.z };
}

// 指定された軸と角度からクォータニオンを生成
Quaternion Quaternion::MakeFromAxis(const Vector3& axis, float theta) {
    Vector3 normAxis = Vector3::Normalize(axis);
    float halfTheta = theta * 0.5f;
    float sinHalfTheta = std::sin(halfTheta);
    return Quaternion(
        normAxis.x * sinHalfTheta,
        normAxis.y * sinHalfTheta,
        normAxis.z * sinHalfTheta,
        std::cos(halfTheta)
    );
}

// クォータニオンを使ってLookAtを実装
Quaternion Quaternion::LockAt(const Vector3& position, const Vector3& target, const Vector3& up) {
    Vector3 forward = Vector3::Normalize(target - position);
    Vector3 right = Vector3::Cross(up, forward).Normalize();
    Vector3 adjustedUp = Vector3::Cross(forward, right);

    float trace = right.x + adjustedUp.y + forward.z;
    Quaternion q;

    if (trace > 0.0f) {
        float s = std::sqrt(trace + 1.0f) * 2.0f;
        q.w = 0.25f * s;
        q.x = (adjustedUp.z - forward.y) / s;
        q.y = (forward.x - right.z) / s;
        q.z = (right.y - adjustedUp.x) / s;
    }
    else if ((right.x > adjustedUp.y) && (right.x > forward.z)) {
        float s = std::sqrt(1.0f + right.x - adjustedUp.y - forward.z) * 2.0f;
        q.w = (adjustedUp.z - forward.y) / s;
        q.x = 0.25f * s;
        q.y = (adjustedUp.x + right.y) / s;
        q.z = (forward.x + right.z) / s;
    }
    else if (adjustedUp.y > forward.z) {
        float s = std::sqrt(1.0f + adjustedUp.y - right.x - forward.z) * 2.0f;
        q.w = (forward.x - right.z) / s;
        q.x = (adjustedUp.x + right.y) / s;
        q.y = 0.25f * s;
        q.z = (forward.y + adjustedUp.z) / s;
    }
    else {
        float s = std::sqrt(1.0f + forward.z - right.x - adjustedUp.y) * 2.0f;
        q.w = (right.y - adjustedUp.x) / s;
        q.x = (forward.x + right.z) / s;
        q.y = (forward.y + adjustedUp.z) / s;
        q.z = 0.25f * s;
    }

    return Normalize(q);
}

// クォータニオンのドット積
float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2) {
    return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

// クォータニオンのクロス積
Vector3 Quaternion::Cross(const Quaternion& q1, const Quaternion& q2) {
    return Vector3(
        q1.y * q2.z - q1.z * q2.y,
        q1.z * q2.x - q1.x * q2.z,
        q1.x * q2.y - q1.y * q2.x
    );
}

// クォータニオンの共役を計算
Quaternion Quaternion::Conjugate() const {
    return Quaternion(-x, -y, -z, w);
}

// クォータニオンの長さを計算
float Quaternion::Lenght() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

// 逆クォータニオンを計算
Quaternion Quaternion::Inverse() const {
    float norm = x * x + y * y + z * z + w * w;
    return Conjugate() / norm;
}
