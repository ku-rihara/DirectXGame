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
        Vector3 firstOffset_;
        float firstRotate_;
    };

private:
    /// ===================================================
    /// private varius
    /// ===================================================

    GlobalParameter* globalParameter_; // グローバルパラメータ
    const std::string groupName_ = "GameCamera"; // グループ名

    LockOn* lockOn_;
    ViewProjection viewprojection_;

    std::unique_ptr<CameraRendition> rendition_; /// 演出管理クラス

    const WorldTransform* target_ = nullptr; 
    Vector3 stickInput_;
    Vector3 interTarget_ = {};
    float destinationAngleY_; 
    int viewMoveTime_;

    Vector3 shakeOffsetPos_;
    float rotate_; 

    Parameter paramater_; /// パラメータ構造体

    Vector3 offset_; /// オフセット

public:
    /// ===================================================
    /// public method
    /// ===================================================
    void Init(); /// 初期化
    void Update(); /// 更新
    void MoveUpdate(); /// カメラ移動更新
    void Reset(); /// リセット
    void GetIsCameraMove(); /// カメラ動いているか判定

    void Debug();

    void RotateAdapt();
    void TranslateAdapt();

    Vector3 OffsetCalc(const Vector3& offset) const;
    Vector3 GetWorldPos() const;

    // CameraRendition関連のメソッド
    void ChangeShakeMode(); /// 通常のシェイク演出開始
    void ChangeZoomInOut(); /// ズーム演出開始
    void StartZoomWithShake(); /// ズーム + シェイクの同時演出

    void PlayAnimation(const std::string& filename);
    void PlayShake(const std::string& filename);

    ///-------------------------------------------------------------------------------------
    /// Editor
    ///------------------------------------------------------------------------------------
    void BindParams();
    void AdjustParam();

    /// ===================================================
    /// getter
    /// ===================================================
    const ViewProjection& GetViewProjection() { return viewprojection_; }
    Vector3 GetTargetPos() const;
    Vector3 GetOffset() const { return offset_; };
    Parameter GetParamater() const { return paramater_; }

    // CameraRendition状態確認
    bool IsShakeActive() const;
    bool IsBehaviorActive() const;
    bool IsAnyRenditionActive() const;
    CameraRendition::ShakeMode GetShakeMode() const;
  
    /// ===================================================
    /// setter
    /// ===================================================
    void SetTarget(const WorldTransform* target);
    void SetRotate(const float& rotate) { rotate_ = rotate; }
    void SetOffSet(const Vector3& offset) { offset_ = offset; }
    void SetLockOn(LockOn* lockon) { lockOn_ = lockon; }
    void SetShakePos(const Vector3& shake) { shakeOffsetPos_ = shake; }
    void SetShakePosY(const float& shake) { shakeOffsetPos_.y = shake; }
    void SetDestinationAngleY_(float angle) { destinationAngleY_ = angle; }
    void SetViewProjectionPos(Vector3 pos) { viewprojection_.translation_ = pos; }
};