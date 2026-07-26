#pragma once

#include "BaseAudienceBehavior.h"
#include "Editor/Easing/Easing.h"

/// <summary>
/// 観客の待機・出現/退場切り替えを制御するルート振る舞い
/// </summary>
class AudienceRoot : public BaseAudienceBehavior {
private:
 
public:
    // コンストラクタ
    AudienceRoot(Audience* audience,bool isDraw);
    ~AudienceRoot();

    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;
    bool IsAudienceRoot() const override { return true; }

    void ChangeCloseMode();
    void ChangeAppearMode();

    bool GetIsDraw() const { return isDraw_; }

private:
    /// ===================================================
    /// private variables
    /// ===================================================
    bool isDraw_;
};