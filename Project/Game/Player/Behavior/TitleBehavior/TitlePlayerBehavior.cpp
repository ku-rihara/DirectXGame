#include "TitlePlayerBehavior.h"

#include "Frame/Frame.h"
#include "Player/Player.h"

///=========================================================
/// 初期化
///=========================================================
TitlePlayerBehavior::TitlePlayerBehavior(Player* player)
    : BaseTitleBehavior("TitlePlayerBehavior", player) {

    // 落下アニメーションを再生
    pPlayer_->GetPlayerAnimator().PlayTitleBodyAnimation("TitlePlayer");
    // タイトルアニメーションのセットアップ
    SetUpAnimations();
}

///=========================================================
/// 更新
///=========================================================
void TitlePlayerBehavior::Update() {
    // アニメーション更新
    auto it = animationPhase_.find(phase_);
    if (it != animationPhase_.end()) {
        it->second();
    }
}

///=========================================================
/// フェーズごとのアニメーション関数
///=========================================================
void TitlePlayerBehavior::UpdateFall() {
    if (!pPlayer_->GetPlayerAnimator().IsTitleBodyAnimationFinished()) {
        return;
    }
    // 着地タイミングでがれきエフェクト
    pPlayer_->GetEffects()->FallEffectStart();

    // 着地演出アニメーションに切り替え
    pPlayer_->GetPlayerAnimator().PlayTitleBodyAnimation("TitlePlayerLand");
    phase_ = Phase::LAND;
}

void TitlePlayerBehavior::UpdateLand() {

    // アニメーションを完了するまで早期リターン
    if (!pPlayer_->GetPlayerAnimator().IsTitleBodyAnimationFinished()) {
        return;
    }
    // 左パンチアニメーションに切り替え
    pPlayer_->GetPlayerAnimator().PlayTitleBodyAnimation("TitlePlayerPunchScaling");
    pPlayer_->GetPlayerAnimator().PlayTitleLeftHandAnimation("TitleLeftHand");
    phase_ = Phase::LEFT_PUNCH;
}

void TitlePlayerBehavior::UpdateLeftPunch() {

    // アニメーションを完了するまで早期リターン
    if (!pPlayer_->GetPlayerAnimator().IsTitleLeftHandAnimationFinished()) {
        return;
    }
    // 右パンチアニメーションに切り替え
    pPlayer_->GetPlayerAnimator().PlayTitleBodyAnimation("TitlePlayerPunchScaling");
    pPlayer_->GetPlayerAnimator().PlayTitleRightHandAnimation("TitleRightHand");
    phase_ = Phase::RIGHT_PUNCH;
}

void TitlePlayerBehavior::UpdateRightPunch() {

    // アニメーションを完了するまで早期リターン
    if (!pPlayer_->GetPlayerAnimator().IsTitleRightHandAnimationFinished()) {
        return;
    }
    // 待機アニメーションへの遷移
    pPlayer_->GetPlayerAnimator().PlayTitleBodyAnimation("TitleWaiting");
    phase_    = Phase::WAITING;
    isFinish_ = true;
}
void TitlePlayerBehavior::UpdateWaiting() {
    // アニメーションを完了するまで早期リターン
    if (!pPlayer_->GetPlayerAnimator().IsTitleBodyAnimationFinished()) {
        return;
    }
    // 終了したら再生してループ
    pPlayer_->GetPlayerAnimator().PlayTitleBodyAnimation("TitleWaiting");
}

void TitlePlayerBehavior::SetUpAnimations() {
    // フェーズごとのアニメーションのセットアップ
    animationPhase_[Phase::FALL] = [this] {
        UpdateFall();
    };

    animationPhase_[Phase::LAND] = [this] {
        UpdateLand();
    };

    animationPhase_[Phase::LEFT_PUNCH] = [this] {
        UpdateLeftPunch();
    };

    animationPhase_[Phase::RIGHT_PUNCH] = [this] {
        UpdateRightPunch();
    };

    animationPhase_[Phase::WAITING] = [this] {
        UpdateWaiting();
    };
}

///=========================================================
/// デバッグ
///=========================================================
void TitlePlayerBehavior::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

#endif
}
