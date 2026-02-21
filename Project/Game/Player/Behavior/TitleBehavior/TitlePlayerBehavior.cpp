#include "TitlePlayerBehavior.h"

#include "Player/Player.h"
#include "Frame/Frame.h"
#include <imgui.h>

///=========================================================
/// 初期化
///=========================================================
TitlePlayerBehavior::TitlePlayerBehavior(Player* player)
    : BaseTitleBehavior("TitlePlayerBehavior", player) {

    // 本体・左右の手のアニメーションを一括再生
    pPlayer_->TitleAnimationPlay("TitlePlayer");
    pPlayer_->TitleRightHandAnimationPlay("TitleRightHand");
    pPlayer_->TitleLeftHandAnimationPlay("TitleLeftHand");
}

///=========================================================
/// 更新
///=========================================================
void TitlePlayerBehavior::Update() {
    elapsedTime_ += KetaEngine::Frame::DeltaTime();

    // 落下終了タイミング（0.2s）でがれきパーティクルを発生させる
    if (!hasLandingParticleEmitted_ && elapsedTime_ >= 0.2f) {
        hasLandingParticleEmitted_ = true;
        pPlayer_->GetEffects()->FallEffectRenditionInit();
    }

    // 本体・左右の手、全てのアニメーションが終了したら完了フラグを立てる
    if (pPlayer_->IsTitleAnimationFinished() &&
        pPlayer_->IsTitleRightHandAnimationFinished() &&
        pPlayer_->IsTitleLeftHandAnimationFinished()) {
        isFinish_ = true;
    }
}

///=========================================================
/// デバッグ
///=========================================================
void TitlePlayerBehavior::Debug() {
#ifdef _DEBUG

#endif
}
