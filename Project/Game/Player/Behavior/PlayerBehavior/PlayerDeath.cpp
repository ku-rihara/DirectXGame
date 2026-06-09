/// behavior
#include "PlayerDeath.h"

/// camera
#include "GameCamera/GameCamera.h"
/// player
#include "Player/Player.h"
/// engine
#include "audio/Audio.h"
#include "Frame/Frame.h"
/// imgui
#include <imgui.h>

PlayerDeath::PlayerDeath(Player* player)
    : BasePlayerBehavior("PlayerDeath", player) {

    // 当たり判定を無効化
    pOwner_->SetIsAdaptCollision(false);

    ChangeState([this](float t) { StartDeathRendition(t); });
}

PlayerDeath::~PlayerDeath() {
}

void PlayerDeath::Update([[maybe_unused]] float timeSpeed) {
    if (currentState_) {
        currentState_(KetaEngine::Frame::DeltaTime());
    }
}

void PlayerDeath::StartDeathRendition(float) {
    pOwner_->GetContext().pGameCamera->PlayAnimation("GameOverCamera", true);
    ChangeState([this](float t) { WaitCameraEnd(t); });
}

void PlayerDeath::WaitCameraEnd(float) {
    if (!pOwner_->GetContext().pGameCamera->IsRenditionFinished()) {
        return;
    }

    pOwner_->GetBaseTransform().PlayObjEaseAnimation("PlayerPreDeath", "Player");
    pOwner_->GetBaseTransform().GetObjEaseAnimationPlayer()->SetEndCallback([this]() {
        isPreDeathAnimDone_ = true;
    });

    ChangeState([this](float t) { WaitPreDeath(t); });
}

void PlayerDeath::WaitPreDeath(float) {
    if (isPreDeathAnimDone_ && !isDeathActionsTriggered_) {
        isDeathActionsTriggered_ = true;
        pOwner_->GetEffects()->Emit("PlayerDeath");
        KetaEngine::Audio::GetInstance()->Play("PlayerDeathExplotion.mp3", 1.0f);
        pOwner_->SetIsDeathRenditionFinish(true);
    }
}

void PlayerDeath::ChangeState(std::function<void(float)> newState) {
    currentState_ = newState;
}

void PlayerDeath::Debug() {
}
