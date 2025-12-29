#pragma once

#include "BaseBehavior/BaseBehavior.h"

class Audience;

/// <summary>
/// 観客振る舞いの基底クラス
/// </summary>
class BaseAudienceBehavior : public BaseBehavior<Audience> {
public:
    BaseAudienceBehavior(const std::string& name, Audience* audience);
    virtual ~BaseAudienceBehavior() = default;

    virtual void Update(float timeSpeed = 0.0f) override = 0;
    virtual void Debug() override                        = 0;
};