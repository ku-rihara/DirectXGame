#include "AudienceDisappear.h"
#include "../Audience.h"
/// frame
#include "Frame/Frame.h"
/// imgui
#include <imgui.h>

// 初期化
AudienceDisappear::AudienceDisappear(Audience* audience)
    : BaseAudienceBehavior("AudienceDisappear", audience) {
}

AudienceDisappear::~AudienceDisappear() {
}

// 更新
void AudienceDisappear::Update() {
}

void AudienceDisappear::Debug() {
   
}