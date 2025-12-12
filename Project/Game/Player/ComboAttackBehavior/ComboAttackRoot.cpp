#include "ComboAttackRoot.h"
#include "DynamicComboAttack.h"
#include "input/Input.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"
#include "Player/PlayerBehavior/PlayerJump.h"

// 初期化
ComboAttackRoot::ComboAttackRoot(Player* player)
    : BaseComboAattackBehavior("ComboAttackRoot", player) {
    Init();
}

ComboAttackRoot::~ComboAttackRoot() {}

void ComboAttackRoot::Init() {
    pPlayer_->SetHeadScale(Vector3::OneVector());
    pPlayer_->RotateReset();
    attackPatern_ = AttackPatern::NORMAL;
}

void ComboAttackRoot::Update() {
    // 現在の状態を判定
    JudgeAttackPattern();

    // 攻撃コントローラーを取得
    PlayerComboAttackController* controller = pPlayer_->GetComboAttackController();
    if (!controller) {
        return;
    }

    // 全ての攻撃データをチェック
    const auto& attacks = controller->GetAllAttacks();

    for (const auto& attackPtr : attacks) {
        if (!attackPtr) {
            continue;
        }

        auto& attackParam  = attackPtr->GetAttackParam();
        auto& triggerParam = attackParam.triggerParam;
    
        // isFirstAttackがtrueの攻撃のみ対象
        if (!triggerParam.isFirstAttack) {
            continue;
        }

        // 状態条件のチェック
        if (!CheckConditionMuch(triggerParam.condition)) {
            continue;
        }

        // 入力チェック
        bool triggered = false;

        // キーボード入力チェック
        if (KetaEngine::Input::GetInstance()->TriggerKey(FromDIKCode(triggerParam.keyBordBottom))) {
            triggered = true;
        }

        // ゲームパッド入力チェック
        if (KetaEngine::Input::IsTriggerPad(0, FromXInputButtonFlag(triggerParam.gamePadBottom))) {
            triggered = true;
        }

        // トリガーされたら攻撃開始
        if (triggered) {
            pPlayer_->ChangeComboBehavior(
                std::make_unique<DynamicComboAttack>(pPlayer_, attackPtr.get()));
            return; // 最初にマッチした攻撃を実行
        }
    }
}

bool ComboAttackRoot::CheckConditionMuch(const PlayerComboAttackData::TriggerCondition& condition) {
    switch (condition) {
    case PlayerComboAttackData::TriggerCondition::GROUND:
        // 地上のみ
        return attackPatern_ == AttackPatern::NORMAL;

    case PlayerComboAttackData::TriggerCondition::AIR:
        // 空中のみ
        return attackPatern_ == AttackPatern::JUMP;

    case PlayerComboAttackData::TriggerCondition::BOTH:
        // 両方OK
        return true;

    default:
        return false;
    }
}

void ComboAttackRoot::JudgeAttackPattern() {
    // ジャンプ中かどうかで判定
    if (dynamic_cast<PlayerJump*>(pPlayer_->GetBehavior())) {
        attackPatern_ = AttackPatern::JUMP;
    } else {
        attackPatern_ = AttackPatern::NORMAL;
    }
}

void ComboAttackRoot::Debug() {

}