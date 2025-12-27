#include "AudienceAppear.h"
#include "../Audience.h"
#include "AudienceRoot.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
/// frame
#include "Frame/Frame.h"
/// imgui
#include <imgui.h>

// 初期化
AudienceAppear::AudienceAppear(Audience* audience)
    : BaseAudienceBehavior("AudienceAppear", audience) {

    // 出現状態に設定
    BaseAudienceBehavior::SetIsAppeared(true);
    pAudience_->GetObjAnimation()->SetLoop(true);

    // 初期フェーズ
    currentPhase_ = [this]() {
        Start();
    };
}

AudienceAppear::~AudienceAppear() {
}

// 更新
void AudienceAppear::Update() {

    if (currentPhase_) {
        currentPhase_();
    }
}

void AudienceAppear::Start() {
    pAudience_->GetObjAnimation()->transform_.PlayObjEaseAnimation("Audience", "AudienceAppear");
    currentPhase_ = [this]() {
        Playing();
    };
}
void AudienceAppear::Playing() {
    bool isFinish = pAudience_->GetObjAnimation()->transform_.GetObjEaseAnimationPlayer()->IsFinished();

    if (!isFinish) {
        return;
    }

     currentPhase_ = [this]() {
        End();
    };
}
void AudienceAppear::End() {
    pAudience_->ChangeBehavior(std::make_unique<AudienceRoot>(pAudience_));
}

void AudienceAppear::Debug() {
}