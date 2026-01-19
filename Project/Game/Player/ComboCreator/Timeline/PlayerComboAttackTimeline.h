#pragma once

#include "PlayerComboAttackTimelineManager.h"
#include "PlayerComboAttackTimelineUI.h"
#include "utility/TimeLine/TimeLine.h"
#include <memory>

class PlayerComboAttackData;

/// <summary>
/// プレイヤー攻撃用タイムラインエディタ
/// </summary>
class PlayerComboAttackTimeline {
public:
    PlayerComboAttackTimeline()  = default;
    ~PlayerComboAttackTimeline() = default;

    void Init(PlayerComboAttackData* attackData);
    void Draw();
    void ApplyToParameters();

private:
    void AdvanceToNextAttack();

private:
    PlayerComboAttackData* attackData_ = nullptr;
    KetaEngine::TimeLine timeline_;

    PlayerComboAttackTimelineManager manager_;
    PlayerComboAttackTimelineUI ui_;

    bool isInitialized_ = false;
};