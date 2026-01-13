#pragma once

#include "BaseBehavior/BaseBehavior.h"

class GameCamera;

/// <summary>
/// カメラの振る舞い基底クラス
/// </summary>
class BaseCameraBehavior : public BaseBehavior<GameCamera> {
public:
    BaseCameraBehavior(const std::string& name, GameCamera* owner)
        : BaseBehavior(name, owner) {}

    virtual ~BaseCameraBehavior() = default;

    virtual void Update(float time = 0.0f) = 0;
    virtual void Debug() override          = 0;
};