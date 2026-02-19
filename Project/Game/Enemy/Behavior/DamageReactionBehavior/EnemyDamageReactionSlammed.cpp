#include "EnemyDamageReactionSlammed.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/EnemyManager.h"
#include "Enemy/DamageReaction/EnemyDamageReactionController.h"
#include "EnemyDamageReactionRoot.h"
#include "EnemyDeath.h"
#include "Frame/Frame.h"
#include "Player/CollisionBox/PlayerAttackCollisionBox.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"
#include <cmath>

EnemyDamageReactionSlammed::EnemyDamageReactionSlammed(
    BaseEnemy* boss,
    EnemyDamageReactionData* reactionData,
    const PlayerAttackCollisionBox* playerCollisionInfo)
    : BaseEnemyDamageReaction("EnemyDamageReactionSlammed", boss) {

    pReactionData_        = reactionData;
    pPlayerCollisionInfo_ = playerCollisionInfo;

    InitReaction();
    damageRendition_.Init(pBaseEnemy_, pReactionData_);

    // 初期フェーズを設定
    currentPhase_ = [this]() {
        UpdatePhase();
    };
}

EnemyDamageReactionSlammed::~EnemyDamageReactionSlammed() {
}

void EnemyDamageReactionSlammed::Update(float deltaTime) {
    if (!pReactionData_) {
        return;
    }

    damageRendition_.Update(deltaTime, reactionTimer_, hasPlayedRendition_);
    reactionTimer_ += deltaTime;

    if (currentPhase_) {
        currentPhase_();
    }
}

void EnemyDamageReactionSlammed::UpdatePhase() {
    UpdateNormal();
    UpdateSlammed();

    if (IsReactionFinished()) {
        OnReactionEnd();
        endType_      = EndType::BackToRoot;
        currentPhase_ = [this]() {
            EndPhase();
        };
    }
}

void EnemyDamageReactionSlammed::EndPhase() {
    if (endType_ == EndType::Death) {
        pBaseEnemy_->ChangeDamageReactionBehavior(std::make_unique<EnemyDeath>(pBaseEnemy_));
    } else {
        pBaseEnemy_->BackToDamageRoot();
    }
}

void EnemyDamageReactionSlammed::Debug() {
}

void EnemyDamageReactionSlammed::InitReaction() {
    if (!pReactionData_ || !pPlayerCollisionInfo_) {
        return;
    }

    const auto& reactionParam = pReactionData_->GetReactionParam();

    // ダメージアニメーションを再生（敵タイプ別）
    int enemyType = static_cast<int>(pBaseEnemy_->GetType());
    const auto& animName = reactionParam.damageAnimationNames[enemyType];
    if (animName == "None") {
        // "None"が設定されている場合は何もしない
    } else if (animName.empty()) {
        // 空の場合はデフォルトアニメーションを再生
        const auto* controller = pBaseEnemy_->GetManager()->GetDamageReactionController();
        const auto& defaultAnim = controller->GetDefaultDamageAnimationName(enemyType, DefaultAnimType::Slammed);
        if (!defaultAnim.empty()) {
            pBaseEnemy_->PlayAnimationByName(defaultAnim, pBaseEnemy_->GetDamageReactionAnimationIsLoop(defaultAnim));
        }
    } else {
        pBaseEnemy_->PlayAnimationByName(animName, pBaseEnemy_->GetDamageReactionAnimationIsLoop(animName));
    }

    blowYPower_     = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().blowYPower;
    knockBackPower_ = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().knockBackPower;

    // プレイヤーから敵への方向ベクトルを計算
    pBaseEnemy_->DirectionToPlayer();
    Vector3 direction  = -pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());
    knockBackVelocity_ = direction * knockBackPower_;
    knockBackTimer_    = 0.0f;

    // バウンドパラメータの設定
    const auto& boundParam = reactionParam.boundParam;
    maxBoundCount_         = boundParam.boundNum;
    bounceDamping_         = boundParam.bounceDamping;
    initialBounceRate_     = boundParam.initialBounceRate;
    currentBoundCount_     = 0;
    hasReachedGround_      = false;

    // 下向きの速度を設定
    bounceSpeed_ = -std::abs(blowYPower_);

    // Slammedのパラメータ
    const auto& slammedParam = reactionParam.slammedParam;
    slammedGravity_          = slammedParam.gravity;
    slammedRotateSpeed_      = slammedParam.rotateSpeed;

    reactionTimer_      = 0.0f;
    hasPlayedRendition_ = false;
}

void EnemyDamageReactionSlammed::UpdateNormal() {
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

void EnemyDamageReactionSlammed::UpdateSlammed() {
    if (!pReactionData_) {
        return;
    }

    const auto& enemyParam = pBaseEnemy_->GetParameter();

    // まず地面に叩きつける
    if (!hasReachedGround_) {
        // 重力を適用して加速
        bounceSpeed_ -= slammedGravity_ * KetaEngine::Frame::DeltaTimeRate();

        // 位置更新
        pBaseEnemy_->SetWorldPositionY(
            pBaseEnemy_->GetWorldPosition().y + bounceSpeed_ * KetaEngine::Frame::DeltaTimeRate());

        // 地面に到達したか確認
        if (pBaseEnemy_->GetWorldPosition().y < enemyParam.basePosY) {
            pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);
            hasReachedGround_ = true;

            // バウンドアニメーション再生
            {
                const auto* ctrl = pBaseEnemy_->GetManager()->GetDamageReactionController();
                int eType = static_cast<int>(pBaseEnemy_->GetType());
                const auto& boundAnim = ctrl->GetDefaultDamageAnimationName(eType, DefaultAnimType::Bound);
                if (!boundAnim.empty()) {
                    pBaseEnemy_->PlayAnimationByName(boundAnim, false);
                }
            }

            // 地面衝突エフェクト
            pBaseEnemy_->ThrustRenditionInit();

            // HPが0以下の場合、1バウンド目で死亡処理
            if (pBaseEnemy_->GetIsDeathPending()) {
                endType_      = EndType::Death;
                currentPhase_ = [this]() {
                    EndPhase();
                };
                return;
            }

            // 叩きつけられたスピードをベースに最初のバウンド速度を設定
            bounceSpeed_ = std::abs(bounceSpeed_) * initialBounceRate_;
        }
        return;
    }

    if (hasReachedGround_) {
        UpdateBounce(enemyParam.basePosY, slammedGravity_);
    }
}

void EnemyDamageReactionSlammed::UpdateBounce(float basePosY, float gravity) {
    if (currentBoundCount_ >= maxBoundCount_) {
        return;
    }

    // 重力を適用
    bounceSpeed_ -= gravity * KetaEngine::Frame::DeltaTimeRate();

    // 位置更新
    Vector3 currentPos = pBaseEnemy_->GetWorldPosition();
    currentPos.y += bounceSpeed_ * KetaEngine::Frame::DeltaTimeRate();
    pBaseEnemy_->SetWorldPositionY(currentPos.y);

    // 回転
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

bool EnemyDamageReactionSlammed::IsReactionFinished() const {
    const auto& enemyParam = pBaseEnemy_->GetParameter();

    if (hasReachedGround_ && currentBoundCount_ >= maxBoundCount_ && pBaseEnemy_->GetWorldPosition().y <= enemyParam.basePosY) {
        return true;
    }
    return false;
}

void EnemyDamageReactionSlammed::OnReactionEnd() {
    pBaseEnemy_->RotateInit();
    const auto& enemyParam = pBaseEnemy_->GetParameter();
    pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);

    // 起き上がりアニメーション再生
    const auto* ctrl = pBaseEnemy_->GetManager()->GetDamageReactionController();
    int eType = static_cast<int>(pBaseEnemy_->GetType());
    const auto& getUpAnim = ctrl->GetDefaultDamageAnimationName(eType, DefaultAnimType::GetUp);
    if (!getUpAnim.empty()) {
        pBaseEnemy_->PlayAnimationByName(getUpAnim, false);
    }
}

void EnemyDamageReactionSlammed::RotationUpdate() {
    // 回転演出
    Vector3 currentRotation = pBaseEnemy_->GetBodyRotation();
    pBaseEnemy_->SetBodyRotate(currentRotation + slammedRotateSpeed_ * KetaEngine::Frame::DeltaTime());
}