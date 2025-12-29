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
AudienceRoot::AudienceRoot(Audience* audience, bool isDraw)
    : BaseAudienceBehavior("AudienceRoot", audience) {
    isDraw_ = isDraw;
}

AudienceRoot::~AudienceRoot() {
}

// 更新
void AudienceRoot::Update([[maybe_unused]] float timeSpeed) {

   pOwner_->GetObjAnimation()->SetIsDraw(isDraw_);
}


void AudienceRoot::ChangeCloseMode() {
    pOwner_->ChangeBehavior(std::make_unique<AudienceDisappear>(pOwner_));
}

void AudienceRoot::ChangeAppearMode() {
    pOwner_->ChangeBehavior(std::make_unique<AudienceAppear>(pOwner_));
}

void AudienceRoot::Debug() {
}
