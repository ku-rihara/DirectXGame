#include "AudienceDisappear.h"
#include "../Audience.h"
#include "AudienceRoot.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
/// frame
#include "Frame/Frame.h"
/// imGui
#include <imgui.h>

// 初期化
AudienceDisappear::AudienceDisappear(Audience* audience)
    : BaseAudienceBehavior("AudienceDisappear", audience) {

    // 初期フェーズをStartに設定
    currentPhase_ = [this]() {
        Init();
    };

    isAnimationEnd_ = false;
    pOwner_->GetObjAnimation()->SetLoop(false);

    pOwner_->GetObjAnimation()->SetAnimationEndCallback([this](const std::string& name) {
        if (name == "AudienceDisAppear") {
            isAnimationEnd_ = true;
        }
    });
}

AudienceDisappear::~AudienceDisappear() {
}

// 更新
void AudienceDisappear::Update([[maybe_unused]] float timeSpeed) {
    if (currentPhase_) {
        currentPhase_();
    }
}

void AudienceDisappear::Init() {
    currentPhase_ = [this]() {
        Start();
    };
}

void AudienceDisappear::Start() {
    pOwner_->GetObjAnimation()->ChangeAnimation("AudienceDisAppear");

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

    pOwner_->GetParticlePlayer()->Play("DisAppearEffect", "Audience");
    pOwner_->GetObjAnimation()->transform_.PlayObjEaseAnimation("AudienceDisAppear", "Audience");

    // 次のフェーズへ
    currentPhase_ = [this]() {
        EasingPlaying();
    };
}

void AudienceDisappear::EasingPlaying() {
    // イージング再生処理

    if (!pOwner_->GetObjAnimation()->transform_.GetObjEaseAnimationPlayer()->IsFinished()) {
        return;
    }

    // 次のフェーズへ
    currentPhase_ = [this]() {
        End();
    };
}

void AudienceDisappear::End() {
    // 終了処理
    pOwner_->GetObjAnimation()->transform_.scale_ = Vector3::ZeroVector();
    pOwner_->ChangeBehavior(std::make_unique<AudienceRoot>(pOwner_,false));
}

void AudienceDisappear::Debug() {
}