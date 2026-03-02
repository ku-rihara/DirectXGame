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

    if (!pBaseEnemy_ || !pReactionData_) {
        return;
    }

    currentTime_ += deltaTime;

    // 演出データの取得
    const EnemyDamageRenditionData* rendition = pReactionData_->GetRendition();

    // 演出データが存在しない場合は何もしない
    if (!rendition) {
        return;
    }

    // すでに再生済みの場合は何もしない
    if (hasPlayedRendition) {
        return;
    }

    // オブジェクトアニメーションの再生
    const auto& animParam = rendition->GetObjAnimationParam();
    if (animParam.fileName != "None") {
        std::string easeFileName = animParam.fileName;
        float easeTiming = animParam.startTiming;

        // ファイル名が空の場合はデフォルトを使用
        if (easeFileName.empty()) {
            const auto* controller = pBaseEnemy_->GetManager()->GetDamageReactionController();
            int enemyType = static_cast<int>(pBaseEnemy_->GetType());
            easeFileName = controller->GetDefaultObjEaseAnimationName(enemyType);
            easeTiming = controller->GetDefaultObjEaseAnimationStartTiming(enemyType);
        }

        if (reactionTimer >= easeTiming && !easeFileName.empty() && easeFileName != "None") {
            if (pBaseEnemy_->GetAnimationObject()) {
                pBaseEnemy_->GetAnimationObject()->transform_.PlayObjEaseAnimation(easeFileName, "Enemy");
            }
        }
    }

    // パーティクルエフェクトの再生
    const auto& particleParam = rendition->GetParticleEffectParam();
    if (reactionTimer >= particleParam.startTiming && !particleParam.fileName.empty() && particleParam.fileName != "None") {
        if (pBaseEnemy_->GetEnemyEffects()) {
            pBaseEnemy_->GetEnemyEffects()->Emit(particleParam.fileName);
        }
    }

    // 両方の演出のタイミングをチェックして、いずれかが再生されたら完了
    // オブジェクトアニメーションの完了判定（デフォルト考慮）
    bool animReady = false;
    if (animParam.fileName == "None") {
        animReady = true;
    } else if (animParam.fileName.empty()) {
        const auto* ctrl = pBaseEnemy_->GetManager()->GetDamageReactionController();
        int enemyType = static_cast<int>(pBaseEnemy_->GetType());
        const auto& defEase = ctrl->GetDefaultObjEaseAnimationName(enemyType);
        if (defEase.empty() || defEase == "None") {
            animReady = true;
        } else {
            animReady = reactionTimer >= ctrl->GetDefaultObjEaseAnimationStartTiming(enemyType);
        }
    } else {
        animReady = reactionTimer >= animParam.startTiming;
    }
    bool particleReady = particleParam.fileName.empty() || particleParam.fileName == "None" || reactionTimer >= particleParam.startTiming;

    if (animReady && particleReady) {
        hasPlayedRendition = true;
    }
}