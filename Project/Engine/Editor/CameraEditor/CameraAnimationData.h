#pragma once
#include "3d/ViewProjection.h"
#include "CameraKeyFrame.h"
#include "Easing/Easing.h"
#include "Editor/BaseEffectEditor/BaseSequenceEffectData.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// カメラアニメーションデータクラス
/// </summary>
class CameraAnimationData : public BaseSequenceEffectData<CameraKeyFrame> {
public:
    struct CameraTransform {
        Vector3 position = {0.0f, 0.0f, 0.0f};
        Vector3 rotation = {0.0f, 0.0f, 0.0f};
        float fov        = 45.0f;
    };

    struct ResetParam {
        float timePoint         = 1.0f;
        float delayTime         = 0.0f;
        float currentDelayTimer = 0.0f;
        int32_t posEaseType     = 0;
        int32_t rotateEaseType  = 0;
        int32_t fovEaseType     = 0;
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
    CameraAnimationData()           = default;
    ~CameraAnimationData() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectDataからのオーバーライド
    void Init(const std::string& animationName) override;
    void Update(const float& speedRate = 1.0f) override;
    void Reset() override;

    // ViewProjectionへの適用
    void ApplyToViewProjection(ViewProjection& viewProjection);

    // エディター用
    void AdjustParam();

    // 初期値設定
    void SetInitialValues(const Vector3& position, const Vector3& rotation, const float& fov);

protected:
    //*---------------------------- protected Methods ----------------------------*//

    void RegisterParams() override;
    void LoadParams() override;
    void InitParams() override;

    void UpdateKeyFrameProgression() override;
    void AdvanceToNexTSequenceElement() override;
    std::unique_ptr<CameraKeyFrame> CreateKeyFrame(const int32_t& index) override;
    std::string GeTSequenceElementFolderPath() const override;

private:
    //*---------------------------- private Methods ----------------------------*//

    void UpdateActiveKeyFrames(const float& speedRate);
    void UpdateInterpolatedValues();
    void StartReturnToInitial();
    void TimeModeSelector(const char* label, int32_t& target);

private:
    //*---------------------------- private Variant ----------------------------*//

    std::string dateFolderPath_           = "CameraAnimation/AnimationData";
    const std::string keyFrameFolderPath_ = "CameraAnimation/KeyFrames/";

    int32_t finalKeyFrameIndex_ = -1;
    bool isAllFinished_         = false;

    // camera Transform
    CameraTransform currentCameraTransform_;
    CameraTransform returnCameraTransform_;
    CameraTransform initialCameraTransform_;

    // reset,return Params
    ResetParam resetParam_;
    ReturnParam returnParam_;

    bool showKeyFrameList_      = true;
    bool showAnimationControls_ = true;

    int32_t timeMode_ = static_cast<int32_t>(CameraKeyFrame::TimeMode::DELTA_TIME_RATE);

    std::vector<const char*> TimeModeLabels = {
        "DeltaTime (No TimeScale)",
        "DeltaTimeRate (With TimeScale)"};

public:
    //*----------------------------- getter Methods -----------------------------*//

    const bool& IsReturningToInitial() const { return returnParam_.isReturningToInitial; }
};