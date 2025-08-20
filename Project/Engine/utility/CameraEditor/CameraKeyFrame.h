#pragma once
#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <cstdint>
#include <string>
#include <vector>

class CameraKeyFrame {
public:
    struct KeyFrameParam {
        Vector3 position = {0.0f, 0.0f, 0.0f};
        Vector3 rotation = {0.0f, 0.0f, 0.0f};
        float fov        = 45.0f * 3.141592654f / 180.0f;
    };

public:
    CameraKeyFrame()  = default;
    ~CameraKeyFrame() = default;

    void Init(const std::string& cameraAnimationName, const int32_t& keyNumber);
    void Update(float deltaTime);

    // paramEdit
    void EasingTypeSelector(const char* label, int32_t& target);
    void AdjustParam();
    void BindParams();
    void AdaptEaseParam();

    // データのロード
    void LoadData();
    void SaveData();

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderName_ = "CameraAnimation/KeyFrames";
    int32_t currentKeyFrameIndex  = -1;

    float timePoint_ = 0.0f;
    KeyFrameParam keyFrameParam_;
    KeyFrameParam currentKeyFrameParam_;

    // easing Type
    int32_t positionEaseType_ = 0;
    int32_t rotationEaseType_ = 0;
    int32_t fovEaseType_      = 0;

    // easing
    Easing<Vector3> positionEase_;
    Easing<Vector3> rotationEase_;
    Easing<float> fovEase_;

public:
    // ゲッター
    float GetTimePoint() const { return timePoint_; }
    Vector3 GetPosition() const { return currentKeyFrameParam_.position; }
    Vector3 GetRotation() const { return currentKeyFrameParam_.rotation; }
    float GetFov() const { return currentKeyFrameParam_.fov; }

    // セッター
    void SetTimePoint(float timePoint) { timePoint_ = timePoint; }
};