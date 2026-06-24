#pragma once

#include "BaseBehavior/BaseBehavior.h"
#include "Editor/Easing/Easing.h"

class GameScene;

/// <summary>
/// ゲームシーンのステート基底クラス
/// </summary>
class BaseGameSceneState : public BaseBehavior<GameScene> {
public:
    BaseGameSceneState(const std::string& name, GameScene* gameScene);
    virtual ~BaseGameSceneState() = default;

    virtual void Init()                  = 0;
    virtual void Update(float timeSpeed) = 0;
    virtual void Debug() override        = 0;

    /// ビュープロジェクション更新
    void ViewUpDate();

protected:

};
