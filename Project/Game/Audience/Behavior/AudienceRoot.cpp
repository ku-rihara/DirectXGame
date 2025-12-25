#include "AudienceRoot.h"
#include "../Audience.h"
#include "AudienceAppear.h"
#include "AudienceDisappear.h"
/// frame
#include "Frame/Frame.h"
/// input
#include "input/Input.h"
/// imgui
#include <imgui.h>

// 初期化
AudienceRoot::AudienceRoot(Audience* audience)
    : BaseAudienceBehavior("AudienceRoot", audience) {
}

AudienceRoot::~AudienceRoot() {
}

// 更新
void AudienceRoot::Update() {
}


void AudienceRoot::ChangeCloseMode() {
    pAudience_->ChangeBehavior(std::make_unique<AudienceDisappear>(pAudience_));
}

void AudienceRoot::ChangeAppearMode() {
    pAudience_->ChangeBehavior(std::make_unique<AudienceAppear>(pAudience_));
}


void AudienceRoot::Debug() {
}
