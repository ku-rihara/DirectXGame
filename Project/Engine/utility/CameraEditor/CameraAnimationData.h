#pragma once
#include "3d/ViewProjection.h"
#include "CameraKeyFrame.h"
#include "Quaternion.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class CameraAnimationData {
public:
    enum class PlayState {
        STOPPED,
        PLAYING,
        PAUSED
    };

public:
    CameraAnimationData()  = default;
    ~CameraAnimationData() = default;

    /// 初期化
    void Init(const std::string& animationName);

    /// 更新
    void Update(float deltaTime);

    /// ImGuiでの調整
    void AdjustParam();

    /// ViewProjectionへの適応
    void ApplyToViewProjection(ViewProjection& viewProjection);

    /// キーフレーム操作
    void EasingTypeSelector(const char* label, int32_t& target);
    void AddKeyFrame();
    void RemoveKeyFrame(int32_t index);
    void ClearAllKeyFrames();

    /// 再生制御
    void Play();
    void Pause();
    void Reset();

    /// データのロード
    void LoadData();
    void LoadAllKeyFrames();
    void SaveAllKeyFrames();
    void SaveData();

private:
    /// パラメータのバインド
    void BindParams();

    /// キーフレーム間の補間
    void InterpolateKeyFrames();

private:
    // GlobalParameter
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderName_ = "CameraAnimation/AnimationData";

    // キーフレーム
    std::vector<std::unique_ptr<CameraKeyFrame>> keyFrames_;
    int32_t selectedKeyFrameIndex_ = -1;

    // 再生状態
    PlayState playState_      = PlayState::STOPPED;
    float currentTime_        = 0.0f;
    bool autoReturnToInitial_ = true;

    // 現在の補間値
    Vector3 currentPosition_;
    Vector3 currentRotation_;
    float currentFov_;

    // リセット用パラメータ
    float resetTimePoint_ = 0.0f;
    int32_t resetPosEaseType_;
    int32_t resetRotateEaseType_;
    int32_t resetFovEaseType_;

    // UI用パラメータ
    float playbackSpeed_        = 1.0f;
    bool showKeyFrameList_      = true;
    bool showAnimationControls_ = true;
    bool showInitialSettings_   = false;

  
public:
    /// アニメーション制御
    float GetCurrentTimer() const { return currentTime_; }
    std::string GetGroupName() const { return groupName_; }
    bool IsPlaying() const { return playState_ == PlayState::PLAYING; }
    bool IsFinished() const;
};