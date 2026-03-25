#include "BaseGameSceneState.h"
#include "Scene/GameScene.h"

BaseGameSceneState::BaseGameSceneState(const std::string& name, GameScene* gameScene)
    : BaseBehavior<GameScene>(name, gameScene) {
}

void BaseGameSceneState::ViewUpDate() {
    pOwner_->ViewProjectionUpdate();
}

