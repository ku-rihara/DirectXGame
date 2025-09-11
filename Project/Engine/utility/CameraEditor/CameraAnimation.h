#pragma once
#include "CameraAnimationData.h"
#include <functional>
#include <memory>
#include <string>

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

    /// 初期化
    void Init();
    void Update(float deltaTime);

    // Play, Stop, ForceReset
    void Play(const std::string& animationName);
    void Reset();
    void SaveInitialValues();

private:
    /// ViewProjectionにオフセット値を適用
    void ApplyOffsetToViewProjection();

private:
    // アニメーションデータ
    std::unique_ptr<CameraAnimationData> animationData_;
    ViewProjection* pViewProjection_ = nullptr;

    // 初期値
    CameraParam initialParam_;

    // 現在のオフセット値
    Vector3 currentOffsetPosition_;
    Vector3 currentOffsetRotation_;
    float currentOffsetFov_ = 0.0f;

    bool isAdapt_ = true;

    // アニメーション名
    std::string currentAnimationName_;

public:
    //--------------------------------------------------------------------------------------
    // getter
    //--------------------------------------------------------------------------------------
    const std::string& GetAnimationName() const { return currentAnimationName_; }
    Vector3 GetOffsetPosition() const { return currentOffsetPosition_; }
    Vector3 GetOffsetRotation() const { return currentOffsetRotation_; }
    float GetOffsetFov() const { return currentOffsetFov_; }

    //--------------------------------------------------------------------------------------
    // setter
    //--------------------------------------------------------------------------------------
    void SetAdapt(bool adapt) { isAdapt_ = adapt; }
    void SetViewProjection(ViewProjection* viewProjection);
};