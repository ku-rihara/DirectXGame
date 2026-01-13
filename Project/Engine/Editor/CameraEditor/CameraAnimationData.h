#pragma once
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "CameraKeyFrame.h"
#include "Easing/Easing.h"
#include "Editor/BaseEffectEditor/BaseSequenceEffectData.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// カメラアニメーションデータクラス
/// </summary>
namespace KetaEngine {

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

    struct LookAtParam {
        bool useLookAt       = false;
        Vector3 lookAtTarget = {0, 0, 0};
    };

public:
    CameraAnimationData()           = default;
    ~CameraAnimationData() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectDataからのオーバーライド
    void Init(const std::string& animationName) override;
    void Update(float speedRate = 1.0f) override;
    void Reset() override;
    void Play() override;

    void LoadSequenceElements() override;
    void SaveSequenceElements() override;

    // ViewProjectionへの適用
    void ApplyToViewProjection(ViewProjection& viewProjection);

    // エディター用
    void AdjustParam();

    // 初期値設定
    void SetInitialValues(const Vector3& position, const Vector3& rotation, float fov);

    // 注視点設定
    void SetLookAtTarget(const WorldTransform* target);

protected:
    //*---------------------------- protected Methods ----------------------------*//

    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    void UpdateKeyFrameProgression() override;
    void AdvanceToNexTSequenceElement() override;
    std::unique_ptr<CameraKeyFrame> CreateKeyFrame(int32_t index) override;
    std::string GeTSequenceElementFolderPath() const override;

private:
    //*---------------------------- private Methods ----------------------------*//

    void UpdateActiveKeyFrames(float speedRate);
    void UpdateAdaptCurrentPos();
    void StartReturnToInitial();
    void CheckIsReturnToInitial(float actualDeltaTime);

    // 注視点モード用の更新
    void UpdateLookAtMode();

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

    // LookAt Params
    LookAtParam lookAtParam_;
    const WorldTransform* targetTransform_ = nullptr;

    // frags
    bool showKeyFrameList_      = true;
    bool showAnimationControls_ = true;

    TimeModeSelector timeModeSelector_;

    const float defaultFovAngle_ = 45.0f;

public:
    //*----------------------------- getter Methods -----------------------------*//

    bool IsReturningToInitial() const { return returnParam_.isReturningToInitial; }
    bool IsUseLookAt() const { return lookAtParam_.useLookAt; }
    const Vector3& GetLookAtTarget() const { return lookAtParam_.lookAtTarget; }
 
    //*----------------------------- setter Methods -----------------------------*//
  
};

}; // KetaEngine