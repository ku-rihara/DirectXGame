#pragma once
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "CameraRendition.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <memory>

class LockOn;

/// <summary>
/// ゲームカメラクラス
/// プレイヤー追従とロックオンなどに対応
/// </summary>
class GameCamera {
public:
    struct Parameter {
        Vector3 offsetPos;
        float rotate;
    };

public:
    // 初期化、更新
    void Init();
    void Update(const float& cameraPlaySpeed = 1.0f);

    void MoveUpdate();      //< 移動更新
    void Reset();           //< リセット
    void GetIsCameraMove(); //< カメラ移動判定取得
    void RotateAdapt();     //< 回転適用
    void TranslateAdapt();  //< 位置適用

    /// <summary>
    /// アニメーション再生
    /// </summary>
    /// <param name="filename">アニメーションファイル名</param>
    void PlayAnimation(const std::string& filename);

    /// <summary>
    /// シェイク再生
    /// </summary>
    /// <param name="filename">シェイクファイル名</param>
    void PlayShake(const std::string& filename);

    /// <summary>
    /// オフセット計算
    /// </summary>
    /// <param name="offset">オフセット</param>
    /// <returns>計算結果</returns>
    Vector3 OffsetCalc(const Vector3& offset) const;

    // Editor
    void BindParams(); //< パラメータバインド
    void AdjustParam(); //< パラメータ調整

private:
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
    Parameter parameter_;

public:
    // getter
    const ViewProjection& GetViewProjection() { return viewProjection_; }
    ViewProjection& GetViewProjectionRef() { return viewProjection_; }
    const Parameter& GetParameter() const { return parameter_; }
    Vector3 GetWorldPos() const; //< ワールド座標取得

    // setter
    void SetTarget(const WorldTransform* target);
    void SetLockOn(LockOn* lockOn) { lockOn_ = lockOn; }
    void SetShakePos(const Vector3& shake) { shakeOffsetPos_ = shake; }
    void SetShakePosY(const float& shake) { shakeOffsetPos_.y = shake; }
    void SetDestinationAngleY_(const float& angle) { destinationAngleY_ = angle; }
    void SetViewProjectionPos(const Vector3& pos) { viewProjection_.translation_ = pos; }
};
