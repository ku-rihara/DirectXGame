#pragma once
#include "3D/ViewProjection.h"
#include "3D/WorldTransform.h"
#include "Behavior/BaseCameraBehavior.h"
#include "CameraRendition.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <memory>

struct GameObj;
class LockOn;

/// <summary>
/// ゲームカメラクラス
/// </summary>
class GameCamera {
public:
    /// <summary>
    /// カメラ追従・Y軸回転の補間にかける時間
    /// </summary>
    struct InterpolationTime {
        float target;
        float rotateY;
    };

    /// <summary>
    /// ゲームカメラの全体パラメータ(補間時間・オフセット・追従条件)
    /// </summary>
    struct Parameter {
        InterpolationTime interpolationTime;
        Vector3 offsetPos;
        float rotate;               // 見下ろし角度
        float rotateYSpeed;         // カメラY軸回転速度
        float lockOnFaceMinDistance; //カメラ追従可能な最低距離
    };

public:
    // 初期化、更新
    void Init();
    void Update(float cameraPlaySpeed = 1.0f);

    void Reset();           // リセット
    void GetIsCameraMove(); // カメラ移動判定取得
    void RotateAdapt();     // 回転適用
    void TranslateAdapt();  // 位置適用

    /// <summary>
    /// アニメーション再生
    /// </summary>
    void PlayAnimation(const std::string& filename, bool isReset);

    /// <summary>
    /// シェイク再生
    /// </summary>
    void PlayShake(const std::string& filename);

    /// <summary>
    /// オフセット計算
    /// </summary>
    Vector3 OffsetCalc(const Vector3& offset) const;

    /// <summary>
    /// Y軸回転補間
    /// </summary>
    void RotateYInterpolation(float targetAngle);

    /// <summary>
    /// Behavior切り替え
    /// </summary>
    void ChangeBehavior(std::unique_ptr<BaseCameraBehavior> behavior);

    // Editor
    void RegisterParams();
    void AdjustParam();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "GameCamera";

    LockOn* lockOn_;
    KetaEngine::ViewProjection viewProjection_;
    std::unique_ptr<CameraRendition> rendition_;
    std::unique_ptr<BaseCameraBehavior> behavior_;
    const KetaEngine::WorldTransform* target_ = nullptr;

    bool isInputEnabled_ = true; // 入力受付フラグ
    bool isLockOnFollowEnabled_ = true; 
    Vector3 stickInput_;
    Vector3 interTarget_;     // 補間された注視点位置
    float destinationAngleY_; // 目標Y軸回転角
    int viewMoveTime_;
    Vector3 shakeOffsetPos_;
    Parameter parameter_;

public:
    // getter
    const KetaEngine::ViewProjection& GetViewProjection() { return viewProjection_; }
    LockOn* GetLockOn() const { return lockOn_; }
    KetaEngine::ViewProjection& GetViewProjectionRef() { return viewProjection_; }
    const Parameter& GetParameter() const { return parameter_; }
    Vector3 GetWorldPos() const;
    const KetaEngine::WorldTransform* GetTarget() const { return target_; }
    float GetDestinationAngleY() const { return destinationAngleY_; }

    // setter
    void Connect(GameObj* go);
    void SetTarget(const KetaEngine::WorldTransform* target);
    void SetLockOn(LockOn* lockOn) { lockOn_ = lockOn; }
    void SetShakePos(const Vector3& shake) { shakeOffsetPos_ = shake; }
    void SetShakePosY(float shake) { shakeOffsetPos_.y = shake; }
    void SetDestinationAngleY(float angle) { destinationAngleY_ = angle; }
    void SetViewProjectionPos(const Vector3& pos) { viewProjection_.translation_ = pos; }

    void AddDestinationAngleY(float deltaAngle) { destinationAngleY_ += deltaAngle; }

    void SetInputEnabled(bool enabled) { isInputEnabled_ = enabled; }
    bool GetIsInputEnabled() const { return isInputEnabled_; }

    void SetLockOnFollowEnabled(bool enabled) { isLockOnFollowEnabled_ = enabled; }
    bool GetIsLockOnFollowEnabled() const { return isLockOnFollowEnabled_; }
    bool IsRenditionFinished() const { return rendition_ && rendition_->IsAnimationFinished(); }
};