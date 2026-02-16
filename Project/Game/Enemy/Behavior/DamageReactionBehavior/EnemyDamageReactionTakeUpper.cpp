#include "EnemyDamageReactionTakeUpper.h"
#include "EnemyDamageReactionRoot.h"
#include "EnemyDeath.h"
#include "Player/CollisionBox/PlayerAttackCollisionBox.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/EnemyManager.h"
#include "Enemy/DamageReaction/EnemyDamageReactionController.h"
#include "Player/Player.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Frame/Frame.h"
#include <cmath>

EnemyDamageReactionTakeUpper::EnemyDamageReactionTakeUpper(
    BaseEnemy* boss,
    EnemyDamageReactionData* reactionData,
    const PlayerAttackCollisionBox* playerCollisionInfo)
    : BaseEnemyDamageReaction("EnemyDamageReactionTakeUpper", boss) {

    pReactionData_ = reactionData;
    pPlayerCollisionInfo_ = playerCollisionInfo;

    InitReaction();
    damageRendition_.Init(pBaseEnemy_, pReactionData_);

    // 初期フェーズを設定
    currentPhase_ = [this]() { UpdatePhase(); };
}

EnemyDamageReactionTakeUpper::~EnemyDamageReactionTakeUpper() {
}

void EnemyDamageReactionTakeUpper::Update(float deltaTime) {
    if (!pReactionData_) {
        return;
    }

    damageRendition_.Update(deltaTime, reactionTimer_, hasPlayedRendition_);
    reactionTimer_ += deltaTime;

    if (currentPhase_) {
        currentPhase_();
    }
}

void EnemyDamageReactionTakeUpper::UpdatePhase() {
    UpdateNormal();
    UpdateTakeUpper();

    if (IsReactionFinished()) {
        OnReactionEnd();
        endType_ = EndType::BackToRoot;
        currentPhase_ = [this]() { EndPhase(); };
    }
}

void EnemyDamageReactionTakeUpper::EndPhase() {
    if (endType_ == EndType::Death) {
        pBaseEnemy_->ChangeDamageReactionBehavior(std::make_unique<EnemyDeath>(pBaseEnemy_));
    } else {
        pBaseEnemy_->BackToDamageRoot();
    }
}

void EnemyDamageReactionTakeUpper::Debug() {
}

void EnemyDamageReactionTakeUpper::InitReaction() {
    if (!pReactionData_ || !pPlayerCollisionInfo_) {
        return;
    }

    const auto& reactionParam = pReactionData_->GetReactionParam();

    // ダメージアニメーションを再生
    if (reactionParam.damageAnimationName == "None") {
        // "None"が設定されている場合は何もしない
    } else if (reactionParam.damageAnimationName.empty()) {
        // 空の場合はデフォルトアニメーションを再生
        const auto* controller = pBaseEnemy_->GetManager()->GetDamageReactionController();
        const auto& defaultAnim = controller->GetDefaultDamageAnimationName();
        if (!defaultAnim.empty()) {
            pBaseEnemy_->PlayAnimationByName(defaultAnim, false);
        }
    } else {
        pBaseEnemy_->PlayAnimationByName(reactionParam.damageAnimationName, false);
    }

    blowYPower_ = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().blowYPower;
    knockBackPower_ = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().knockBackPower;

    // プレイヤーから敵への方向ベクトルを計算
    Vector3 playerPos = pPlayerCollisionInfo_->GetPlayerTransform()->GetWorldPos();
    Vector3 enemyPos = pBaseEnemy_->GetWorldPosition();
    Vector3 direction = enemyPos - playerPos;
    direction.y = 0.0f;
    direction = direction.Normalize();

    knockBackVelocity_ = direction * knockBackPower_;
    knockBackTimer_ = 0.0f;

    // バウンドパラメータの設定
    const auto& boundParam = reactionParam.boundParam;
    maxBoundCount_ = boundParam.boundNum;
    bounceDamping_ = boundParam.bounceDamping;
    initialBounceRate_ = boundParam.initialBounceRate;
    currentBoundCount_ = 0;

    // TakeUpper固有のパラメータ
    const auto& takeUpperParam = reactionParam.takeUpperParam;
    floatingTime_ = takeUpperParam.floatingTime;
    hasReachedPeak_ = false;
    floatingTimer_ = 0.0f;
    hasReachedGround_ = false;

    // 吹っ飛び速度
    jumpSpeed_ = std::abs(blowYPower_);
    initialPosition_ = pBaseEnemy_->GetWorldPosition();

    takeUpperGravity_ = takeUpperParam.gravity;
    takeUpperRotateSpeed_ = takeUpperParam.rotateSpeed;
    takeUpperFallLimit_ = takeUpperParam.fallSpeedLimit;

    reactionTimer_ = 0.0f;
    hasPlayedRendition_ = false;
}

void EnemyDamageReactionTakeUpper::UpdateNormal() {
    if (!pReactionData_) {
        return;
    }

    const auto& param = pReactionData_->GetReactionParam().normalParam;

    knockBackTimer_ += KetaEngine::Frame::DeltaTimeRate();

    if (knockBackTimer_ < param.knockBackTime) {
        float dampingFactor = 1.0f - (param.knockBackDamping * KetaEngine::Frame::DeltaTimeRate());
        knockBackVelocity_ *= dampingFactor;

        Vector3 newPos = pBaseEnemy_->GetWorldPosition() + knockBackVelocity_ * KetaEngine::Frame::DeltaTimeRate();
        pBaseEnemy_->SetWorldPositionX(newPos.x);
        pBaseEnemy_->SetWorldPositionZ(newPos.z);
    }
}

void EnemyDamageReactionTakeUpper::UpdateTakeUpper() {
    if (!pReactionData_) {
        return;
    }

    const auto& enemyParam = pBaseEnemy_->GetParameter();

    // 頂点に到達していない場合は上昇
    if (!hasReachedPeak_) {
        // 重力を適用しながらジャンプ
        pBaseEnemy_->Jump(jumpSpeed_, takeUpperFallLimit_, takeUpperGravity_);

        // 回転演出
        RotationUpdate();

        // 速度が0以下になったら頂点到達
        if (jumpSpeed_ <= 0.0f) {
            hasReachedPeak_ = true;
            floatingTimer_ = 0.0f;
        }
    }
    // 頂点到達後、浮遊時間中
    else if (floatingTimer_ < floatingTime_) {
        floatingTimer_ += KetaEngine::Frame::DeltaTimeRate();

        // 回転演出
        RotationUpdate();
    }
    // 浮遊時間終了後、落下
    else {
        // 落下中
        pBaseEnemy_->Fall(jumpSpeed_, takeUpperFallLimit_, takeUpperGravity_, false);

        // 回転継続
        RotationUpdate();

        // 地面に着地したらバウンド処理へ
        if (pBaseEnemy_->GetWorldPosition().y < enemyParam.basePosY) {
            pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);
            hasReachedGround_ = true;

            // バウンドエフェクト
            pBaseEnemy_->ThrustRenditionInit();

            // HPが0以下の場合、最初の着地で死亡処理
            if (pBaseEnemy_->GetIsDeathPending()) {
                endType_ = EndType::Death;
                currentPhase_ = [this]() { EndPhase(); };
                return;
            }

            // 最初の着地時のバウンド速度を設定
            bounceSpeed_ = std::abs(blowYPower_) * initialBounceRate_;
        }
    }

    // 地面到達後のバウンド処理
    if (hasReachedGround_) {
        UpdateBounce(enemyParam.basePosY, takeUpperGravity_);
    }
}

void EnemyDamageReactionTakeUpper::UpdateBounce(float basePosY, float gravity) {
    if (currentBoundCount_ >= maxBoundCount_) {
        return;
    }

    // 重力を適用
    bounceSpeed_ -= gravity * KetaEngine::Frame::DeltaTimeRate();

    // 位置更新
    Vector3 currentPos = pBaseEnemy_->GetWorldPosition();
    currentPos.y += bounceSpeed_ * KetaEngine::Frame::DeltaTimeRate();
    pBaseEnemy_->SetWorldPositionY(currentPos.y);

    // 回転処理
    RotationUpdate();

    // 地面に着地したら次のバウンド
    if (currentPos.y <= basePosY) {
        pBaseEnemy_->SetWorldPositionY(basePosY);
        currentBoundCount_++;

        float nextBounceSpeed = std::abs(bounceSpeed_) * bounceDamping_;

        if (currentBoundCount_ >= maxBoundCount_) {
            bounceSpeed_ = 0.0f;
        } else {
            bounceSpeed_ = nextBounceSpeed;
            pBaseEnemy_->ThrustRenditionInit();
        }
    }
}

bool EnemyDamageReactionTakeUpper::IsReactionFinished() const {
    const auto& enemyParam = pBaseEnemy_->GetParameter();

    if (hasReachedPeak_ && floatingTimer_ >= floatingTime_ && hasReachedGround_ &&
        currentBoundCount_ >= maxBoundCount_ &&
        pBaseEnemy_->GetWorldPosition().y <= enemyParam.basePosY &&
        std::abs(bounceSpeed_) < 0.01f) {
        return true;
    }
    return false;
}

void EnemyDamageReactionTakeUpper::OnReactionEnd() {
    pBaseEnemy_->RotateInit();
    const auto& enemyParam = pBaseEnemy_->GetParameter();
    pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);
}

void EnemyDamageReactionTakeUpper::RotationUpdate() {
    // 回転演出
    Vector3 currentRotation = pBaseEnemy_->GetBodyRotation();
    pBaseEnemy_->SetBodyRotate(currentRotation + takeUpperRotateSpeed_ * KetaEngine::Frame::DeltaTimeRate());
}