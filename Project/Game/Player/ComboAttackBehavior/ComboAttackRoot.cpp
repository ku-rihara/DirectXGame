/// Combobehavior
#include "ComboAttackRoot.h"
#include "FallAttack.h"
#include "RightJobPunch.h"

#include "input/Input.h"
#include "Player/PlayerBehavior/PlayerJump.h"

/// Player
#include "Player/Player.h"

#include <imgui.h>

// 初期化
ComboAttackRoot::ComboAttackRoot(Player* player)
    : BaseComboAattackBehavior("ComboAttackRoot", player) {

    Init();
}

ComboAttackRoot ::~ComboAttackRoot() {
}

void ComboAttackRoot::Init() {

  
    pPlayer_->SetHeadScale(Vector3::UnitVector());
    pPlayer_->RotateReset();
    attackPatern_ = AttackPatern::NORMAL;
}

// 更新
void ComboAttackRoot::Update() {

    ///---------------------------------------------------------
    /// キー入力によるコンボ開始処理
    ///---------------------------------------------------------

    JudgeAttackPatern();

    switch (attackPatern_) {
    case AttackPatern::NORMAL:
        /// 通常攻撃
        BaseComboAattackBehavior::PreOderNextComboForButton();
        if (!isNextCombo_) {
            break;
        }
        BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<RightJobPunch>(pPlayer_));

        break;
    case AttackPatern::JUMP:
        /// ジャンプ攻撃
        BaseComboAattackBehavior::PreOderNextComboForButton();
        if (!isNextCombo_) {
            break;
        }
        BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<FallAttack>(pPlayer_));

        break;
    default:
        break;
    }
}

void ComboAttackRoot::JudgeAttackPatern() {
    /// 浮遊時のコンボ
    if (dynamic_cast<PlayerJump*>(pPlayer_->GetBehavior())) {
        attackPatern_ = AttackPatern::JUMP;
    } else {
        attackPatern_ = AttackPatern::NORMAL;
    }
}

void ComboAttackRoot::Debug() {
    ImGui::Text("ComboAttackRoot");
}