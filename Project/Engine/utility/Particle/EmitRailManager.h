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

class SrvManager;
class EmitRailManager {
private:

    /// other class
    EmitRail rail_; 

    WorldTransform worldTransform_;
    ViewProjection viewProjection_;
 

    float railMoveTime_ = 0.0f;  // レール移動の進行度
    Vector3 cameraRotate_;       // カメラの回転角度
    Vector3 scale_;
 
    std::unique_ptr<Object3d>obj3D_;

public:
    //Function
    void Init(SrvManager* srvManager);
    void Update(const std::vector<Vector3>& controlPos,const float& speed);

    void Draw(const ViewProjection& viewProjection);
    void RailDraw(const ViewProjection& viewProjection);

    void Debug();

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
    void SetScale(Vector3 scale) { scale_ = scale; }
};
