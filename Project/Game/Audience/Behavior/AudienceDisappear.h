#pragma once

#include "BaseAudienceBehavior.h"
#include <functional>

class AudienceDisappear : public BaseAudienceBehavior {

public:
    // コンストラクタ
    AudienceDisappear(Audience* audience);
    ~AudienceDisappear();

    void Update() override;
    void Debug() override;

private:
    /// ===================================================
    /// private methods
    /// ===================================================
    void Start();
    void AnimationPlaying();
    void EasingPlaying();
    void End();

    /// ===================================================
    /// private variables
    /// ===================================================
    std::function<void()> currentPhase_;
    bool isAnimationEnd_;
};