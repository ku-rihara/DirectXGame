#include "TitlePlayer.h"
#include "Player/Behavior/TitleBehavior/TitlePlayerBehavior.h"

void TitlePlayer::SetTitleBehavior() {
    titleBehavior_ = std::make_unique<TitlePlayerBehavior>(this);
}

void TitlePlayer::TitleUpdate() {
    effects_.Update(GetWorldPosition(), baseTransform_.rotation_);
    titleBehavior_->Update();
    UpdateMatrix();
}
