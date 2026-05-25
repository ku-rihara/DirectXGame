#include "EnemyDamageRendition.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/EnemyManager.h"
#include "Enemy/DamageReaction/EnemyDamageReactionController.h"
#include "Frame/Frame.h"

void EnemyDamageRendition::Init(BaseEnemy* enemy, EnemyDamageReactionData* reactionData) {
    pBaseEnemy_    = enemy;
    pReactionData_ = reactionData;
    Reset();
}

void EnemyDamageRendition::Reset() {
    currentTime_ = 0.0f;
}

void EnemyDamageRendition::Update(
    float deltaTime,
    float reactionTimer,
    bool& hasPlayedRendition) {

    if (!pBaseEnemy_) {
        return;
    }

    currentTime_ += deltaTime;

    // すでに再生済みの場合は何もしない
    if (hasPlayedRendition) {
        return;
    }

    const EnemyDamageRenditionData* rendition = pReactionData_ ? pReactionData_->GetRendition() : nullptr;

    std::string easeFileName = "None";
    float easeTiming = 0.0f;
    std::string particleFileName = "None";
    float particleTiming = 0.0f;

    if (rendition) {
        easeFileName = rendition->GetObjAnimationParam().fileName;
        easeTiming = rendition->GetObjAnimationParam().startTiming;
        particleFileName = rendition->GetParticleEffectParam().fileName;
        particleTiming = rendition->GetParticleEffectParam().startTiming;
    } else {
        // デフォルトとして空文字を指定し、下のロジックでデフォルト名を引かせる
        easeFileName = "";
        particleFileName = "";
    }

    // オブジェクトアニメーションの再生
    if (easeFileName != "None") {
        if (easeFileName.empty()) {
            const auto* controller = pBaseEnemy_->GetBaseInfo()->GetManager()->GetDamageReactionController();
            int enemyType = static_cast<int>(pBaseEnemy_->GetBaseInfo()->GetType());
            easeFileName = controller->GetDefaultObjEaseAnimationName(enemyType);
            easeTiming = controller->GetDefaultObjEaseAnimationStartTiming(enemyType);
        }

        if (reactionTimer >= easeTiming && !easeFileName.empty() && easeFileName != "None") {
            if (pBaseEnemy_->GetAnimator()->GetAnimationObject()) {
                pBaseEnemy_->GetAnimator()->GetAnimationObject()->transform_.PlayObjEaseAnimation(easeFileName, "Enemy");
            }
        }
    }

    // パーティクルエフェクトの再生
    if (particleFileName != "None") {
        if (particleFileName.empty()) {
            const auto* controller = pBaseEnemy_->GetBaseInfo()->GetManager()->GetDamageReactionController();
            int enemyType = static_cast<int>(pBaseEnemy_->GetBaseInfo()->GetType());
            particleFileName = controller->GetDefaultParticleEffectName(enemyType);
            particleTiming = 0.0f;
        }

        if (reactionTimer >= particleTiming && !particleFileName.empty() && particleFileName != "None") {
            if (pBaseEnemy_->GetEnemyEffects()) {
                pBaseEnemy_->GetEnemyEffects()->Emit(particleFileName);
            }
        }
    }

    // 両方の演出のタイミングをチェックして、いずれかが再生されたら完了
    bool animReady = false;
    if (easeFileName.empty() || easeFileName == "None") {
        animReady = true;
    } else {
        animReady = reactionTimer >= easeTiming;
    }

    bool particleReady = false;
    if (particleFileName.empty() || particleFileName == "None") {
        particleReady = true;
    } else {
        particleReady = reactionTimer >= particleTiming;
    }

    if (animReady && particleReady) {
        hasPlayedRendition = true;
    }
}