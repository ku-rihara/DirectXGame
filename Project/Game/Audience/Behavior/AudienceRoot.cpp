#include "AudienceRoot.h"
#include "../Audience.h"
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

void AudienceRoot::Debug() {

}