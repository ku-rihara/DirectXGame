#pragma once
#include "3d/ViewProjection.h"
#include "CameraKeyFrame.h"
#include "Quaternion.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// カメラアニメーションデータクラス
/// </summary>
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

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="animationName">アニメーション名</param>
    void Init(const std::string& animationName);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="speedRate">速度倍率</param>
    void Update(const float& speedRate = 1.0f);

    void AdjustParam(); //< ImGuiパラメータ調整

    /// <summary>
    /// ViewProjectionへの適用
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void ApplyToViewProjection(ViewProjection& viewProjection);

    /// <summary>
    /// アクティブキーフレーム更新
    /// </summary>
    /// <param name="speedRate">速度倍率</param>
    void UpdateActiveKeyFrames(const float& speedRate = 1.0f);

     void AddKeyFrame();                       //< キーフレーム追加
    void RemoveKeyFrame(const int32_t& index); //< キーフレーム削除
    void ClearAllKeyFrames();                  //< 全キーフレームクリア

    void Play();  //< 再生
    void Pause(); //< 一時停止
    void Reset(); //< リセット

    void LoadData();         //< データ読み込み
    void LoadAllKeyFrames(); //< 全キーフレーム読み込み
    void SaveAllKeyFrames(); //< 全キーフレーム保存
    void SaveData();         //< データ保存

private:
    void BindParams();                //< パラメータバインド
    void UpdateKeyFrameProgression(); //< キーフレーム進行更新
    void AdvanceToNextKeyFrame();     //< 次のキーフレームへ進む
    void UpdateInterpolatedValues();  //< 補間値更新
    void StartReturnToInitial();      //< 初期値復帰開始

    /// <summary>
    /// タイムモードセレクター
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="target">対象</param>
    void TimeModeSelector(const char* label, int32_t& target);

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "CameraAnimation/AnimationData";

    std::vector<std::unique_ptr<CameraKeyFrame>> keyFrames_;
    int32_t selectedKeyFrameIndex_ = -1;
    int32_t finalKeyFrameIndex_    = -1;
    int32_t activeKeyFrameIndex_   = 0;

    PlayState playState_      = PlayState::STOPPED;
    bool autoReturnToInitial_ = true;
    bool isAllFinished_       = false;

    Vector3 currentPosition_;
    Vector3 currentRotation_;
    float currentFov_;

    Vector3 returnPosition_;
    Vector3 returnRotation_;
    float returnFov_;

    float resetTimePoint_   = 0.0f;
    float returnDelayTime_  = 0.0f;
    float returnDelayTimer_ = 0.0f;
    int32_t resetPosEaseType_;
    int32_t resetRotateEaseType_;
    int32_t resetFovEaseType_;

    bool showKeyFrameList_      = true;
    bool showAnimationControls_ = true;
    bool showInitialSettings_   = false;

    bool isAllKeyFramesFinished_        = false;
    int32_t lastCompletedKeyFrameIndex_ = -1;

    bool isReturningToInitial_ = false;
    bool isWaitingForReturn_   = false;
    Vector3 initialPosition_   = {0.0f, 0.0f, 0.0f};
    Vector3 initialRotation_   = {0.0f, 0.0f, 0.0f};
    float initialFov_          = 45.0f;

    Easing<Vector3> returnPositionEase_;
    Easing<Vector3> returnRotationEase_;
    Easing<float> returnFovEase_;

    int32_t timeMode_ = static_cast<int32_t>(CameraKeyFrame::TimeMode::DELTA_TIME_RATE);

    std::vector<const char*> TimeModeLabels = {
        "DeltaTime (No TimeScale)",
        "DeltaTimeRate (With TimeScale)"};

public:
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

    void SetSelectedKeyFrameIndex(const int32_t& index);
    void SetInitialValues(const Vector3& position, const Vector3& rotation, const float& fov);
};