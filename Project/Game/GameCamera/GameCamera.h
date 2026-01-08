#pragma once
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "CameraRendition.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <memory>

class LockOn;

/// <summary>
/// ゲームカメラクラス
/// 注視点ベースのカメラ制御
/// </summary>
class GameCamera {
public:
    struct Parameter {
        Vector3 offsetPos; // 注視点からのオフセット位置
        float rotate; // 見下ろし角度
        float distance; // 注視点からの距離
        bool useLookAt; // 注視点モード使用フラグ
    };

public:
    // 初期化、更新
    void Init();
    void Update(float cameraPlaySpeed = 1.0f);

    void MoveUpdate(); // 移動更新
    void Reset(); // リセット
    void GetIsCameraMove(); // カメラ移動判定取得
    void RotateAdapt(); // 回転適用
    void TranslateAdapt(); // 位置適用


    /// <summary>
    /// アニメーション再生
    /// </summary>
    void PlayAnimation(const std::string& filename);

    /// <summary>
    /// シェイク再生
    /// </summary>
    void PlayShake(const std::string& filename);

    /// <summary>
    /// オフセット計算
    /// </summary>
    Vector3 OffsetCalc(const Vector3& offset) const;

    // Editor
    void RegisterParams();
    void AdjustParam();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "GameCamera";

    LockOn* lockOn_;
    KetaEngine::ViewProjection viewProjection_;
    std::unique_ptr<CameraRendition> rendition_;
    const KetaEngine::WorldTransform* target_ = nullptr;

    Vector3 stickInput_;
    Vector3 interTarget_; // 補間された注視点位置
    Vector3 lookAtTarget_; // 実際の注視点位置
    float destinationAngleY_; // 目標Y軸回転角
    int viewMoveTime_;
    Vector3 shakeOffsetPos_;
    Parameter parameter_;

public:
    // getter
    const KetaEngine::ViewProjection& GetViewProjection() { return viewProjection_; }
    const Parameter& GetParameter() const { return parameter_; }
    Vector3 GetWorldPos() const;
    const Vector3& GetLookAtTarget() const { return lookAtTarget_; }
    const KetaEngine::WorldTransform* GetTarget() const { return target_; }


    // setter
    void SetTarget(const KetaEngine::WorldTransform* target);
    void SetLockOn(LockOn* lockOn) { lockOn_ = lockOn; }
    void SetShakePos(const Vector3& shake) { shakeOffsetPos_ = shake; }
    void SetShakePosY(float shake) { shakeOffsetPos_.y = shake; }
    void SetDestinationAngleY_(float angle) { destinationAngleY_ = angle; }
    void SetViewProjectionPos(const Vector3& pos) { viewProjection_.translation_ = pos; }
    void SetUseLookAtMode(bool use) { parameter_.useLookAt = use; }
};