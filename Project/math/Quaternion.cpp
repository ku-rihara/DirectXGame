#include "Quaternion.h" 
#include "Vector3.h"
#include <cmath>        


const float kColumnWidth = 60;

// クォータニオンの乗算
Quaternion Quaternion::operator*(const Quaternion& obj) const {
    return Quaternion(
        w * obj.x + x * obj.w + y * obj.z - z * obj.y,
        w * obj.y - x * obj.z + y * obj.w + z * obj.x,
        w * obj.z + x * obj.y - y * obj.x + z * obj.w,
        w * obj.w - x * obj.x - y * obj.y - z * obj.z
    );
}

Quaternion Quaternion::operator*(float scalar) const {
    Quaternion result;

    // クォータニオンの各成分にスカラーを掛ける
    result.w = this->w * scalar;
    result.x = this->x * scalar;
    result.y = this->y * scalar;
    result.z = this->z * scalar;

    return result;
}

Quaternion Quaternion::operator+ (const Quaternion& obj)const {
    return Quaternion(
        x + obj.x,
        y + obj.y,
        z + obj.z,
        w + obj.w
    );
}


// 單位Quaternionの生成
Quaternion Quaternion::Identity() {
    return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

// 共装Quaternionを返す
Quaternion Quaternion::Conjugate() const {
    return Quaternion(-x, -y, -z, w);
}

// ノルム(長さの等価)
float Quaternion::Norm() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

// 正規化(ノルムを一にする)
Quaternion Quaternion::Normalize() const {
    float norm = Norm();
    if (norm == 0) {
        // ノルムが0の場合は、Identityを返す
        return Identity();
    }
    return Quaternion(x / norm, y / norm, z / norm, w / norm);
}

// 逆Quaternionを返す
Quaternion Quaternion::Inverse()const {
    float norm = Norm();
    if (norm == 0) {
        // 逆が存在しない場合はIdentity
        return Identity();
    }
    Quaternion conjugate = Conjugate();
    float normSquared = norm * norm;
    return Quaternion(conjugate.x / normSquared, conjugate.y / normSquared, conjugate.z / normSquared, conjugate.w / normSquared);

}

// 任意回転を表すQuaternionの生成
Quaternion Quaternion::MakeRotateAxisAngleQuaternion(const Vector3& axis, const float& angle) {
    // 正規化された軸を使用
    Vector3 normalizedAxis = (axis).Normalize();
    float halfAngle = angle / 2.0f;
    float sinHalfAngle = std::sin(halfAngle);

    return Quaternion(
        normalizedAxis.x * sinHalfAngle,
        normalizedAxis.y * sinHalfAngle,
        normalizedAxis.z * sinHalfAngle,
        std::cos(halfAngle)
    );
}

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 Quaternion::RotateVector(const Vector3& vector) {
    Quaternion vectorQuat(vector.x, vector.y, vector.z, 0.0f);
    Quaternion inverseQuat = this->Inverse();

    // 回転の計算
    Quaternion rotatedQuat = (*this) * vectorQuat * inverseQuat;

    // 回転後のベクトルを返す
    return Vector3(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z);
}

// ドット積の計算
float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2) {
    return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

// QuaternionのLerp関数
Quaternion Quaternion::Lerp(const Quaternion& start, const Quaternion& end, float t) {
    // 線形補間を実行
    Quaternion result = (start * (1.0f - t)) + (end * t);

    // 結果を正規化して返す（回転なので正規化は重要）
    return (result).Normalize();
}

// Slerp関数（最短距離で補間）
Quaternion Quaternion::Slerp(const Quaternion& start, Quaternion end, float t) {

    float dot = Quaternion::Dot(start, end);

    // 内積が負の場合、最短経路を取るためにendを反転
    if (dot < 0.0f) {
        end = end * -1.0f;
        dot = -dot;
    }

    const float DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        return Lerp(start, end, t);  // 線形補間にフォールバック
    }

    float theta_0 = std::acos(dot);
    float theta = theta_0 * t;

    float sin_theta = std::sin(theta);
    float sin_theta_0 = std::sin(theta_0);

    float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    return (start * s0) + (end * s1);
}