#include "MathFunction.h"
#include"3d/ViewProjection.h"
#include <cmath>
#include<numbers>
#include<assert.h>

float Lerp(const float& start, const float& end, float t) {
	return (1.0f - t) * start + end * t;
}

Vector2 Lerp(const Vector2& start, const Vector2& end, float t) {
	Vector2 result;
	result.x = (1.0f - t) * start.x + end.x * t;
	result.y = (1.0f - t) * start.y + end.y * t;
	return result;
}


Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
	Vector3 result;
	result.x = (1.0f - t) * start.x + end.x * t;
	result.y = (1.0f - t) * start.y + end.y * t;
	result.z = (1.0f - t) * start.z + end.z * t;
	return result;
}


Vector3 SLerp(const Vector3& start, const Vector3& end, float t) {
    // ベクトルの正規化
    Vector3 Nstart = Vector3::Normalize(start);
    Vector3 Nend = Vector3::Normalize(end);

    // 内積を求める
    float dot = Vector3::Dot(Nstart, Nend);

    // ドット積の誤差補正
    if (dot > 1.0f) {
        dot = 1.0f;
    }
    else if (dot < -1.0f) {
        dot = -1.0f;
    }

    // 角度θをアークコサインで求める
    float theta = std::acos(dot);

    // もしθが非常に小さい（つまりベクトルがほぼ同じ方向）なら、線形補間を使う
    if (std::abs(theta) < 1.0e-5) {
        return Lerp(start, end, t);  // 線形補間で十分
    }

    // sinθを求める
    float sinTheta = std::sin(theta);
    float sinThetaFrom = std::sin((1 - t) * theta);
    float sinThetaTo = std::sin(t * theta);

    // 球面線形補間したベクトル（単位ベクトル）
    Vector3 NormalizeVector = (sinThetaFrom * Nstart + sinThetaTo * Nend) / sinTheta;

    // ベクトルの長さを線形補間
    float length1 = (start).Length();
    float length2 = (end).Length();
    float length = Lerp(length1, length2, t);

    // 補間したベクトルに長さを掛けて返す
    return NormalizeVector * length;
}


float Clamp(float n, float min, float max) {
	if (n > max) {
		return max;
	}
	if (n < min) {
		return min;
	}
	return n;
}

size_t Clamp(size_t n, size_t min, size_t max) {
	if (n > max) {
		return max;
	}
	if (n < min) {
		return min;
	}
	return n;
}

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3 p2, const Vector3& p3, float t) {
	Vector3 result;
	const float s = 0.5f;
	float t2 = t * t;
	float t3 = t2 * t;
	Vector3 e3;
	Vector3 e2;
	Vector3 e1;
	Vector3 e0;
	e3.x = -p0.x + 3 * p1.x - 3 * p2.x + p3.x;
	e3.y = -p0.y + 3 * p1.y - 3 * p2.y + p3.y;
	e3.z = -p0.z + 3 * p1.z - 3 * p2.z + p3.z;
	e2.x = 2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x;
	e2.y = 2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y;
	e2.z = 2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z;
	e1.x = -p0.x + p2.x;
	e1.y = -p0.y + p2.y;
	e1.z = -p0.z + p2.z;
	e0.x = 2 * p1.x;
	e0.y = 2 * p1.y;
	e0.z = 2 * p1.z;
	result.x = s * (e3.x * t3 + e2.x * t2 + e1.x * t + e0.x);
	result.y = s * (e3.y * t3 + e2.y * t2 + e1.y * t + e0.y);
	result.z = s * (e3.z * t3 + e2.z * t2 + e1.z * t + e0.z);
	return result;
}

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t) {
    if (points.size() < 2) {
        return Vector3(0, 0, 0);
    }

    if (points.size() == 2) {
        // 制御点が2つの場合は線形補間
        return Lerp(points[0], points[1], t);
    }

    if (points.size() == 3) {
        // 制御点が3つの場合は特殊処理
        // p0とp3を重複使用
        if (t <= 0.5f) {
            float t_2 = t * 2.0f;
            return CatmullRomInterpolation(points[0], points[0], points[1], points[2], t_2);
        } else {
            float t_2 = (t - 0.5f) * 2.0f;
            return CatmullRomInterpolation(points[0], points[1], points[2], points[2], t_2);
        }
    }

    // tを[0.0, 1.0]の範囲にクランプ
    t = Clamp(t, 0.0f, 1.0f);

    // 区間数は制御点の数-1
    size_t division = points.size() - 1;

    // tを使って区間番号を計算
    float scaledT = t * static_cast<float>(division);
    size_t index  = static_cast<size_t>(scaledT);

    // 境界条件: t = 1.0の時は最後の区間に留める
    if (index >= division) {
        index = division - 1;
    }

    // 区間内の進行度を計算(0.0~1.0)
    float t_2 = scaledT - static_cast<float>(index);
    // 浮動小数点誤差を考慮してクランプ
    t_2 = Clamp(t_2, 0.0f, 1.0f);

    // 4点分のインデックス
    size_t index0 = (index > 0) ? index - 1 : index;
    size_t index1 = index;
    size_t index2 = index + 1;
    size_t index3 = (index + 2 < points.size()) ? index + 2 : index + 1;

    // 4点の座標
    const Vector3& p0 = points[index0];
    const Vector3& p1 = points[index1];
    const Vector3& p2 = points[index2];
    const Vector3& p3 = points[index3];

    // 4点を指定してCatmull-Rom補間
    return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}


float LerpShortAngle(float a, float b, float t) {
	// 角度差分を求める
	float diff = b - a;
	float pi = 3.141592f;
	// 角度を[-2PI,+2PI]に補正する
	diff = std::fmodf(diff, 2.0f * pi);
	// 角度を[-PI,PI]に補正する
	if (diff > pi) {
		diff -= 2.0f * pi;
	}
	else if (diff < -pi) {
		diff += 2.0f * pi;
	}
	return a + diff * t;
}
 
Vector3 DirectionToEulerAngles(const Vector3& direction, const ViewProjection& view) {

    // ベクトル正規化
    Vector3 rdirection = direction.Normalize();

    // カメラの回転を反映した回転行列を作成
    Matrix4x4 rotateCameraMatrix = MakeRotateMatrix(Vector3(-view.rotation_.x, -view.rotation_.y, -view.rotation_.z));
    rdirection                   = TransformNormal(rdirection, rotateCameraMatrix);

    // 基準ベクトル(上方向)をカメラの回転で変換
    Vector3 up = {0.0f, 1.0f, 0.0f};
    up         = TransformNormal(up, rotateCameraMatrix);

    // 方向変換行列を作成
    Matrix4x4 dToDMatrix = DirectionToDirection(up, rdirection);

    // 方向変換行列からオイラー角を抽出
    Vector3 angle = ExtractEulerAngles(dToDMatrix);

    return angle;
}


constexpr float Deg2Rad = std::numbers::pi_v<float> / 180.0f;
constexpr float Rad2Deg = 180.0f / std::numbers::pi_v<float>;

// float
float ToRadian(float degrees) {
    return degrees * Deg2Rad;
}

float ToDegree(float radians) {
    return radians * Rad2Deg;
}

// Vector2
Vector2 ToRadian(const Vector2& degrees) {
    return {degrees.x * Deg2Rad, degrees.y * Deg2Rad};
}

Vector2 ToDegree(const Vector2& radians) {
    return {radians.x * Rad2Deg, radians.y * Rad2Deg};
}

// Vector3
Vector3 ToRadian(const Vector3& degrees) {
    return {degrees.x * Deg2Rad, degrees.y * Deg2Rad, degrees.z * Deg2Rad};
}

Vector3 ToDegree(const Vector3& radians) {
    return {radians.x * Rad2Deg, radians.y * Rad2Deg, radians.z * Rad2Deg};
}