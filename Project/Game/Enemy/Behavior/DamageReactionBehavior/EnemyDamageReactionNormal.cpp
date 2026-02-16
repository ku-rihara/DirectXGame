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
    damageRendition_.Init(pBaseEnemy_, pReactionData_);
}

EnemyDamageReactionNormal::~EnemyDamageReactionNormal() {
}

void EnemyDamageReactionNormal::Update(float deltaTime) {
    if (!pReactionData_) {
        return;
    }

    damageRendition_.Update(deltaTime, reactionTimer_, hasPlayedRendition_);
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
        const auto& defaultAnim = controller->GetDefaultDamageAnimationName(enemyType, DefaultAnimType::Normal);
        if (!defaultAnim.empty()) {
            pBaseEnemy_->PlayAnimationByName(defaultAnim, false);
        }
    } else {
        pBaseEnemy_->PlayAnimationByName(animName, false);
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
    totalReactionTime_ = reactionParam.normalParam.knockBackTime;

    reactionTimer_ = 0.0f;
    hasPlayedRendition_ = false;
}

void EnemyDamageReactionNormal::UpdateNormal() {
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

bool EnemyDamageReactionNormal::IsReactionFinished() const {
    return knockBackTimer_ >= totalReactionTime_;
}

void EnemyDamageReactionNormal::OnReactionEnd() {
    pBaseEnemy_->RotateInit();
    const auto& enemyParam = pBaseEnemy_->GetParameter();
    pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);
}
