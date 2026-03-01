#include "EnemyDamageReactionNormal.h"
#include "EnemyDamageReactionRoot.h"
#include "Player/CollisionBox/PlayerAttackCollisionBox.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/EnemyManager.h"
#include "Enemy/DamageReaction/EnemyDamageReactionController.h"
#include "Player/Player.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Frame/Frame.h"

EnemyDamageReactionNormal::EnemyDamageReactionNormal(
    BaseEnemy* boss,
    EnemyDamageReactionData* reactionData,
    const PlayerAttackCollisionBox* playerCollisionInfo)
    : BaseEnemyDamageReaction("EnemyDamageReactionNormal", boss) {

    pReactionData_ = reactionData;
    pPlayerCollisionInfo_ = playerCollisionInfo;

    InitReaction();
    if (pReactionData_) {
        damageRendition_.Init(pBaseEnemy_, pReactionData_);
    }
}

EnemyDamageReactionNormal::~EnemyDamageReactionNormal() {
}

void EnemyDamageReactionNormal::Update(float deltaTime) {
    if (pReactionData_) {
        damageRendition_.Update(deltaTime, reactionTimer_, hasPlayedRendition_);
    }
    UpdateNormal();

    reactionTimer_ += deltaTime;

    if (IsReactionFinished()) {
        OnReactionEnd();
        pBaseEnemy_->BackToDamageRoot();
    }
}

void EnemyDamageReactionNormal::Debug() {
}

void EnemyDamageReactionNormal::InitReaction() {
    if (!pPlayerCollisionInfo_) {
        return;
    }

    int enemyType = static_cast<int>(pBaseEnemy_->GetType());

    if (pReactionData_) {
        const auto& reactionParam = pReactionData_->GetReactionParam();

        // ダメージアニメーションを再生（敵タイプ別）
        const auto& animName = reactionParam.damageAnimationNames[enemyType];
        if (animName == "None") {
            // "None"が設定されている場合は何もしない
        } else if (animName.empty()) {
            // 空の場合はデフォルトアニメーションを再生
            const auto* controller = pBaseEnemy_->GetManager()->GetDamageReactionController();
            const auto& defaultAnim = controller->GetDefaultDamageAnimationName(enemyType, DefaultAnimType::Normal);
            if (!defaultAnim.empty()) {
                pBaseEnemy_->PlayAnimationByName(defaultAnim, pBaseEnemy_->GetDamageReactionAnimationIsLoop(defaultAnim));
            }
        } else {
            pBaseEnemy_->PlayAnimationByName(animName, pBaseEnemy_->GetDamageReactionAnimationIsLoop(animName));
        }

        totalReactionTime_ = reactionParam.normalParam.knockBackTime;
    } else {
        // リアクションデータが未設定: デフォルトアニメーションを再生
        const auto* controller = pBaseEnemy_->GetManager()->GetDamageReactionController();
        const auto& defaultAnim = controller->GetDefaultDamageAnimationName(enemyType, DefaultAnimType::Normal);
        if (!defaultAnim.empty()) {
            pBaseEnemy_->PlayAnimationByName(defaultAnim, pBaseEnemy_->GetDamageReactionAnimationIsLoop(defaultAnim));
        }
        totalReactionTime_ = kDefaultKnockBackTime;
    }

    knockBackPower_ = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().knockBackPower;

    // プレイヤーから敵への方向ベクトルを計算
    Vector3 playerPos = pPlayerCollisionInfo_->GetPlayerTransform()->GetWorldPos();
    Vector3 enemyPos = pBaseEnemy_->GetWorldPosition();
    Vector3 direction = enemyPos - playerPos;
    direction.y = 0.0f;
    direction = direction.Normalize();

    // ノックバック速度ベクトルを設定
    knockBackVelocity_ = direction * knockBackPower_;
    knockBackTimer_ = 0.0f;
    reactionTimer_ = 0.0f;
    hasPlayedRendition_ = false;
}

void EnemyDamageReactionNormal::UpdateNormal() {
    float knockBackTime    = pReactionData_ ? pReactionData_->GetReactionParam().normalParam.knockBackTime    : kDefaultKnockBackTime;
    float knockBackDamping = pReactionData_ ? pReactionData_->GetReactionParam().normalParam.knockBackDamping : kDefaultKnockBackDamping;

    knockBackTimer_ += KetaEngine::Frame::DeltaTimeRate();

    if (knockBackTimer_ < knockBackTime) {
        float dampingFactor = 1.0f - (knockBackDamping * KetaEngine::Frame::DeltaTimeRate());
        knockBackVelocity_ *= dampingFactor;

        Vector3 newPos = pBaseEnemy_->GetWorldPosition() + knockBackVelocity_ * KetaEngine::Frame::DeltaTimeRate();
        pBaseEnemy_->SetWorldPositionX(newPos.x);
        pBaseEnemy_->SetWorldPositionZ(newPos.z);

        // 滞空中の場合、重力を適用して落下させる
        const auto& enemyParam = pBaseEnemy_->GetParameter();
        float currentY = pBaseEnemy_->GetWorldPosition().y;
        if (currentY > enemyParam.basePosY) {
            float newY = currentY - enemyParam.deathGravity * KetaEngine::Frame::DeltaTimeRate();
            pBaseEnemy_->SetWorldPositionY(newY < enemyParam.basePosY ? enemyParam.basePosY : newY);
        }
    }
}

bool EnemyDamageReactionNormal::IsReactionFinished() const {
    return knockBackTimer_ >= totalReactionTime_;
}

void EnemyDamageReactionNormal::OnReactionEnd() {
    pBaseEnemy_->RotateInit();
    const auto& enemyParam = pBaseEnemy_->GetParameter();
    pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);
}
