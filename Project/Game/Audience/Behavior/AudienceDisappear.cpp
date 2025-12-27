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

    isAnimationEnd_ = false;

     pAudience_->GetObjAnimation()->SetAnimationEndCallback([]() {
       
    });
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
    pAudience_->GetObjAnimation()->ChangeAnimation("AudienceDisAppear");

    // 次のフェーズへ
    currentPhase_ = [this]() {
        AnimationPlaying();
    };
}

void AudienceDisappear::AnimationPlaying() {
    // アニメーション再生処理

    if (!isAnimationEnd_) {
        return;
    }

    pAudience_->GetObject3D()->transform_.PlayObjEaseAnimation("Audience", "AudienceAppear");

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