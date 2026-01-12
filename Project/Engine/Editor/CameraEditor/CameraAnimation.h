#pragma once
#include "CameraAnimationData.h"
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include <functional>
#include <memory>
#include <string>

/// <summary>
/// カメラアニメーション制御クラス
/// </summary>
namespace KetaEngine {

class CameraAnimation : public BaseEffectPlayer {
public:
    struct CameraParam {
        Vector3 position;
        Vector3 rotation;
        float fov;
    };

public:
    CameraAnimation()           = default;
    ~CameraAnimation() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // 初期化、更新
    void Init() override;
    void Update(float speedRate = 1.0f) override;

    // 再生、リセット、初期値保存
    void Play(const std::string& animationName) override;
    void Reset() override;

    // 注視点ターゲット設定
    void SetLookAtTarget(const WorldTransform* target);

private:
    //*---------------------------- private Methods ----------------------------*//
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

    void ApplyOffsetToViewProjection();
    void SaveInitialValues();
    void ResetOffsetParam();

private:
    ViewProjection* pViewProjection_ = nullptr;

    CameraParam startParam_;

    // offset
    Vector3 currentOffsetPosition_;
    Vector3 currentOffsetRotation_;
    float currentOffsetFov_ = 0.0f;

    bool isAdapt_ = true;

public:
    const Vector3& GetOffsetPosition() const { return currentOffsetPosition_; }
    const Vector3& GetOffsetRotation() const { return currentOffsetRotation_; }
    float GetOffsetFov() const { return currentOffsetFov_; }

   
    void SetAdapt(bool adapt) { isAdapt_ = adapt; }
    void SetViewProjection(ViewProjection* viewProjection);
};
} // KetaEngine