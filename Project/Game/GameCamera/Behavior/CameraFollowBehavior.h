#pragma once

#include "BaseCameraBehavior.h"
#include "Vector2.h"

/// <summary>
/// カメラ追従振る舞い
/// </summary>
class CameraFollowBehavior : public BaseCameraBehavior {
public:
    CameraFollowBehavior(GameCamera* owner);
    ~CameraFollowBehavior() = default;

    void Update(float time = 0.0f) override;
    void Debug() override;

private:
    Vector2 InputUpdate();
    void MoveUpdate();

private:
    bool isReset_ = false;
};