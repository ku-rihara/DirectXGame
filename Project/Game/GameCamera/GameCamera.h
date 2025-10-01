#pragma once
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "CameraRendition.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <memory>

class LockOn;
class GameCamera {
public:
    struct Parameter {
        Vector3 offsetPos;
        float rotate;
    };

public:
    /// ===================================================
    /// public method
    /// ===================================================
    void Init();
    void Update(const float& cameraPlaySpeed = 1.0f);
    void MoveUpdate();

    void Reset();
    void GetIsCameraMove();

  
    void RotateAdapt();
    void TranslateAdapt();

    void PlayAnimation(const std::string& filename);
    void PlayShake(const std::string& filename);

    // Editor
    void BindParams();
    void AdjustParam();

private:
    /// ===================================================
    /// private various
    /// ===================================================

    GlobalParameter* globalParameter_;
    const std::string groupName_ = "GameCamera";

    LockOn* lockOn_;
    ViewProjection viewProjection_;

    std::unique_ptr<CameraRendition> rendition_;

    const WorldTransform* target_ = nullptr;
    Vector3 stickInput_;
    Vector3 interTarget_ = {};
    float destinationAngleY_;
    int viewMoveTime_;

    Vector3 shakeOffsetPos_;
    Parameter parameter_; //< パラメータ構造体

  

public:
    /// ===================================================
    /// getter
    /// ===================================================
    const ViewProjection& GetViewProjection() { return viewProjection_; }
    ViewProjection& GetViewProjectionRef() { return viewProjection_; }
    const Parameter& GetParameter() const { return parameter_; }
    Vector3 OffsetCalc(const Vector3& offset) const;
    Vector3 GetWorldPos() const;
    /// ===================================================
    /// setter
    /// ===================================================
    void SetTarget(const WorldTransform* target);
    void SetLockOn(LockOn* lockOn) { lockOn_ = lockOn; }
    void SetShakePos(const Vector3& shake) { shakeOffsetPos_ = shake; }
    void SetShakePosY(const float& shake) { shakeOffsetPos_.y = shake; }
    void SetDestinationAngleY_(const float& angle) { destinationAngleY_ = angle; }
    void SetViewProjectionPos(const Vector3& pos) { viewProjection_.translation_ = pos; }
};