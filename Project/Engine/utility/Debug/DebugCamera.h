#pragma once
#include "3d/ViewProjection.h"
#include "input/Input.h"
#include "Matrix4x4.h"
#include "Vector3.h"

/// <summary>
/// デバッグカメラクラス
/// </summary>
namespace KetaEngine {

class DebugCamera {
public:
    DebugCamera()  = default;
    ~DebugCamera() = default;

    void Init(); //< 初期化
    void Update(); //< 更新

private:
    void UpdateMatrix(); //< 行列更新

private:
    Input* input_;
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;
    float yaw_;
    float pitch_;
    ViewProjection viewProjection_;
    Matrix4x4 matRot_;
    bool isActive_;

public:
    const ViewProjection& GetViewProjection() { return viewProjection_; }
    void SetIsActive(bool is) { isActive_ = is; }
    void SetFovAngleY(float value) { viewProjection_.fovAngleY_ = value; }
    void SetAspectRatio(float value) { viewProjection_.aspectRatio_ = value; }
    void SetNearZ(float value) { viewProjection_.nearZ_ = value; }
    void SetFarZ(float value) { viewProjection_.farZ_ = value; }
};

}; // KetaEngine
