#pragma once

#include "Scene/BaseScene.h"
#include "Scene/Factory/AbstractSceneFactory.h"
#include <Collider/CollisionManager.h>
#include <memory>
#include <type_traits>

namespace KetaEngine {
/// <summary>
/// シーン管理
/// </summary>
class SceneManager {
public:
    static SceneManager* GetInstance();
    ~SceneManager();

    // 更新、描画
    void Update();
    void SkyBoxDraw();
    void Debug();

    /// <summary>
    /// シーン切り替え
    /// </summary>
    /// <param name="sceneName"></param>
    void ChangeScene(const std::string& scenemane);

private:
    std::unique_ptr<BaseScene> scene_                   = nullptr;
    std::unique_ptr<BaseScene> nextScene_               = nullptr;
    AbstractSceneFactory* sceneFactory_                 = nullptr;
    std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

public:
    ///===========================================================
    /// getter method
    ///===========================================================
    BaseScene* GetScene() const { return scene_.get(); }

    ///===========================================================
    /// setter method
    ///===========================================================
    void SetNextScene(std::unique_ptr<BaseScene> nextScene) { nextScene_ = std::move(nextScene); }
    void SetSceneFactory(AbstractSceneFactory* AbstractFactory) {
        sceneFactory_ = AbstractFactory;
    }
};

}