#pragma once

#include "BaseAudienceBehavior.h"
#include <functional>

class AudienceAppear : public BaseAudienceBehavior {

public:
    // コンストラクタ
    AudienceAppear(Audience* audience);
    ~AudienceAppear();

    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;

private:
    /// ===================================================
    /// private methods
    /// ===================================================
    void Init();
    void Start();
    void Playing();
    void End();

private:
    /// ===================================================
    /// private variables
    /// ===================================================
    std::function<void()> currentPhase_;
};