#include "StrongEnemyAttackStrategy.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include <cmath>

StrongEnemyAttackStrategy::StrongEnemyAttackStrategy(BaseEnemy* enemy)
    : IEnemyAttackStrategy(enemy),
      attackDirection_(Vector3::ZeroVector()),
      currentMoveDistance_(0.0f),
      targetMoveDistance_(0.0f) {}

void StrongEnemyAttackStrategy::Anticipation() {
    // 予備動作エフェクトを一度だけ再生
    if (!hasPlayedAnticipationEffect_) {
        hasPlayedAnticipationEffect_ = true;
        if (pEnemy_->GetEnemyEffects()) {
            pEnemy_->GetEnemyEffects()->Emit("AnticipationEffectStrong");
        }
    }

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

    // 攻撃時の移動パラメータを設定
    targetMoveDistance_ = attackParam.attackMoveDistance;
    currentMoveDistance_ = 0.0f;

    // 敵の向いている方向（Y軸回転から計算）
    float rotationY = pEnemy_->GetBodyRotation().y;
    attackDirection_.x = std::sin(rotationY);
    attackDirection_.y = 0.0f;
    attackDirection_.z = std::cos(rotationY);
    attackDirection_ = attackDirection_.Normalize();

    // 攻撃エフェクト再生
    if (pEnemy_->GetEnemyEffects()) {
        pEnemy_->GetEnemyEffects()->Emit("AttackEffectStrong");
    }
}

void StrongEnemyAttackStrategy::Update() {
    // 攻撃時の前方移動
    const auto& attackParam = pEnemy_->GetParameter().attackParam;
    if (currentMoveDistance_ < targetMoveDistance_) {
        float moveAmount = attackParam.attackMoveSpeed * KetaEngine::Frame::DeltaTimeRate();

        // 目標距離を超えないように調整
        if (currentMoveDistance_ + moveAmount > targetMoveDistance_) {
            moveAmount = targetMoveDistance_ - currentMoveDistance_;
        }

        // 位置を更新
        Vector3 currentPos = pEnemy_->GetWorldPosition();
        currentPos += attackDirection_ * moveAmount;
        pEnemy_->SetWorldPositionX(currentPos.x);
        pEnemy_->SetWorldPositionZ(currentPos.z);

        currentMoveDistance_ += moveAmount;
    }

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
}

void StrongEnemyAttackStrategy::Reset() {
    // 基底クラスのリセット
    IEnemyAttackStrategy::Reset();

    // 移動パラメータのリセット
    attackDirection_ = Vector3::ZeroVector();
    currentMoveDistance_ = 0.0f;
    targetMoveDistance_ = 0.0f;
}
