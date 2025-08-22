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

    // ステップ実行用の関数ポインタ型
    using StepFunction = std::function<void(float)>;

public:
    CameraAnimation()  = default;
    ~CameraAnimation() = default;

    /// 初期化
    void Init();
    void Update(float deltaTime);

    // Play, Stop, ForceReset
    void Play(const std::string& animationName);
    void Stop();
    void ForceReset();

private:
    /// ステップ1: アニメーションロードと初期値取得
    void StepLoad(float deltaTime);

    /// ステップ2: キーフレームアニメーション実行
    void StepPlay(float deltaTime);

    /// ステップ3: 初期値へのリセット
    void StepReset(float deltaTime);

    /// 初期値を保存
    void SaveInitialValues();

    /// ViewProjectionにオフセット値を適用
    void ApplyOffsetToViewProjection();

private:
    // アニメーションデータ
    std::unique_ptr<CameraAnimationData> animationData_;

    // アニメーション状態
    AnimationState state_ = AnimationState::IDLE;

    // 現在実行中のステップ関数
    StepFunction currentStep_;

    // 初期値（アニメーション開始時の値）
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
    ViewProjection tempViewProjection_;

    // アニメーション名
    std::string currentAnimationName_;

public:
    // ゲッター
    bool IsPlaying() const { return state_ == AnimationState::PLAYING; }
    bool IsResetting() const { return state_ == AnimationState::RESETTING; }
    bool IsIdle() const { return state_ == AnimationState::IDLE; }
    bool IsLoading() const { return state_ == AnimationState::LOADING; }
    AnimationState GetState() const { return state_; }
    const std::string& GetAnimationName() const { return currentAnimationName_; }

    // オフセット値のゲッター
    Vector3 GetOffsetPosition() const { return currentOffsetPosition_; }
    Vector3 GetOffsetRotation() const { return currentOffsetRotation_; }
    float GetOffsetFov() const { return currentOffsetFov_; }

  
};