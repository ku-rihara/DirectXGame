#pragma once

#include "BaseBehavior/BaseBehavior.h"
#include "Easing/Easing.h"
#include <memory>

class GameScene;

/// <summary>
/// ゲームシーンのステート基底クラス
/// </summary>
class BaseGameSceneState : public BaseBehavior<GameScene> {
public:
    BaseGameSceneState(const std::string& name, GameScene* player);
    virtual ~BaseGameSceneState() = default;

    virtual void Init()                 = 0;
    virtual void Update(float atkSpeed) = 0;
    virtual void Debug() override       = 0;
   
protected:
  
};