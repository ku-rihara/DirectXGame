#pragma once

#include "BaseAudienceBehavior.h"
#include "Easing/Easing.h"
#include "Easing/EasingFunction.h"

class AudienceRoot : public BaseAudienceBehavior {
private:
 
public:
    // コンストラクタ
    AudienceRoot(Audience* audience);
    ~AudienceRoot();

    void Update() override;
    void Debug() override;

private:
    /// ===================================================
    /// private variables
    /// ===================================================

};