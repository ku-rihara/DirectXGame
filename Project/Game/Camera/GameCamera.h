#pragma once
//std
#include <vector>
#include <memory>
//math
#include "Vector3.h"
//3D
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "3d/Object3d.h"
#include "Rail/Rail.h" // レールクラスを追加

class GameCamera {
private:
    WorldTransform worldTransform_;
    ViewProjection viewProjection_;
    Rail rail_;   // 追加：Railクラスのインスタンス

    float railMoveTime_ = 0.0f;  // レール移動の進行度
    Vector3 cameraRotate_;       // カメラの回転角度
    float railMoveSpeed_ = 0.1f; // レールに沿った移動速度

public:
    //Function
    void Init();
    void Update(const std::vector<Vector3>& controlPos);
    void RailDraw(const ViewProjection& viewProjection);
    void Debug();

    /// <summary>
    /// プロジェクション行列計算用のメンバ設定関数群
    /// </summary>
    void SetFovAngleY(float value) { viewProjection_.fovAngleY_ = value; }
    void SetAspectRatio(float value) { viewProjection_.aspectRatio_ = value; }
    void SetNearZ(float value) { viewProjection_.nearZ_ = value; }
    void SetFarZ(float value) { viewProjection_.farZ_ = value; }

    //getter
    const WorldTransform& GetWorldTransform() const { return worldTransform_; }
    const ViewProjection& GetViewProjection() const { return viewProjection_; }
};
