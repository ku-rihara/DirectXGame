#include "../Audience.h"
#include "AudienceAppear.h"
#include "AudienceRoot.h"
/// frame
#include "Frame/Frame.h"
/// imgui
#include <imgui.h>

// 初期化
AudienceAppear::AudienceAppear(Audience* audience)
    : BaseAudienceBehavior("AudienceAppear", audience) {

     // 出現状態に設定
    isAppeared_ = true;
}

AudienceAppear::~AudienceAppear() {
}

// 更新
void AudienceAppear::Update() {
}

void AudienceAppear::Debug() {
   
}