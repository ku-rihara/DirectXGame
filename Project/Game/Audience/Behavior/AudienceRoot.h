#pragma once

#include "BaseAudienceBehavior.h"
#include "Easing/Easing.h"
#include "Easing/EasingFunction.h"

class AudienceRoot : public BaseAudienceBehavior {
private:
 
public:
    // コンストラクタ
    AudienceRoot(Audience* audience,bool isDraw);
    ~AudienceRoot();

    void Update() override;
    void Debug() override;

    void ChangeCloseMode();
    void ChangeAppearMode();

private:
    /// ===================================================
    /// private variables
    /// ===================================================
    bool isDraw_;
};