#pragma once

#include "BaseCameraBehavior.h"
#include<functional>

/// <summary>
/// カメラリセット振る舞い
/// </summary>
class CameraResetBehavior : public BaseCameraBehavior {
public:
    CameraResetBehavior(GameCamera* owner);
    ~CameraResetBehavior() = default;

    void Update(float time = 0.0f) override;
    void Debug() override;

    void Init();
    void SetTargetRotate();
    void Reset();

private:
    float targetRotationY_ = 0.0f;
    std::function<void()> phase_;
};