#pragma once
#include "Scene/Manager/SceneManager.h"
#include "Scene/Factory/AbstractSceneFactory.h"
//utility
#include "Collider/CollisionManager.h"
#include "EngineCore.h"
/// std
#include <memory>


// ゲーム全体
class KTFramework {
protected:

    /// other class
    SceneManager* pSceneManager_ = nullptr;

    ///=======================================================
    /// private variants
   ///=======================================================
    std::unique_ptr<AbstractSceneFactory> sceneFactory_;
    std::unique_ptr<CollisionManager> collisionManager_;
    std::unique_ptr<EngineCore> engineCore_;

public:

    virtual ~KTFramework() = default;

    ///=======================================================
    ///public method
    ///=======================================================
    virtual void Init();
    virtual void Update();
    virtual void Draw() = 0;
	virtual void DrawPostEffect() = 0;  
    virtual void Finalize();

    void DisplayFPS(); /// FPS表示

    void Run(); /// 実行
};
