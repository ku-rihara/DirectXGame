#pragma once
//std
#include <vector>
#include <memory>
//math
#include "Vector3.h"
//3D
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "3d/Line3D.h"

#include"utility/RailEditor/Rail.h"
#include"utility/RailEditor/ControlPosManager.h"

class SrvManager;
class RailManager {
public:
    enum class PositionMode {
        WORLD,
        LOCAL,
    };

public:

    RailManager() = default;
    ~RailManager() = default;

    // Function
    void Init(const std::string& groupName);
    void Update(const float& speed, const PositionMode& mode = PositionMode::WORLD, const Vector3& dirention = {1.0f, 1.0f, 1.0f});

    void Draw(const ViewProjection& viewProjection, const Vector3& size = Vector3(0.1f, 0.1f, 0.1f));
    void RailDraw(const ViewProjection& viewProjection);
    void RoopOrStop();

    void ImGuiEdit();

private:
    ///name
    std::string groupName_;

    /// other class
    Rail rail_;

    std::unique_ptr<ControlPosManager>emitControlPosManager_;

    WorldTransform worldTransform_;
    ViewProjection viewProjection_;


    float railMoveTime_ = 0.0f;  // レール移動の進行度
    Vector3 cameraRotate_;       // カメラの回転角度
    Vector3 scale_;
    bool isRoop_;

    Line3D lineDrawer_;

public:
     ///=====================================================
     /// getter method
     ///=====================================================

    bool GetIsRoop()const { return isRoop_; }
    float GetRailMoveTime()const { return railMoveTime_; }
    const Vector3& GetCameraRotate()const { return cameraRotate_; }
    const WorldTransform& GetWorldTransform() const { return worldTransform_; }
    const ViewProjection& GetViewProjection() const { return viewProjection_; }
    Vector3 GetPositionOnRail() const;

    ///=====================================================
    /// setter method
    ///=====================================================
    void SetParent(WorldTransform* parent);
    void SetScale(Vector3 scale) { scale_ = scale; }
    void SetRailMoveTime(const float& t) { railMoveTime_ = t; }
    void SetIsRoop(const bool& is) { isRoop_ = is; }


    void SetFovAngleY(float value) { viewProjection_.fovAngleY_ = value; }
    void SetAspectRatio(float value) { viewProjection_.aspectRatio_ = value; }
    void SetNearZ(float value) { viewProjection_.nearZ_ = value; }
    void SetFarZ(float value) { viewProjection_.farZ_ = value; }
};
