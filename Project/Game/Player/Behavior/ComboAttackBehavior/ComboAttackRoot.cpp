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

    // -------------------------------------------------------
    // 自動実行キューのチェック（アンロック演出用）
    // -------------------------------------------------------
    auto& queue = pOwner_->GetAutoComboQueue();
    if (!queue.IsEmpty()) {
        PlayerComboAttackData* nextData = queue.Peek();
        if (nextData) {
            if (controller->IsFirstAttack(nextData->GetGroupName())) {
                using TC = PlayerComboAttackData::TriggerCondition;
                auto& triggerParam = nextData->GetAttackParam().triggerParam;

                if (CheckConditionMuch(triggerParam.condition)) {
                    // 条件OK: 強制ダッシュを解除してから実行
                    pOwner_->ClearAutoDash();
                    queue.Dequeue();
                    pOwner_->ChangeComboBehavior(
                        std::make_unique<ComboAttackAction>(pOwner_, nextData));
                    return;
                }

                // 条件が合わない場合の前処理
                if (triggerParam.condition == TC::AIR && attackPatern_ == AttackPatern::NORMAL) {
                    // 地上にいるのでジャンプを開始。次フレームでAIR条件が通る
                    pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_));
                    return;
                }

                if (triggerParam.condition == TC::DASH && !pOwner_->IsDashing()) {
                    // ダッシュ中でないので強制ダッシュを開始。次フレームでDASH条件が通る
                    pOwner_->StartAutoDash();
                    return;
                }

                if (triggerParam.condition == TC::GROUND && attackPatern_ == AttackPatern::JUMP) {
                    // 空中にいるので着地を待機（キューを保持したまま次フレームで再チェック）
                    return;
                }

                // 解決できない条件ミスマッチはキューをクリア
                pOwner_->ClearAutoDash();
                queue.Clear();

            } else {
                // 中間の攻撃がルートまで戻ってきた
                pOwner_->ClearAutoDash();
                queue.Clear();
            }
        }
    }

    // 全ての攻撃データをチェック
    const auto& attacks = controller->GetAllAttacks();

    for (const auto& attackPtr : attacks) {
        if (!attackPtr) {
            continue;
        }

        // 最初の攻撃かどうかを自動判定
        if (!controller->IsFirstAttack(attackPtr->GetGroupName())) {
            continue;
        }

        // 未解放の攻撃はスキップ
        if (!attackPtr->GetAttackParam().isUnlocked) {
            continue;
        }

        auto& attackParam  = attackPtr->GetAttackParam();
        auto& triggerParam = attackParam.triggerParam;

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
        // 地上のみ（ダッシュ中は除外）
        return attackPatern_ == AttackPatern::NORMAL && !pOwner_->IsDashing();

    case PlayerComboAttackData::TriggerCondition::AIR:
        // 空中のみ
        return attackPatern_ == AttackPatern::JUMP;

    case PlayerComboAttackData::TriggerCondition::BOTH:
        // 両方OK
        return true;

    case PlayerComboAttackData::TriggerCondition::DASH:
        // ダッシュ中のみ
        return pOwner_->IsDashing();

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