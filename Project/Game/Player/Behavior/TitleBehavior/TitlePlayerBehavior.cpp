#include "TitlePlayerBehavior.h"

#include "Player/Player.h"
#include "Frame/Frame.h"
#include <imgui.h>

///=========================================================
/// 初期化
///=========================================================
TitlePlayerBehavior::TitlePlayerBehavior(Player* player)
    : BaseTitleBehavior("TitlePlayerBehavior", player) {

    // 落下アニメーション（section0のみ）を再生
    pPlayer_->TitleAnimationPlay("TitlePlayer");
}

///=========================================================
/// 更新
///=========================================================
void TitlePlayerBehavior::Update() {

    switch (phase_) {

    // ─── フェーズ1：落下 ───────────────────────────────────
    case Phase::FALL:
        if (pPlayer_->IsTitleAnimationFinished()) {
            // 着地タイミングでがれきエフェクト
            pPlayer_->GetEffects()->FallEffectRenditionInit();

            // 着地演出アニメーション開始
            pPlayer_->TitleAnimationPlay("TitlePlayerLand");
            phase_ = Phase::LAND;
        }
        break;

    // ─── フェーズ2：着地演出 ──────────────────────────────
    case Phase::LAND:
        if (pPlayer_->IsTitleAnimationFinished()) {
            // 左パンチ開始
            pPlayer_->TitleLeftHandAnimationPlay("TitleLeftHand");
            phase_ = Phase::LEFT_PUNCH;
        }
        break;

    // ─── フェーズ3：左パンチ ──────────────────────────────
    case Phase::LEFT_PUNCH:
        if (pPlayer_->IsTitleLeftHandAnimationFinished()) {
            // 右パンチ開始
            pPlayer_->TitleRightHandAnimationPlay("TitleRightHand");
            phase_ = Phase::RIGHT_PUNCH;
        }
        break;

    // ─── フェーズ4：右パンチ ──────────────────────────────
    case Phase::RIGHT_PUNCH:
        if (pPlayer_->IsTitleRightHandAnimationFinished()) {
            // 全アニメーション完了 → 待機ループ開始
            pPlayer_->TitleAnimationPlay("TitleWaiting");
            phase_    = Phase::WAITING;
            isFinish_ = true;
        }
        break;

    // ─── フェーズ5：待機ループ（無限） ────────────────────
    case Phase::WAITING:
        if (pPlayer_->IsTitleAnimationFinished()) {
            // 終了したら即再生してループ
            pPlayer_->TitleAnimationPlay("TitleWaiting");
        }
        break;
    }
}

///=========================================================
/// デバッグ
///=========================================================
void TitlePlayerBehavior::Debug() {
#ifdef _DEBUG

#endif
}
