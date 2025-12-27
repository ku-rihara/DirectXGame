#include "AudienceDisappear.h"
#include "../Audience.h"
#include "AudienceRoot.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
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
    pAudience_->GetObjAnimation()->SetLoop(false);

    pAudience_->GetObjAnimation()->SetAnimationEndCallback([this](const std::string& name) {
        if (name == "AudienceDisAppear") {
            isAnimationEnd_ = true;
        }
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

    pAudience_->GetObjAnimation()->transform_.PlayObjEaseAnimation("Audience", "AudienceDisAppear");

    // 次のフェーズへ
    currentPhase_ = [this]() {
        EasingPlaying();
    };
}

void AudienceDisappear::EasingPlaying() {
    // イージング再生処理

    if (!pAudience_->GetObjAnimation()->transform_.GetObjEaseAnimationPlayer()->IsFinished()) {
        return;
    }

    // 次のフェーズへ
    currentPhase_ = [this]() {
        End();
    };
}

void AudienceDisappear::End() {
    // 終了処理
    BaseAudienceBehavior::SetIsAppeared(false);
    pAudience_->ChangeBehavior(std::make_unique<AudienceRoot>(pAudience_));
}

void AudienceDisappear::Debug() {
}