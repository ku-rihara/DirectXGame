#pragma once
#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <string>
#include <vector>
#include <cstdint>
class CameraKeyFrame {
public:
    CameraKeyFrame()  = default;
    ~CameraKeyFrame() = default;

    void Init();
    void Update(float deltaTime);

    // paramEdit
    void EasingTypeSelector(const char* label, int32_t& target);
    void AdjustParam();
    void BindParams();

private:
    GlobalParameter* globalParameter_;
    const std::string groupName_;

    float timePoint  = 0.0f;
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 rotation = {0.0f, 0.0f, 0.0f};
    float fov        = 45.0f * 3.141592654f / 180.0f;

    // easing Type
    int32_t positionEaseType_ = 0;
    int32_t rotationEaseType_ = 0;
    int32_t fovEaseType_      = 0;

    // easing
    Easing<Vector3> positionEase_;
    Easing<Vector3> rotationEase_;
    Easing<float> fovEase_;

public:
};