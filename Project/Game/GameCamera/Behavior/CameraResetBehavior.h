#pragma once

#include "BaseCameraBehavior.h"

/// <summary>
/// カメラリセット振る舞い
/// </summary>
class CameraResetBehavior : public BaseCameraBehavior {
public:
    CameraResetBehavior(GameCamera* owner);
    ~CameraResetBehavior() = default;

    void Update(float time = 0.0f) override;
    void Debug() override;

private:
   
};