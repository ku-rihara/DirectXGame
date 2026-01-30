#include "StrongEnemyAttackStrategy.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Frame/Frame.h"

StrongEnemyAttackStrategy::StrongEnemyAttackStrategy(BaseEnemy* enemy)
    : IEnemyAttackStrategy(enemy) {}

void StrongEnemyAttackStrategy::Anticipation() {
    // タイマー更新
    anticipationTimer_ += KetaEngine::Frame::DeltaTimeRate();

    // 予備動作時間が経過したら完了フラグを立てる
    if (anticipationTimer_ >= pEnemy_->GetParameter().attackAnticipationTime) {
        isAnticipationFinished_ = true;
    }
}

bool StrongEnemyAttackStrategy::IsAnticipationFinished() const {
    return isAnticipationFinished_;
}

void StrongEnemyAttackStrategy::Start() {
    // 攻撃フラグリセット
    isAttackFinished_ = false;

    // スケールと回転を元に戻す
    pEnemy_->SetScale(pEnemy_->GetParameter().baseScale_);
    pEnemy_->SetBodyRotateX(0.0f);

    // 攻撃コリジョンを設定して開始
    const auto& attackParam = pEnemy_->GetParameter().attackParam;
    pEnemy_->GetAttackCollisionBox()->AttackStart(
        attackParam.attackValue,
        attackParam.collisionSize,
        attackParam.collisionOffset,
        attackParam.adaptTime);

    // 攻撃エフェクト再生
    if (pEnemy_->GetEnemyEffects()) {
        pEnemy_->GetEnemyEffects()->Emit("AttackEffectStrong");
    }
}

void StrongEnemyAttackStrategy::Update() {
    // コリジョンが終了したか確認
    if (pEnemy_->GetAttackCollisionBox()->GetIsFinish()) {
        isAttackFinished_ = true;
    }

    // プレイヤーにヒットした場合の処理
    if (pEnemy_->GetAttackCollisionBox()->GetIsHit()) {
        // ヒットフラグをリセット
        pEnemy_->GetAttackCollisionBox()->SetIsHit(false);
    }
}

bool StrongEnemyAttackStrategy::IsFinished() const {
    return isAttackFinished_;
}

void StrongEnemyAttackStrategy::Finish() {
    // 攻撃ステートをリセット
    Reset();

    // 攻撃から戻ってきたフラグを立てる
    pEnemy_->SetIsReturningFromAttack(true);
}
