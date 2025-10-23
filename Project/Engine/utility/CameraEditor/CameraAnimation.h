#pragma once
#include "CameraAnimationData.h"
#include <functional>
#include <memory>
#include <string>

/// <summary>
/// カメラアニメーション制御クラス
/// </summary>
class CameraAnimation {
public:
    struct CameraParam {
        Vector3 position;
        Vector3 rotation;
        float fov;
    };

public:
    CameraAnimation()  = default;
    ~CameraAnimation() = default;

    void Init(); //< 初期化

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="speedRate">速度倍率</param>
    void Update(const float& speedRate = 1.0f);

    /// <summary>
    /// アニメーション再生
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    void Play(const std::string& animationName);

    void Reset(); //< リセット
    void SaveInitialValues(); //< 初期値保存

private:
    void ApplyOffsetToViewProjection(); //< ViewProjectionにオフセット適用

private:
    std::unique_ptr<CameraAnimationData> animationData_;
    ViewProjection* pViewProjection_ = nullptr;

    CameraParam initialParam_;

    Vector3 currentOffsetPosition_;
    Vector3 currentOffsetRotation_;
    float currentOffsetFov_ = 0.0f;

    bool isAdapt_ = true;

    std::string currentAnimationName_;

public:
    const std::string& GetAnimationName() const { return currentAnimationName_; }
    const Vector3& GetOffsetPosition() const { return currentOffsetPosition_; }
    const Vector3& GetOffsetRotation() const { return currentOffsetRotation_; }
    const float& GetOffsetFov() const { return currentOffsetFov_; }

    void SetAdapt(const bool& adapt) { isAdapt_ = adapt; }
    void SetViewProjection(ViewProjection* viewProjection);
};