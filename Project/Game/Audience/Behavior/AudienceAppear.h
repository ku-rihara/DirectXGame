#pragma once

#include "BaseAudienceBehavior.h"

class AudienceAppear : public BaseAudienceBehavior {

public:
    // コンストラクタ
    AudienceAppear(Audience* audience);
    ~AudienceAppear();

    void Update() override;
    void Debug() override;

private:
    /// ===================================================
    /// private variables
    /// ===================================================
};