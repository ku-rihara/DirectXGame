#include "Matrix4x4.h"
#include "3d/ViewProjection.h"
#include "base/WinApp.h"
#include "Quaternion.h"
#include <assert.h>
#include <cmath>
#include <DirectXMath.h>
#include <numbers>
using namespace DirectX;

Matrix4x4 MakeIdentity4x4() {
    XMMATRIX identity = XMMatrixIdentity();
    Matrix4x4 result;
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&result), identity);
    return result;
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& obj) const {
    Matrix4x4 result;

    for (int row = 0; row < 4; ++row) {

        for (int column = 0; column < 4; ++column) {

            result.m[row][column] = m[row][column] + obj.m[row][column];
        }
    }

    return result;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& obj) const {
    Matrix4x4 result;

    for (int row = 0; row < 4; ++row) {

        for (int column = 0; column < 4; ++column) {

            result.m[row][column] = m[row][column] - obj.m[row][column];
        }
    }

    return result;
}

void Matrix4x4::operator*=(const Matrix4x4& obj) {
    *this = *this * obj;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& obj) const {
    Matrix4x4 result;

    for (int row = 0; row < 4; ++row) {

        for (int column = 0; column < 4; ++column) {

            result.m[row][column] = m[row][0] * obj.m[0][column] + m[row][1] * obj.m[1][column] + m[row][2] * obj.m[2][column] + m[row][3] * obj.m[3][column];
        }
    }
    return result;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
    Matrix4x4 result;

    result.m[0][0] = 1.0f;
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;
    result.m[1][0] = 0.0f;
    result.m[1][1] = 1.0f;
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;
    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = 1.0f;
    result.m[2][3] = 0.0f;
    result.m[3][0] = translate.x;
    result.m[3][1] = translate.y;
    result.m[3][2] = translate.z;
    result.m[3][3] = 1.0f;

    return result;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
    Matrix4x4 result;

    result.m[0][0] = scale.x;
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;
    result.m[1][0] = 0.0f;
    result.m[1][1] = scale.y;
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;
    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = scale.z;
    result.m[2][3] = 0.0f;
    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
    Matrix4x4 result;
    result.m[0][0] = 1;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = std::cos(radian);
    result.m[1][2] = std::sin(radian);
    result.m[1][3] = 0;
    result.m[2][0] = 0;
    result.m[2][1] = -std::sin(radian);
    result.m[2][2] = std::cos(radian);
    result.m[2][3] = 0;
    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;
    return result;
}

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
    Matrix4x4 result;
    result.m[0][0] = std::cos(radian);
    result.m[0][1] = 0;
    result.m[0][2] = -std::sin(radian);
    result.m[0][3] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = 1;
    result.m[1][2] = 0;
    result.m[1][3] = 0;
    result.m[2][0] = std::sin(radian);
    result.m[2][1] = 0;
    result.m[2][2] = std::cos(radian);
    result.m[2][3] = 0;
    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;
    return result;
}

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
    Matrix4x4 result;
    result.m[0][0] = std::cos(radian);
    result.m[0][1] = std::sin(radian);
    result.m[0][2] = 0;
    result.m[0][3] = 0;
    result.m[1][0] = -std::sin(radian);
    result.m[1][1] = std::cos(radian);
    result.m[1][2] = 0;
    result.m[1][3] = 0;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1;
    result.m[2][3] = 0;
    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;
    return result;
}

Matrix4x4 MakeRotateMatrix(Vector3 rotate) {
    return (MakeRotateXMatrix(rotate.x) * MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z));
}

Vector3 TransformMatrix(const Vector3& vector, const Matrix4x4& matrix) {
    Vector3 result;

    result.x = (vector.x * matrix.m[0][0]) + (vector.y * matrix.m[1][0]) + (vector.z * matrix.m[2][0]) + (1.0f * matrix.m[3][0]);
    result.y = (vector.x * matrix.m[0][1]) + (vector.y * matrix.m[1][1]) + (vector.z * matrix.m[2][1]) + (1.0f * matrix.m[3][1]);
    result.z = (vector.x * matrix.m[0][2]) + (vector.y * matrix.m[1][2]) + (vector.z * matrix.m[2][2]) + (1.0f * matrix.m[3][2]);

    float w = (vector.x * matrix.m[0][3]) + (vector.y * matrix.m[1][3]) + (vector.z * matrix.m[2][3]) + (1.0f * matrix.m[3][3]);

    if (w == 0.0f) {

        return Vector3(0, 0, 0);
    } else {
        result.x /= w;
        result.y /= w;
        result.z /= w;
    }

    return result;
}

Vector4 TransformMatrix(const Vector4& vector, const Matrix4x4& matrix) {
    // 行列の乗算を行い、結果を返す
    Vector4 result;
    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + vector.w * matrix.m[3][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + vector.w * matrix.m[3][1];
    result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + vector.w * matrix.m[3][2];
    result.w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + vector.w * matrix.m[3][3];
    return result;
}

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {

    Vector3 result{
        v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
        v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
        v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]
    };
    return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3 translate) {
    Matrix4x4 scaleMatrix;
    Matrix4x4 rotateMatrix;
    Matrix4x4 translateMatrix;
    Matrix4x4 SR;
    Matrix4x4 result;
    scaleMatrix     = MakeScaleMatrix(scale);
    rotateMatrix    = (MakeRotateXMatrix(rotate.x) * MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z));
    translateMatrix = MakeTranslateMatrix(translate);
    SR              = scaleMatrix * rotateMatrix;
    result          = SR * translateMatrix;

    return result;
}

Matrix4x4 MakeAffineMatrixQuaternion(const Vector3& scale, const Quaternion& rotate, const Vector3 translate) {
    return MakeScaleMatrix(scale) * MakeRotateMatrixFromQuaternion(rotate) * MakeTranslateMatrix(translate);
}

Matrix4x4 Inverse(const Matrix4x4& m) {
    float a = (m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]) - (m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]) - (m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]) + (m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]) + (m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]) - (m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]) - (m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]) + (m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]);

    Matrix4x4 result;

    result.m[0][0] = (1.0f / a) * ((m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[1][3] * m.m[2][1] * m.m[3][2]) - (m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[1][1] * m.m[2][3] * m.m[3][2]));
    result.m[0][1] = (1.0f / a) * (-(m.m[0][1] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[2][1] * m.m[3][2]) + (m.m[0][3] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[2][3] * m.m[3][2]));
    result.m[0][2] = (1.0f / a) * ((m.m[0][1] * m.m[1][2] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[3][2]) - (m.m[0][3] * m.m[1][2] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[3][2]));
    result.m[0][3] = (1.0f / a) * (-(m.m[0][1] * m.m[1][2] * m.m[2][3]) - (m.m[0][2] * m.m[1][3] * m.m[2][1]) - (m.m[0][3] * m.m[1][1] * m.m[2][2]) + (m.m[0][3] * m.m[1][2] * m.m[2][1]) + (m.m[0][2] * m.m[1][1] * m.m[2][3]) + (m.m[0][1] * m.m[1][3] * m.m[2][2]));

    result.m[1][0] = (1.0f / a) * (-(m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[1][3] * m.m[2][0] * m.m[3][2]) + (m.m[1][3] * m.m[2][2] * m.m[3][0]) + (m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[1][0] * m.m[2][3] * m.m[3][2]));
    result.m[1][1] = (1.0f / a) * ((m.m[0][0] * m.m[2][2] * m.m[3][3]) + (m.m[0][2] * m.m[2][3] * m.m[3][0]) + (m.m[0][3] * m.m[2][0] * m.m[3][2]) - (m.m[0][3] * m.m[2][2] * m.m[3][0]) - (m.m[0][2] * m.m[2][0] * m.m[3][3]) - (m.m[0][0] * m.m[2][3] * m.m[3][2]));
    result.m[1][2] = (1.0f / a) * (-(m.m[0][0] * m.m[1][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][3] * m.m[3][0]) - (m.m[0][3] * m.m[1][0] * m.m[3][2]) + (m.m[0][3] * m.m[1][2] * m.m[3][0]) + (m.m[0][2] * m.m[1][0] * m.m[3][3]) + (m.m[0][0] * m.m[1][3] * m.m[3][2]));
    result.m[1][3] = (1.0f / a) * ((m.m[0][0] * m.m[1][2] * m.m[2][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0]) + (m.m[0][3] * m.m[1][0] * m.m[2][2]) - (m.m[0][3] * m.m[1][2] * m.m[2][0]) - (m.m[0][2] * m.m[1][0] * m.m[2][3]) - (m.m[0][0] * m.m[1][3] * m.m[2][2]));

    result.m[2][0] = (1.0f / a) * ((m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[1][3] * m.m[2][0] * m.m[3][1]) - (m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[1][0] * m.m[2][3] * m.m[3][1]));
    result.m[2][1] = (1.0f / a) * (-(m.m[0][0] * m.m[2][1] * m.m[3][3]) - (m.m[0][1] * m.m[2][3] * m.m[3][0]) - (m.m[0][3] * m.m[2][0] * m.m[3][1]) + (m.m[0][3] * m.m[2][1] * m.m[3][0]) + (m.m[0][1] * m.m[2][0] * m.m[3][3]) + (m.m[0][0] * m.m[2][3] * m.m[3][1]));
    result.m[2][2] = (1.0f / a) * ((m.m[0][0] * m.m[1][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][3] * m.m[3][0]) + (m.m[0][3] * m.m[1][0] * m.m[3][1]) - (m.m[0][3] * m.m[1][1] * m.m[3][0]) - (m.m[0][1] * m.m[1][0] * m.m[3][3]) - (m.m[0][0] * m.m[1][3] * m.m[3][1]));
    result.m[2][3] = (1.0f / a) * (-(m.m[0][0] * m.m[1][1] * m.m[2][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0]) - (m.m[0][3] * m.m[1][0] * m.m[2][1]) + (m.m[0][3] * m.m[1][1] * m.m[2][0]) + (m.m[0][1] * m.m[1][0] * m.m[2][3]) + (m.m[0][0] * m.m[1][3] * m.m[2][1]));

    result.m[3][0] = (1.0f / a) * (-(m.m[1][0] * m.m[2][1] * m.m[3][2]) - (m.m[1][1] * m.m[2][2] * m.m[3][0]) - (m.m[1][2] * m.m[2][0] * m.m[3][1]) + (m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[1][1] * m.m[2][0] * m.m[3][2]) + (m.m[1][0] * m.m[2][2] * m.m[3][1]));
    result.m[3][1] = (1.0f / a) * ((m.m[0][0] * m.m[2][1] * m.m[3][2]) + (m.m[0][1] * m.m[2][2] * m.m[3][0]) + (m.m[0][2] * m.m[2][0] * m.m[3][1]) - (m.m[0][2] * m.m[2][1] * m.m[3][0]) - (m.m[0][1] * m.m[2][0] * m.m[3][2]) - (m.m[0][0] * m.m[2][2] * m.m[3][1]));
    result.m[3][2] = (1.0f / a) * (-(m.m[0][0] * m.m[1][1] * m.m[3][2]) - (m.m[0][1] * m.m[1][2] * m.m[3][0]) - (m.m[0][2] * m.m[1][0] * m.m[3][1]) + (m.m[0][2] * m.m[1][1] * m.m[3][0]) + (m.m[0][1] * m.m[1][0] * m.m[3][2]) + (m.m[0][0] * m.m[1][2] * m.m[3][1]));
    result.m[3][3] = (1.0f / a) * ((m.m[0][0] * m.m[1][1] * m.m[2][2]) + (m.m[0][1] * m.m[1][2] * m.m[2][0]) + (m.m[0][2] * m.m[1][0] * m.m[2][1]) - (m.m[0][2] * m.m[1][1] * m.m[2][0]) - (m.m[0][1] * m.m[1][0] * m.m[2][2]) - (m.m[0][0] * m.m[1][2] * m.m[2][1]));

    return result;
}

Matrix4x4 Transpose(const Matrix4x4& m) {
    Matrix4x4 result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m.m[j][i];
        }
    }

    return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
    Matrix4x4 result;
    result.m[0][0] = (1 / aspectRatio) * (1 / std::tan(fovY / 2));
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = (1 / std::tan(fovY / 2));
    result.m[1][2] = 0;
    result.m[1][3] = 0;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = farClip / (farClip - nearClip);
    result.m[2][3] = 1;
    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
    result.m[3][3] = 0;
    return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
    Matrix4x4 result;
    result.m[0][0] = 2 / (right - left);
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = 2 / (top - bottom);
    result.m[1][2] = 0;
    result.m[1][3] = 0;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1 / (farClip - nearClip);
    result.m[2][3] = 0;
    result.m[3][0] = (left + right) / (left - right);
    result.m[3][1] = (top + bottom) / (bottom - top);
    result.m[3][2] = nearClip / (nearClip - farClip);
    result.m[3][3] = 1;
    return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
    Matrix4x4 result;
    result.m[0][0] = width / 2;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = -(height / 2);
    result.m[1][2] = 0;
    result.m[1][3] = 0;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = maxDepth - minDepth;
    result.m[2][3] = 0;
    result.m[3][0] = left + (width / 2);
    result.m[3][1] = top + (height / 2);
    result.m[3][2] = minDepth;
    result.m[3][3] = 1;
    return result;
}

Vector3 ScreenTransform(Vector3 worldPos, const ViewProjection& viewProjection) {
    // ビューポート行列
    Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
    // ビュー行列とプロジェクション行列、ビューポート行列を合成する
    Matrix4x4 matViewProjectionViewport = viewProjection.matView_ * viewProjection.matProjection_ * matViewport;
    // ワールド→スクリーン変換
    return TransformMatrix(worldPos, matViewProjectionViewport);
}

// 行列の特定の行（0, 1, 2のいずれか）を正規化し、新しい行列を返す
Matrix4x4 NormalizeMatrixRow(const Matrix4x4& matrix, int row) {
    Matrix4x4 result = matrix; // 元の行列をコピー

    // 行のベクトルを取り出す（X, Y, Z 成分）
    float length = std::sqrt(result.m[row][0] * result.m[row][0] + result.m[row][1] * result.m[row][1] + result.m[row][2] * result.m[row][2]);

    // 長さが0でないことを確認
    if (length > 0.0f) {
        // 正規化（長さを1にする）
        result.m[row][0] /= length;
        result.m[row][1] /= length;
        result.m[row][2] /= length;
    }

    return result; // 正規化された行列を返す
}

// 任意軸回転行列
Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {

    Matrix4x4 result{};

    float length = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    if (length == 0.0f) {
        length = 1.0f;
    }
    float x = axis.x / length;
    float y = axis.y / length;
    float z = axis.z / length;

    float c = std::cos(angle);
    float s = std::sin(angle);
    float t = 1.0f - c;

    result.m[0][0] = c + x * x * t;
    result.m[0][1] = x * y * t + z * s;
    result.m[0][2] = x * z * t - y * s;
    result.m[0][3] = 0.0f;

    result.m[1][0] = y * x * t - z * s;
    result.m[1][1] = c + y * y * t;
    result.m[1][2] = y * z * t + x * s;
    result.m[1][3] = 0.0f;

    result.m[2][0] = z * x * t + y * s;
    result.m[2][1] = z * y * t - x * s;
    result.m[2][2] = c + z * z * t;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}

Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
    Matrix4x4 result;

    Vector3 fromNorm = from.Normalize();
    Vector3 toNorm   = to.Normalize();

    Vector3 axis = Vector3::Cross(fromNorm, toNorm);
    float dot    = Vector3::Dot(fromNorm, toNorm);
    float angle  = std::acos(dot);

    if (axis.Length() < 1e-6f) {
        if (dot > 0.9999f) {
            result.m[0][0] = result.m[1][1] = result.m[2][2] = result.m[3][3] = 1.0f;
            return result;
        } else {
            if (std::abs(fromNorm.x) > 0.99f) {
                axis = {0.0f, 1.0f, 0.0f};
            } else {
                axis = {0.0f, 0.0f, 1.0f};
            }
        }
    }

    return MakeRotateAxisAngle(axis, angle);
}

Vector3 ExtractEulerAngles(const Matrix4x4& rotationMatrix) {
    Vector3 eulerAngles;

    if (rotationMatrix.m[2][0] < 1) {
        if (rotationMatrix.m[2][0] > -1) {
            eulerAngles.y = asinf(rotationMatrix.m[2][0]);
            eulerAngles.x = atan2f(-rotationMatrix.m[2][1], rotationMatrix.m[2][2]);
            eulerAngles.z = atan2f(-rotationMatrix.m[1][0], rotationMatrix.m[0][0]);
        } else { // rotationMatrix.m[2][0] <= -1
            eulerAngles.y = -std::numbers::pi_v<float> / 2.0f;
            eulerAngles.x = -atan2f(rotationMatrix.m[1][2], rotationMatrix.m[1][1]);
            eulerAngles.z = 0;
        }
    } else { // rotationMatrix.m[2][0] >= 1
        eulerAngles.y = std::numbers::pi_v<float> / 2;
        eulerAngles.x = atan2f(rotationMatrix.m[1][2], rotationMatrix.m[1][1]);
        eulerAngles.z = 0;
    }

    return eulerAngles;
}

// クォータニオンから回転行列を作成する関数
Matrix4x4 MakeRotateMatrixFromQuaternion(const Quaternion& q) {
    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;

    Matrix4x4 matrix;

    matrix.m[0][0] = 1.0f - 2.0f * (y * y + z * z);
    matrix.m[0][1] = 2.0f * (x * y - w * z);
    matrix.m[0][2] = 2.0f * (x * z + w * y);
    matrix.m[0][3] = 0.0f;

    matrix.m[1][0] = 2.0f * (x * y + w * z);
    matrix.m[1][1] = 1.0f - 2.0f * (x * x + z * z);
    matrix.m[1][2] = 2.0f * (y * z - w * x);
    matrix.m[1][3] = 0.0f;

    matrix.m[2][0] = 2.0f * (x * z - w * y);
    matrix.m[2][1] = 2.0f * (y * z + w * x);
    matrix.m[2][2] = 1.0f - 2.0f * (x * x + y * y);
    matrix.m[2][3] = 0.0f;

    matrix.m[3][0] = 0.0f;
    matrix.m[3][1] = 0.0f;
    matrix.m[3][2] = 0.0f;
    matrix.m[3][3] = 1.0f;

    return matrix;
}
