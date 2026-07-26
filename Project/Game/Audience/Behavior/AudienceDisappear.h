#pragma once

#include "BaseAudienceBehavior.h"
#include <functional>

/// <summary>
/// 観客が退場していく際の振る舞い
/// </summary>
class AudienceDisappear : public BaseAudienceBehavior {

public:
    // コンストラクタ
    AudienceDisappear(Audience* audience);
    ~AudienceDisappear();

    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;
    bool IsAudienceDisappear() const override { return true; }

private:
    /// ===================================================
    /// private methods
    /// ===================================================
    void Init();
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