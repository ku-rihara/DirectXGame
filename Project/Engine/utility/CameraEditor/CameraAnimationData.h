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
    void Update(const float& playSpeed);

    /// ImGuiでの調整
    void AdjustParam();

    /// ViewProjectionへの適応
    void ApplyToViewProjection(ViewProjection& viewProjection);

    /// キーフレーム操作
    void EasingTypeSelector(const char* label, int32_t& target);
    void AddKeyFrame();
    void RemoveKeyFrame(const int32_t& index);
    void ClearAllKeyFrames();

    /// 再生制御
    void Play();
    void Pause();
    void Reset();

    void UpdateActiveKeyFrames(const float& playSpeed);

    /// Load,Save
    void LoadData();
    void LoadAllKeyFrames();
    void SaveAllKeyFrames();
    void SaveData();

private:
  
    void BindParams();

    // キーフレーム進行管理
    void UpdateKeyFrameProgression();
    void AdvanceToNextKeyFrame();

    // 補間値更新
    void UpdateInterpolatedValues();
    void StartReturnToInitial();

private:
    // GlobalParameter
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "CameraAnimation/AnimationData";

    // キーフレーム
    std::vector<std::unique_ptr<CameraKeyFrame>> keyFrames_;
    int32_t selectedKeyFrameIndex_ = -1;
    int32_t finalKeyFrameIndex_    = -1;
    int32_t activeKeyFrameIndex_   = 0;

    // 再生状態
    PlayState playState_      = PlayState::STOPPED;
    bool autoReturnToInitial_ = true;
    bool isAllFinished_       = false;

    // 現在の補間値
    Vector3 currentPosition_;
    Vector3 currentRotation_;
    float currentFov_;

    Vector3 returnPosition_;
    Vector3 returnRotation_;
    float returnFov_;

    // リセット用パラメータ
    float resetTimePoint_ = 0.0f;
    int32_t resetPosEaseType_;
    int32_t resetRotateEaseType_;
    int32_t resetFovEaseType_;

    // UI用パラメータ
    bool showKeyFrameList_      = true;
    bool showAnimationControls_ = true;
    bool showInitialSettings_   = false;

    // キーフレーム進行管理
    bool isAllKeyFramesFinished_        = false;
    int32_t lastCompletedKeyFrameIndex_ = -1;

    // 初期値復帰用のメンバ変数
    bool isReturningToInitial_ = false;
    Vector3 initialPosition_   = {0.0f, 0.0f, 0.0f};
    Vector3 initialRotation_   = {0.0f, 0.0f, 0.0f};
    float initialFov_          = 45.0f;

    // 初期値復帰用のイージング
    Easing<Vector3> returnPositionEase_;
    Easing<Vector3> returnRotationEase_;
    Easing<float> returnFovEase_;

public:

    //--------------------------------------------------------------------------------------
    // getter
    //--------------------------------------------------------------------------------------
    std::string GetGroupName() const { return groupName_; }
    bool IsPlaying() const;
    bool IsFinished() const;

    const bool& IsAllKeyFramesFinished() const { return isAllKeyFramesFinished_; }
    const int32_t& GetActiveKeyFrameIndex() const { return activeKeyFrameIndex_; }
    const int32_t& GetLastCompletedKeyFrameIndex() const { return lastCompletedKeyFrameIndex_; }
    int32_t GetTotalKeyFrameCount() const { return static_cast<int32_t>(keyFrames_.size()); }
    const bool& IsReturningToInitial() const { return isReturningToInitial_; }
  
    const int32_t& GetSelectedKeyFrameIndex() const { return selectedKeyFrameIndex_; }
    CameraKeyFrame* GetSelectedKeyFrame();
    const CameraKeyFrame* GetSelectedKeyFrame() const;
    //--------------------------------------------------------------------------------------
    // setter
    //--------------------------------------------------------------------------------------
    void SetSelectedKeyFrameIndex(const int32_t& index);
    void SetInitialValues(const Vector3& position, const Vector3& rotation, const float& fov);
};