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

#include"utility/Particle/EmitRail.h"
#include"utility/Editor/ControlPosManager.h"

class SrvManager;
class RailManager {
private:
    ///name
    std::string groupName_;

    /// other class
    EmitRail rail_; 

    std::unique_ptr<ControlPosManager>emitControlPosManager_;

    WorldTransform worldTransform_;
    ViewProjection viewProjection_;
 

    float railMoveTime_ = 0.0f;  // レール移動の進行度
    Vector3 cameraRotate_;       // カメラの回転角度
    Vector3 scale_;
 
    std::unique_ptr<Object3d>obj3D_;

public:
    //Function
    void Init(const std::string& groupName);
    void Update(const float& speed);

    void Draw(const ViewProjection& viewProjection);
    void RailDraw(const ViewProjection& viewProjection);

    void ImGuiEdit();

    Vector3 RotateVectorAroundAxis(const Vector3& vec, const Vector3& axis, float angle);

    /// <summary>
    /// プロジェクション行列計算用のメンバ設定関数群
    /// </summary>
    void SetFovAngleY(float value) { viewProjection_.fovAngleY_ = value; }
    void SetAspectRatio(float value) { viewProjection_.aspectRatio_ = value; }
    void SetNearZ(float value) { viewProjection_.nearZ_ = value; }
    void SetFarZ(float value) { viewProjection_.farZ_ = value; }

    //getter
    float GetRailMoveTime()const { return railMoveTime_; }
    const Vector3& GetCameraRotate()const { return cameraRotate_; }
    Vector3 GetWorldPos()const;
    const WorldTransform& GetWorldTransform() const { return worldTransform_; }
    const ViewProjection& GetViewProjection() const { return viewProjection_; }

    /// set
    void SetParent(WorldTransform* parent);
    void SetScale(Vector3 scale) { scale_ = scale; }
};
