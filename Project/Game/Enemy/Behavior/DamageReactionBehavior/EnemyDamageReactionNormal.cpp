#include "EnemyDamageReactionNormal.h"
// Manager
#include "Enemy/EnemyManager/DamageReaction/EnemyDamageReactionController.h"
#include "Enemy/EnemyManager/EnemyManager.h"
// Types
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/EntourageEnemy.h"
// Frame
#include "Frame/Frame.h"
// Player
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Components/CollisionBox/PlayerAttackCollider.h"

EnemyDamageReactionNormal::EnemyDamageReactionNormal(
    BaseEnemy* boss,
    EnemyDamageReactionData* reactionData,
    const PlayerAttackCollider* playerCollisionInfo,
    bool skipAnimation)
    : BaseEnemyDamageReaction("EnemyDamageReactionNormal", boss) {

    // ポインタ保存 
    pReactionData_        = reactionData;
    pPlayerCollisionInfo_ = playerCollisionInfo;
    skipAnimation_        = skipAnimation;

    // 初期化
    InitReaction();
    damageRendition_.Init(pBaseEnemy_, pReactionData_);

    // reactionTime=0 は即時演出
    if (totalReactionTime_ <= 0.0f) {
        damageRendition_.Update(0.0f, 0.0f, hasPlayedRendition_);
    }
}

EnemyDamageReactionNormal::~EnemyDamageReactionNormal() {
}

void EnemyDamageReactionNormal::Update(float deltaTime) {

    // 演出・リアクション更新
    damageRendition_.Update(deltaTime, reactionTimer_, hasPlayedRendition_);
    UpdateReaction();

    // 経過時間加算
    reactionTimer_ += deltaTime;

    // 終了時処理
    if (IsReactionFinished()) {
        OnReactionEnd();
        if (pBaseEnemy_) {
            pBaseEnemy_->BackToDamageRoot();
        }
    }
}

void EnemyDamageReactionNormal::Debug() {
}

void EnemyDamageReactionNormal::InitReaction() {

    // ポインタ null チェック
    if (!pPlayerCollisionInfo_ || !pBaseEnemy_) {
        return;
    }

    // 敵のタイプを取得
    int enemyType = static_cast<int>(pBaseEnemy_->GetBaseInfo()->GetType());

    // StumbleBackwards スキップ判定
    bool skipAnimation = skipAnimation_;
    if (!skipAnimation) {
        if (EntourageEnemy* pEntourageEnemy = dynamic_cast<EntourageEnemy*>(pBaseEnemy_)) {
            skipAnimation = pEntourageEnemy->IsInStumblePhase();
        }
    }

    if (!pReactionData_) {
        if (!skipAnimation) { PlayDefaultDamageAnim(enemyType); }
        totalReactionTime_ = kDefaultKnockBackTime;
    } else {
        const auto& reactionParam = pReactionData_->GetReactionParam();
        if (!skipAnimation) {
            PlayDamageAnim(reactionParam.damageAnimationNames[enemyType], enemyType);
        }
        totalReactionTime_ = reactionParam.normalParam.knockBackTime;
    }
    // ノックバック強度を取得
    const auto* comboData = pPlayerCollisionInfo_->GetComboAttackData();
    if (comboData) {
        knockBackPower_ = comboData->GetAttackParam().knockBackPower;
    } else {
        knockBackPower_ = 0.0f;
    }

    // 方向ベクトルを計算
    const auto* playerTransform = pPlayerCollisionInfo_->GetPlayerTransform();
    if (playerTransform) {
        Vector3 playerPos = playerTransform->GetWorldPos();
        Vector3 enemyPos  = pBaseEnemy_->GetWorldPosition();
        Vector3 direction = enemyPos - playerPos;
        direction.y       = 0.0f;
        direction         = direction.Normalize();

        // ノックバック速度ベクトルを設定
        knockBackVelocity_ = direction * knockBackPower_;
    } else {
        // プレイヤートランスフォームが無ければノックバックなし
        knockBackVelocity_ = Vector3::ZeroVector();
    }

    knockBackTimer_     = 0.0f;
    reactionTimer_      = 0.0f;
    hasPlayedRendition_ = false;
}

void EnemyDamageReactionNormal::UpdateReaction() {
    // パラメータ取得
    float knockBackTime    = kDefaultKnockBackTime;
    float knockBackDamping = kDefaultKnockBackDamping;
    if (pReactionData_) {
        knockBackTime    = pReactionData_->GetReactionParam().normalParam.knockBackTime;
        knockBackDamping = pReactionData_->GetReactionParam().normalParam.knockBackDamping;
    }

    // タイマー更新
    knockBackTimer_ += KetaEngine::Frame::DeltaTimeRate();

    // ノックバック中のみ移動・重力を適用
    if (knockBackTimer_ < knockBackTime) {
        float dampingFactor = 1.0f - (knockBackDamping * KetaEngine::Frame::DeltaTimeRate());
        knockBackVelocity_ *= dampingFactor;

        Vector3 newPos = pBaseEnemy_->GetWorldPosition() + knockBackVelocity_ * KetaEngine::Frame::DeltaTimeRate();
        pBaseEnemy_->SetWorldPositionX(newPos.x);
        pBaseEnemy_->SetWorldPositionZ(newPos.z);

        // 滞空中の場合、重力を適用して落下させる
        const auto& enemyParam = pBaseEnemy_->GetBaseInfo()->GetParameter();
        float currentY         = pBaseEnemy_->GetWorldPosition().y;
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
    if (pBaseEnemy_) {
        pBaseEnemy_->GetAnimator()->RotateInit();
        const auto& enemyParam = pBaseEnemy_->GetBaseInfo()->GetParameter();
        pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);
    }
}

void EnemyDamageReactionNormal::PlayDefaultDamageAnim(int enemyType) {
    const auto* ctrl = pBaseEnemy_->GetBaseInfo()->GetManager()->GetDamageReactionController();
    const auto& anim = ctrl->GetDefaultDamageAnimationName(enemyType, DefaultAnimType::Normal);
    if (!anim.empty()) {
        pBaseEnemy_->GetAnimator()->PlayAnimationByName(anim, pBaseEnemy_->GetAnimator()->GetDamageReactionAnimationIsLoop(anim));
    }
}

void EnemyDamageReactionNormal::PlayDamageAnim(const std::string& animName, int enemyType) {
    if (animName == "None") {
        return;
    }
    if (animName.empty()) {
        PlayDefaultDamageAnim(enemyType);
        return;
    }
    pBaseEnemy_->GetAnimator()->PlayAnimationByName(animName, pBaseEnemy_->GetAnimator()->GetDamageReactionAnimationIsLoop(animName));
}
