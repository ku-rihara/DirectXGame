#include "EnemyDamageRendition.h"
#include "Enemy/Types/BaseEnemy.h"
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
    if (reactionTimer >= animParam.startTiming && !animParam.fileName.empty() && animParam.fileName != "None") {
        if (pBaseEnemy_->GetObject3D()) {
            pBaseEnemy_->GetObject3D()->transform_.PlayObjEaseAnimation("Enemy", animParam.fileName);
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
    bool animReady     = animParam.fileName.empty() || animParam.fileName == "None" || reactionTimer >= animParam.startTiming;
    bool particleReady = particleParam.fileName.empty() || particleParam.fileName == "None" || reactionTimer >= particleParam.startTiming;

    if (animReady && particleReady) {
        hasPlayedRendition = true;
    }
}