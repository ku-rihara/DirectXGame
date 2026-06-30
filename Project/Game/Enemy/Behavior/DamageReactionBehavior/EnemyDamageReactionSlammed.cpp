#include "EnemyDamageReactionSlammed.h"
// Manager
#include "Enemy/EnemyManager/DamageReaction/EnemyDamageReactionController.h"
#include "Enemy/EnemyManager/EnemyManager.h"
// Type
#include "Enemy/Types/BaseEnemy.h"
// Behavior
#include "EnemyDeath.h"
// Frame
#include "Frame/Frame.h"
// Player
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Components/CollisionBox/PlayerAttackCollider.h"
#include "Player/Player.h"

EnemyDamageReactionSlammed::EnemyDamageReactionSlammed(
    BaseEnemy* boss,
    EnemyDamageReactionData* reactionData,
    const PlayerAttackCollider* playerCollisionInfo)
    : BaseEnemyDamageReaction("EnemyDamageReactionSlammed", boss) {

    // ポインタ保存
    pReactionData_        = reactionData;
    pPlayerCollisionInfo_ = playerCollisionInfo;

    // 初期化
    InitReaction();
    damageRendition_.Init(pBaseEnemy_, pReactionData_);

    // 初期フェーズを設定
    currentPhase_ = [this]() {
        UpdatePhase();
    };
}

EnemyDamageReactionSlammed::~EnemyDamageReactionSlammed() {
    if (pBaseEnemy_ && pBaseEnemy_->GetAnimator()->GetAnimationObject()) {
        pBaseEnemy_->GetAnimator()->GetAnimationObject()->ClearAllAnimationEndCallbacks();
    }
}

void EnemyDamageReactionSlammed::Update(float deltaTime) {
    // 演出・リアクション更新
    damageRendition_.Update(deltaTime, reactionTimer_, hasPlayedRendition_);

    // 経過時間加算
    reactionTimer_ += deltaTime;

    // フェーズ更新
    if (currentPhase_) {
        currentPhase_();
    }
}

void EnemyDamageReactionSlammed::UpdatePhase() {
    // 水平ノックバック・叩きつけ処理
    UpdateNormal();
    UpdateSlammed();

    // 死亡予約済みの場合は起き上がりフェーズに移行しない
    if (pBaseEnemy_->GetIsDeathPending()) {
        return;
    }

    // 終了判定
    if (IsReactionFinished()) {
        OnReactionEnd();
        endType_      = EndType::BackToRoot;
        currentPhase_ = [this]() {
            GetUpPhase();
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
    // null チェック
    if (!pReactionData_ || !pPlayerCollisionInfo_) {
        return;
    }

    const auto& reactionParam = pReactionData_->GetReactionParam();

    // ダメージアニメーション再生
    int enemyType = static_cast<int>(pBaseEnemy_->GetBaseInfo()->GetType());
    PlayDamageAnim(reactionParam.damageAnimationNames[enemyType], enemyType);

    // 物理パラメータ取得
    blowYPower_     = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().blowYPower;
    knockBackPower_ = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().knockBackPower;

    // 方向ベクトルを計算
    pBaseEnemy_->DirectionToPlayer();
    Vector3 direction  = -pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetBaseInfo()->GetPlayer()->GetWorldPosition());
    knockBackVelocity_ = direction * knockBackPower_;
    knockBackTimer_    = 0.0f;

    // バウンドパラメータの設定
    const auto& boundParam = reactionParam.boundParam;
    maxBoundCount_         = boundParam.boundNum;
    bounceDamping_         = boundParam.bounceDamping;
    initialBounceRate_     = boundParam.initialBounceRate;
    currentBoundCount_     = 0;
    hasReachedGround_      = false;

    // 下向きの初速を設定
    bounceSpeed_ = -std::abs(blowYPower_);

    // Slammed固有パラメータ
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

    // タイマー更新
    knockBackTimer_ += KetaEngine::Frame::DeltaTimeRate();

    // ノックバック中のみ水平移動を適用
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

    const auto& enemyParam = pBaseEnemy_->GetBaseInfo()->GetParameter();

    // 地面到達前は重力で落下させる
    if (!hasReachedGround_) {
        // 重力を適用して加速
        bounceSpeed_ -= slammedGravity_ * KetaEngine::Frame::DeltaTimeRate();

        // 位置更新
        pBaseEnemy_->SetWorldPositionY(
            pBaseEnemy_->GetWorldPosition().y + bounceSpeed_ * KetaEngine::Frame::DeltaTimeRate());

        // 地面到達チェック
        if (pBaseEnemy_->GetWorldPosition().y < enemyParam.basePosY) {
            pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);
            hasReachedGround_ = true;

            // バウンドアニメーション再生
            const auto* ctrl      = pBaseEnemy_->GetBaseInfo()->GetManager()->GetDamageReactionController();
            int eType             = static_cast<int>(pBaseEnemy_->GetBaseInfo()->GetType());
            const auto& boundAnim = ctrl->GetDefaultDamageAnimationName(eType, DefaultAnimType::Bound);
            if (!boundAnim.empty()) {
                pBaseEnemy_->GetAnimator()->PlayAnimationByName(boundAnim, false);
            }

            // 地面衝突エフェクト
            pBaseEnemy_->ThrustRenditionInit();

            // 死亡予約済みの場合は1バウンド後に死亡処理
            if (pBaseEnemy_->GetIsDeathPending()) {
                endType_      = EndType::Death;
                currentPhase_ = [this]() {
                    EndPhase();
                };
                return;
            }

            // 叩きつけ速度をベースに最初のバウンド速度を設定
            bounceSpeed_ = std::abs(bounceSpeed_) * initialBounceRate_;
        }
        return;
    }

    // 地面到達後のバウンド処理
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

    // 着地したら次のバウンド
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
    const auto& enemyParam = pBaseEnemy_->GetBaseInfo()->GetParameter();

    if (hasReachedGround_ && currentBoundCount_ >= maxBoundCount_ && pBaseEnemy_->GetWorldPosition().y <= enemyParam.basePosY) {
        return true;
    }
    return false;
}

void EnemyDamageReactionSlammed::GetUpPhase() {
    if (getUpFinished_) {
        EndPhase();
    }
}

void EnemyDamageReactionSlammed::OnReactionEnd() {
    pBaseEnemy_->GetAnimator()->RotateInit();
    const auto& enemyParam = pBaseEnemy_->GetBaseInfo()->GetParameter();
    pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);

    // 起き上がりアニメーション再生
    const auto* ctrl      = pBaseEnemy_->GetBaseInfo()->GetManager()->GetDamageReactionController();
    int eType             = static_cast<int>(pBaseEnemy_->GetBaseInfo()->GetType());
    const auto& getUpAnim = ctrl->GetDefaultDamageAnimationName(eType, DefaultAnimType::GetUp);

    if (getUpAnim.empty()) {
        // GetUpアニメーションが未設定の場合はすぐに終了へ
        getUpFinished_ = true;
        return;
    }

    pBaseEnemy_->GetAnimator()->PlayAnimationByName(getUpAnim, false);

    // アニメーション終了コールバックで完了フラグを立てる
    pBaseEnemy_->GetAnimator()->GetAnimationObject()->SetAnimationEndCallback(getUpAnim, [this]() {
        getUpFinished_ = true;
    });
}

void EnemyDamageReactionSlammed::RotationUpdate() {
    // 回転演出
    Vector3 currentRotation = pBaseEnemy_->GetAnimator()->GetBodyRotation();
    pBaseEnemy_->GetAnimator()->SetBodyRotate(currentRotation + slammedRotateSpeed_ * KetaEngine::Frame::DeltaTime());
}

void EnemyDamageReactionSlammed::PlayDefaultDamageAnim(int enemyType) {
    const auto* ctrl = pBaseEnemy_->GetBaseInfo()->GetManager()->GetDamageReactionController();
    const auto& anim = ctrl->GetDefaultDamageAnimationName(enemyType, DefaultAnimType::Slammed);
    if (!anim.empty()) {
        pBaseEnemy_->GetAnimator()->PlayAnimationByName(anim, pBaseEnemy_->GetAnimator()->GetDamageReactionAnimationIsLoop(anim));
    }
}

void EnemyDamageReactionSlammed::PlayDamageAnim(const std::string& animName, int enemyType) {
    if (animName == "None") {
        return;
    }
    if (animName.empty()) {
        PlayDefaultDamageAnim(enemyType);
        return;
    }
    pBaseEnemy_->GetAnimator()->PlayAnimationByName(animName, pBaseEnemy_->GetAnimator()->GetDamageReactionAnimationIsLoop(animName));
}
