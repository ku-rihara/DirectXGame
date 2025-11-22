#pragma once
#include "3d/ViewProjection.h"
#include "CameraKeyFrame.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Quaternion.h"
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
    struct CameraTransform {
        Vector3 position = {0.0f, 0.0f, 0.0f};
        Vector3 rotation = {0.0f, 0.0f, 0.0f};
        float fov        = 45.0f;
    };

    struct ResetParam {
        float timePoint;
        float delayTime;
        float currentDelayTimer;
        int32_t posEaseType;
        int32_t rotateEaseType;
        int32_t fovEaseType;
    };

    struct ReturnParam {
        Easing<Vector3> positionEase;
        Easing<Vector3> rotationEase;
        Easing<float> fovEase;
        bool isReturningToInitial = false;
        bool isWaitingForReturn   = false;
        bool autoReturnToInitial  = true;
    };

public:
    CameraAnimationData()  = default;
    ~CameraAnimationData() = default;

    /// 初期化
    void Init(const std::string& animationName);
    void Update(const float& speedRate = 1.0f);

    /// ViewProjectionへの適用
    void ApplyToViewProjection(ViewProjection& viewProjection);

    /// <summary>
    /// アクティブなキーフレーム更新
    /// </summary>
    /// <param name="speedRate">速度倍率</param>
    void UpdateActiveKeyFrames(const float& speedRate = 1.0f);

    // 再生、一時停止、リセット
    void Play();
    void Pause();
    void Reset();

    // 再生状態取得
    bool IsPlaying() const;
    bool IsFinished() const;

    // ロード、セーブ、編集
    void LoadData();
    void SaveData();
    void AdjustParam();

    // キーフレームロード、セーブ
    void LoadKeyFrames();
    void SaveAllKeyFrames();

    // キーフレーム、追加、削除、クリア、初期化
    void AddKeyFrame();
    void RemoveKeyFrame(const int32_t& index);
    void ClearKeyFrames();
    void InitKeyFrames();

private:

    // パラメータ登録、取得、リセット
    void InitParams();
    void RegisterParams();
    void GetParams();
  
    // 更新処理
    void UpdateKeyFrameProgression(); 
    void UpdateInterpolatedValues(); 

    void AdvanceToNextKeyFrame(); //< 次のキーフレームへ進む
    void StartReturnToInitial(); //< 初期値復帰開始

    void CreateOrLoadKeyFrames(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles);

    /// <summary>
    /// タイムモードセレクター
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="target">対象</param>
    void TimeModeSelector(const char* label, int32_t& target);

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string dateFolderPath_               = "CameraAnimation/AnimationData";
    const std::string keyFrameFolderPath_ = "CameraAnimation/KeyFrames/";

    std::vector<std::unique_ptr<CameraKeyFrame>> keyFrames_;
    int32_t selectedKeyFrameIndex_ = -1;
    int32_t finalKeyFrameIndex_    = -1;
    int32_t activeKeyFrameIndex_   = 0;

    PlayState playState_ = PlayState::STOPPED;
    bool isAllFinished_  = false;

    // camera Transform
    CameraTransform currentCameraTransform_;
    CameraTransform returnCameraTransform_;
    CameraTransform initialCameraTransform_;

    // reset,return Params
    ResetParam resetParam_;
    ReturnParam returnParam_;

    bool showKeyFrameList_      = true;
    bool showAnimationControls_ = true;
    bool showInitialSettings_   = false;

    bool isAllKeyFramesFinished_        = false;
 

    int32_t timeMode_ = static_cast<int32_t>(CameraKeyFrame::TimeMode::DELTA_TIME_RATE);

    std::vector<const char*> TimeModeLabels = {
        "DeltaTime (No TimeScale)",
        "DeltaTimeRate (With TimeScale)"};

public:
    const std::string& GetGroupName() const { return groupName_; }
    const bool& IsAllKeyFramesFinished() const { return isAllKeyFramesFinished_; }
    const int32_t& GetActiveKeyFrameIndex() const { return activeKeyFrameIndex_; }
    int32_t GetTotalKeyFrameCount() const { return static_cast<int32_t>(keyFrames_.size()); }
    const bool& IsReturningToInitial() const { return returnParam_.isReturningToInitial; }

    const int32_t& GetSelectedKeyFrameIndex() const { return selectedKeyFrameIndex_; }
    const CameraKeyFrame* GetSelectedKeyFrame() const;

    void SetSelectedKeyFrameIndex(const int32_t& index);
    void SetInitialValues(const Vector3& position, const Vector3& rotation, const float& fov);
};