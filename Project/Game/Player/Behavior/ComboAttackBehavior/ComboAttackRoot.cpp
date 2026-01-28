#include "ComboAttackRoot.h"
// behavior
#include "ComboAttackAction.h"
// Input
#include "input/Input.h"
// Player
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"
#include "Player/Behavior/PlayerBehavior/PlayerJump.h"

// 初期化
ComboAttackRoot::ComboAttackRoot(Player* player)
    : BaseComboAttackBehavior("ComboAttackRoot", player) {
    Init();
}

ComboAttackRoot::~ComboAttackRoot() {}

void ComboAttackRoot::Init() {
    pOwner_->SetHeadScale(Vector3::OneVector());
    pOwner_->RotateReset();
    attackPatern_ = AttackPatern::NORMAL;
}

void ComboAttackRoot::Update(float atkSpeed) {
    atkSpeed; // 未使用

    // 現在の状態を判定
    JudgeAttackPattern();

    // 攻撃コントローラーを取得
    PlayerComboAttackController* controller = pOwner_->GetComboAttackController();
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
            pOwner_->ChangeComboBehavior(
                std::make_unique<ComboAttackAction>(pOwner_, attackPtr.get()));
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
    if (dynamic_cast<PlayerJump*>(pOwner_->GetBehavior())) {
        attackPatern_ = AttackPatern::JUMP;
    } else {
        attackPatern_ = AttackPatern::NORMAL;
    }
}

void ComboAttackRoot::Debug() {
}