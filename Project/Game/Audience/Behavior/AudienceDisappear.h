#pragma once

#include "BaseAudienceBehavior.h"

class AudienceDisappear : public BaseAudienceBehavior {

public:
    // コンストラクタ
    AudienceDisappear(Audience* audience);
    ~AudienceDisappear();

    void Update() override;
    void Debug() override;

private:
    /// ===================================================
    /// private variants
    /// ===================================================
};