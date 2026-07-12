#include "EnemyDamageReactionTakeUpper.h"
// Behavior
#include "EnemyDeath.h"
// Types
#include "Enemy/Types/BaseEnemy.h"
// Manager
#include "Enemy/EnemyManager/EnemyManager.h"
#include "Enemy/EnemyManager/DamageReaction/EnemyDamageReactionController.h"
// Player
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Components/CollisionBox/PlayerAttackCollider.h"
// Frame
#include "Frame/Frame.h"

EnemyDamageReactionTakeUpper::EnemyDamageReactionTakeUpper(
    BaseEnemy* boss,
    EnemyDamageReactionData* reactionData,
    const PlayerAttackCollider* playerCollisionInfo)
    : BaseEnemyDamageReaction("EnemyDamageReactionTakeUpper", boss) {

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

EnemyDamageReactionTakeUpper::~EnemyDamageReactionTakeUpper() {
    // GetUpフェーズのアニメーション終了コールバックは[this]をキャプチャしているため、
    // デストラクタで必ずクリアし、ダングリングポインタアクセスを防ぐ
    if (pBaseEnemy_ && pBaseEnemy_->GetAnimator()->GetAnimationObject()) {
        pBaseEnemy_->GetAnimator()->GetAnimationObject()->ClearAllAnimationEndCallbacks();
    }
}

void EnemyDamageReactionTakeUpper::Update(float deltaTime) {
    // 演出・リアクション更新
    damageRendition_.Update(deltaTime, reactionTimer_, hasPlayedRendition_);

    // 経過時間加算
    reactionTimer_ += deltaTime;

    // フェーズ更新
    if (currentPhase_) {
        currentPhase_();
    }
}

void EnemyDamageReactionTakeUpper::UpdatePhase() {
    // 水平ノックバック・打ち上げ処理
    UpdateNormal();
    UpdateTakeUpper();

    // 終了判定
    if (IsReactionFinished()) {
        OnReactionEnd();
        currentPhase_ = [this]() {
            GetUpPhase();
        };
    }
}

void EnemyDamageReactionTakeUpper::EndPhase() {
    pBaseEnemy_->BackToDamageRoot();
}

void EnemyDamageReactionTakeUpper::Debug() {
}

void EnemyDamageReactionTakeUpper::InitReaction() {
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
    Vector3 playerPos  = pPlayerCollisionInfo_->GetPlayerTransform()->GetWorldPos();
    Vector3 enemyPos   = pBaseEnemy_->GetWorldPosition();
    Vector3 direction  = enemyPos - playerPos;
    direction.y        = 0.0f;
    direction          = direction.Normalize();

    knockBackVelocity_ = direction * knockBackPower_;
    knockBackTimer_    = 0.0f;

    // バウンドパラメータの設定
    const auto& boundParam = reactionParam.boundParam;
    maxBoundCount_         = boundParam.boundNum;
    bounceDamping_         = boundParam.bounceDamping;
    initialBounceRate_     = boundParam.initialBounceRate;
    currentBoundCount_     = 0;

    // TakeUpper固有パラメータ
    const auto& takeUpperParam = reactionParam.takeUpperParam;
    floatingTime_              = takeUpperParam.floatingTime;
    hasReachedPeak_            = false;
    floatingTimer_             = 0.0f;
    hasReachedGround_          = false;

    // 吹っ飛び初速
    jumpSpeed_        = std::abs(blowYPower_);
    initialPosition_  = pBaseEnemy_->GetWorldPosition();

    takeUpperGravity_      = takeUpperParam.gravity;
    takeUpperRotateSpeed_  = takeUpperParam.rotateSpeed;
    takeUpperFallLimit_    = takeUpperParam.fallSpeedLimit;

    reactionTimer_      = 0.0f;
    hasPlayedRendition_ = false;
}

void EnemyDamageReactionTakeUpper::UpdateNormal() {
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

void EnemyDamageReactionTakeUpper::UpdateTakeUpper() {
    if (!pReactionData_) {
        return;
    }

    const auto& enemyParam = pBaseEnemy_->GetBaseInfo()->GetParameter();

    // 頂点到達前: 上昇フェーズ
    if (!hasReachedPeak_) {
        // 重力を適用しながらジャンプ
        pBaseEnemy_->Jump(jumpSpeed_, takeUpperFallLimit_, takeUpperGravity_);

        // 回転演出
        RotationUpdate();

        // 速度が0以下になったら頂点到達
        if (jumpSpeed_ <= 0.0f) {
            hasReachedPeak_ = true;
            floatingTimer_  = 0.0f;
        }
    }
    // 頂点到達後: 浮遊フェーズ
    else if (floatingTimer_ < floatingTime_) {
        floatingTimer_ += KetaEngine::Frame::DeltaTimeRate();

        // 回転演出
        RotationUpdate();
    }
    // 浮遊終了後: 落下フェーズ
    else {
        pBaseEnemy_->Fall(jumpSpeed_, takeUpperFallLimit_, takeUpperGravity_, false);

        // 回転継続
        RotationUpdate();

        // 地面着地でバウンド処理へ
        if (pBaseEnemy_->GetWorldPosition().y < enemyParam.basePosY) {
            pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);
            hasReachedGround_ = true;

            // バウンドアニメーション再生
            {
                const auto* ctrl      = pBaseEnemy_->GetBaseInfo()->GetManager()->GetDamageReactionController();
                int eType             = static_cast<int>(pBaseEnemy_->GetBaseInfo()->GetType());
                const auto& boundAnim = ctrl->GetDefaultDamageAnimationName(eType, DefaultAnimType::Bound);
                if (!boundAnim.empty()) {
                    pBaseEnemy_->GetAnimator()->PlayAnimationByName(boundAnim, false);
                }
            }

            // バウンドエフェクト
            pBaseEnemy_->ThrustRenditionInit();

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

bool EnemyDamageReactionTakeUpper::IsReactionFinished() const {
    const auto& enemyParam = pBaseEnemy_->GetBaseInfo()->GetParameter();

    if (hasReachedPeak_ && floatingTimer_ >= floatingTime_ && hasReachedGround_ &&
        currentBoundCount_ >= maxBoundCount_ &&
        pBaseEnemy_->GetWorldPosition().y <= enemyParam.basePosY &&
        std::abs(bounceSpeed_) < 0.01f) {
        return true;
    }
    return false;
}

void EnemyDamageReactionTakeUpper::GetUpPhase() {
    if (getUpFinished_) {
        EndPhase();
    }
}

void EnemyDamageReactionTakeUpper::OnReactionEnd() {
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

void EnemyDamageReactionTakeUpper::RotationUpdate() {
    // 回転演出
    Vector3 currentRotation = pBaseEnemy_->GetAnimator()->GetBodyRotation();
    pBaseEnemy_->GetAnimator()->SetBodyRotate(currentRotation + takeUpperRotateSpeed_ * KetaEngine::Frame::DeltaTimeRate());
}

void EnemyDamageReactionTakeUpper::PlayDefaultDamageAnim(int enemyType) {
    const auto* ctrl = pBaseEnemy_->GetBaseInfo()->GetManager()->GetDamageReactionController();
    const auto& anim = ctrl->GetDefaultDamageAnimationName(enemyType, DefaultAnimType::TakeUpper);
    if (!anim.empty()) {
        pBaseEnemy_->GetAnimator()->PlayAnimationByName(anim, pBaseEnemy_->GetAnimator()->GetDamageReactionAnimationIsLoop(anim));
    }
}

void EnemyDamageReactionTakeUpper::PlayDamageAnim(const std::string& animName, int enemyType) {
    if (animName == "None") {
        return;
    }
    if (animName.empty()) {
        PlayDefaultDamageAnim(enemyType);
        return;
    }
    pBaseEnemy_->GetAnimator()->PlayAnimationByName(animName, pBaseEnemy_->GetAnimator()->GetDamageReactionAnimationIsLoop(animName));
}
