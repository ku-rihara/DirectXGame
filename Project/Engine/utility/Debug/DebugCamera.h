#pragma once
#include "3d/ViewProjection.h"
#include "input/Input.h"
#include "Matrix4x4.h"
#include "Vector3.h"

/// <summary>
/// デバッグカメラクラス
/// </summary>
class DebugCamera {
public:
    DebugCamera(const int& window_width, const int& window_height);
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
    void SetIsActive(const bool& is) { isActive_ = is; }
    void SetFovAngleY(const float& value) { viewProjection_.fovAngleY_ = value; }
    void SetAspectRatio(const float& value) { viewProjection_.aspectRatio_ = value; }
    void SetNearZ(const float& value) { viewProjection_.nearZ_ = value; }
    void SetFarZ(const float& value) { viewProjection_.farZ_ = value; }
};