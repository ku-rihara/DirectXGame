#include "EnemyAttack.h"
#include "EnemyWait.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include <imgui.h>

// 初期化
EnemyAttack::EnemyAttack(BaseEnemy* boss)
    : BaseEnemyBehavior("EnemyAttack", boss) {

    // 初期ステートは予備動作
    ChangeState(AttackState::ANTICIPATION);
}

EnemyAttack::~EnemyAttack() {
}

void EnemyAttack::Update() {
    // 現在のステート処理を実行
    ExecuteCurrentState();
}

void EnemyAttack::ChangeState(AttackState newState) {
    currentState_ = newState;
    stateTimer_ = 0.0f;

    // ステートに応じた処理関数を設定
    switch (currentState_) {
    case AttackState::ANTICIPATION:
        currentStateFunction_ = [this]() {
            // 前隙開始
            pBaseEnemy_->SetIsInAnticipation(true);

            // 予備動作処理
            stateTimer_ += KetaEngine::Frame::DeltaTimeRate();

            // 攻撃戦略の予備動作を実行
            if (auto* strategy = pBaseEnemy_->GetAttackStrategy()) {
                strategy->Anticipation();

                // 予備動作完了チェック
                if (strategy->IsAnticipationFinished()) {
                    // 前隙終了
                    pBaseEnemy_->SetIsInAnticipation(false);
                    ChangeState(AttackState::START);
                }
            }
        };
        break;

    case AttackState::START:
        currentStateFunction_ = [this]() {
            // 攻撃開始処理
            if (auto* strategy = pBaseEnemy_->GetAttackStrategy()) {
                strategy->Start();
            }

            // すぐに更新ステートへ
            ChangeState(AttackState::UPDATE);
        };
        break;

    case AttackState::UPDATE:
        currentStateFunction_ = [this]() {
            // 攻撃更新処理
            stateTimer_ += KetaEngine::Frame::DeltaTimeRate();

            // 攻撃戦略の攻撃更新を実行
            if (auto* strategy = pBaseEnemy_->GetAttackStrategy()) {
                strategy->Update();

                // 攻撃完了チェック
                if (strategy->IsFinished()) {
                    ChangeState(AttackState::FINISH);
                }
            }
        };
        break;

    case AttackState::FINISH:
        currentStateFunction_ = [this]() {
            // 念のため前隙フラグをオフ
            pBaseEnemy_->SetIsInAnticipation(false);

            // 終了処理
            if (auto* strategy = pBaseEnemy_->GetAttackStrategy()) {
                strategy->Finish();
            }

            // Waitステートに戻る
            pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
        };
        break;
    }
}

void EnemyAttack::ExecuteCurrentState() {
    if (currentStateFunction_) {
        currentStateFunction_();
    }
}

void EnemyAttack::Debug() {
#ifdef _DEBUG
    if (ImGui::TreeNode("EnemyAttack")) {
        const char* stateNames[] = {"ANTICIPATION", "START", "UPDATE", "FINISH"};
        ImGui::Text("Current State: %s", stateNames[static_cast<int>(currentState_)]);
        ImGui::Text("State Timer: %.2f", stateTimer_);
        ImGui::TreePop();
    }
#endif
}
