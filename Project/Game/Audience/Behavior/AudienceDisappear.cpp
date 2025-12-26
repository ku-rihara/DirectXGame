#include "AudienceDisappear.h"
#include "../Audience.h"
/// frame
#include "Frame/Frame.h"
/// imgui
#include <imgui.h>

// 初期化
AudienceDisappear::AudienceDisappear(Audience* audience)
    : BaseAudienceBehavior("AudienceDisappear", audience) {

    // 初期フェーズをStartに設定
    currentPhase_ = [this]() {
        Start();
    };
}

AudienceDisappear::~AudienceDisappear() {
}

// 更新
void AudienceDisappear::Update() {
    if (currentPhase_) {
        currentPhase_();
    }
}

void AudienceDisappear::Start() {
    pAudience_->GetObject3D()->transform_.PlayObjEaseAnimation("Audience", "AudienceClose");

    // 次のフェーズへ
    currentPhase_ = [this]() {
        AnimationPlaying();
    };
}

void AudienceDisappear::AnimationPlaying() {
    // アニメーション再生処理

    // 次のフェーズへ
    currentPhase_ = [this]() {
        EasingPlaying();
    };
}

void AudienceDisappear::EasingPlaying() {
    // イージング再生処理

    // 次のフェーズへ
    currentPhase_ = [this]() {
        End();
    };
}

void AudienceDisappear::End() {
    // 終了処理
}

void AudienceDisappear::Debug() {
}