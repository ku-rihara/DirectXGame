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

    // 初期フェーズ
    currentPhase_ = [this]() {
        Init();
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

void AudienceAppear::Init() {

    // 出現状態に設定
    pAudience_->GetObjAnimation()->ChangeAnimation("AudienceAppear");
    pAudience_->GetObjAnimation()->SetLoop(true);

    currentPhase_ = [this]() {
        Start();
    };
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
    pAudience_->ChangeBehavior(std::make_unique<AudienceRoot>(pAudience_, true));
}

void AudienceAppear::Debug() {
    
}