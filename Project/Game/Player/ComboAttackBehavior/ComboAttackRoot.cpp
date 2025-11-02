#include "ComboAttackRoot.h"
#include "FallAttack.h"
#include "RightJobPunch.h"

#include "DynamicComboAttack.h"
#include "input/Input.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
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

void ComboAttackRoot::Update() {
    // 攻撃ボタンが押された時
    if (Input::GetInstance()->TriggerKey(KeyboardKey::H) || Input::IsTriggerPad(0, GamepadButton::X)) {

        // ComboAttackControllerから攻撃を取得
        auto* controller = pPlayer_->GetComboAttackController();

        if (controller) {
            // デフォルトの最初の攻撃名を取得
            auto* firstAttack = controller->GetAttackByName("FirstPunch");

            if (!firstAttack) {
                return;
            }

            // コンボ攻撃を開始
            pPlayer_->ChangeComboBehavior(std::make_unique<DynamicComboAttack>(pPlayer_, firstAttack));
        }
    }
}

//// 更新
// void ComboAttackRoot::Update() {
//
//     ///---------------------------------------------------------
//     /// キー入力によるコンボ開始処理
//     ///---------------------------------------------------------
//
//     JudgeAttackPatern();
//
//     switch (attackPatern_) {
//     case AttackPatern::NORMAL:
//         /// 通常攻撃
//         BaseComboAattackBehavior::PreOderNextComboForButton();
//         if (!isNextCombo_) {
//             break;
//         }
//         BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<RightJobPunch>(pPlayer_));
//
//         break;
//     case AttackPatern::JUMP:
//         /// ジャンプ攻撃
//         BaseComboAattackBehavior::PreOderNextComboForButton();
//         if (!isNextCombo_) {
//             break;
//         }
//         BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<FallAttack>(pPlayer_));
//
//         break;
//     default:
//         break;
//     }
// }

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