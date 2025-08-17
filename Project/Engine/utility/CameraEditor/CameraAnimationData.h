#pragma once
#include "Easing/Easing.h"
#include "Vector3.h"
#include <string>
#include <vector>

struct CameraKeyFrame {
    float timePoint  = 0.0f;
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 rotation = {0.0f, 0.0f, 0.0f};
    float fov        = 45.0f * 3.141592654f / 180.0f;

    EasingType positionEasingType = EasingType::InOutSine;
    EasingType rotationEasingType = EasingType::InOutSine;
    EasingType fovEasingType      = EasingType::InOutSine;
};

class CameraAnimationData {
public:
    CameraAnimationData() = default;
    CameraAnimationData(const std::string& name);

    void AddKeyFrame(const CameraKeyFrame& keyFrame);
    void RemoveKeyFrame(size_t index);

    const std::vector<CameraKeyFrame>& GetKeyFrames() const;
    std::vector<CameraKeyFrame>& GetKeyFrames();

    const std::string& GetName() const;
    void SetName(const std::string& name);

    float GetTotalDuration() const;

    void SortKeyFramesByTime();

private:
    void RecalculateTotalDuration();

    std::string name_;
    std::vector<CameraKeyFrame> keyFrames_;
    float totalDuration_ = 0.0f;
};