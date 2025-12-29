#include "BaseGameSceneState.h"


BaseGameSceneState::BaseGameSceneState(const std::string& name, GameScene* gameScene)
    : BaseBehavior<GameScene>(name, gameScene) {
  
}

