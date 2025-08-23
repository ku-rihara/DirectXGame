#pragma once
#include "CameraAnimationData.h"
#include <functional>
#include <memory>
#include <string>

class CameraAnimation {
public:
    enum class AnimationState {
        IDLE, // 待機状態
        LOADING, // ロード中
        PLAYING, // 再生中
        RESETTING // リセット中
    };

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
    
    // step
    void StepLoad();
    void StepWait();
    void StepPlaying();
    void StepReset();

    /// ViewProjectionにオフセット値を適用
    void ApplyOffsetToViewProjection();

private:
    // アニメーションデータ
    std::unique_ptr<CameraAnimationData> animationData_;

    // アニメーション状態
    AnimationState currentStep_ = AnimationState::IDLE;

    // 初期値
    CameraParam initialParam_;

    // リセット用パラメータ
    float resetTimer_    = 0.0f;
    float resetDuration_ = 1.0f;

    // リセット時のイージングタイプ
    int32_t resetPosEaseType_    = 0;
    int32_t resetRotateEaseType_ = 0;
    int32_t resetFovEaseType_    = 0;

    // 現在のオフセット値
    Vector3 currentOffsetPosition_;
    Vector3 currentOffsetRotation_;
    float currentOffsetFov_ = 0.0f;

    // 一時的なViewProjection
    ViewProjection* pViewProjection_=nullptr;

    // アニメーション名
    std::string currentAnimationName_;

private:
    static void (CameraAnimation::* spFuncTable_[])();
public:
    // ゲッター
    const std::string& GetAnimationName() const { return currentAnimationName_; }

    // オフセット値のゲッター
    Vector3 GetOffsetPosition() const { return currentOffsetPosition_; }
    Vector3 GetOffsetRotation() const { return currentOffsetRotation_; }
    float GetOffsetFov() const { return currentOffsetFov_; }
    void SetViewProjection(ViewProjection* viewProjection);
};