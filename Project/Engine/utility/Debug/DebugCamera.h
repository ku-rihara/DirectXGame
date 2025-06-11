#pragma once
#include "3d/ViewProjection.h"
#include "input/Input.h"
#include "Matrix4x4.h"
#include "Vector3.h"
class DebugCamera {
    // カメラ注視点までの距離
    static const float distance_;

public:
    DebugCamera(int window_width, int window_height);

    void Init();

    void Update();

    /// <summary>
    /// プロジェクション行列計算用のメンバ設定関数群
    /// </summary>
    void SetFovAngleY(float value) { viewProjection_.fovAngleY_ = value; }
    void SetAspectRatio(float value) { viewProjection_.aspectRatio_ = value; }
    void SetNearZ(float value) { viewProjection_.nearZ_ = value; }
    void SetFarZ(float value) { viewProjection_.farZ_ = value; }

private: 
    /// --------------------------------------------------------------------------
    ///  Private method
    /// --------------------------------------------------------------------------
    Input* input_;
    // スケーリング
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;

    float yaw_;
    float pitch_;

    // ビュープロジェクション
    ViewProjection viewProjection_;
    // 回転行列
    Matrix4x4 matRot_;
    // アクティブ化
    bool isActive_;

private:
    void UpdateMatrix();

public:
   

public: // acsesser
    const ViewProjection& GetViewProjection() { return viewProjection_; }
    void SetIsActive(bool is) { isActive_ = is; }
};
