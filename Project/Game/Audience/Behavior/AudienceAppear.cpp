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
void AudienceAppear::Update([[maybe_unused]] float timeSpeed) {

    if (currentPhase_) {
        currentPhase_();
    }
}

void AudienceAppear::Init() {

    // 出現状態に設定
    pOwner_->GetObjAnimation()->SetIsDraw(true);
    pOwner_->GetObjAnimation()->ChangeAnimation("AudienceJump");
    pOwner_->GetObjAnimation()->SetLoop(true);

    currentPhase_ = [this]() {
        Start();
    };
}

void AudienceAppear::Start() {

    pOwner_->GetObjAnimation()->transform_.PlayObjEaseAnimation("AudienceAppear", "Audience");

    currentPhase_ = [this]() {
        Playing();
    };
}
void AudienceAppear::Playing() {
    bool isFinish = pOwner_->GetObjAnimation()->transform_.GetObjEaseAnimationPlayer()->IsFinished();

    if (!isFinish) {
        return;
    }

    currentPhase_ = [this]() {
        End();
    };
}
void AudienceAppear::End() {
    pOwner_->ChangeBehavior(std::make_unique<AudienceRoot>(pOwner_, true));
}

void AudienceAppear::Debug() {

}