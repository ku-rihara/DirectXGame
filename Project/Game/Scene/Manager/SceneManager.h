#pragma once

#include "Scene/BaseScene.h"
#include "Scene/Factory/AbstractSceneFactory.h"
#include <memory> // unique_ptr
#include <type_traits>

/// <summary>
/// シーン管理
/// </summary>
class SceneManager {
private:
    std::unique_ptr<BaseScene> scene_ = nullptr;           
    std::unique_ptr<BaseScene> nextScene_ = nullptr;         
    AbstractSceneFactory* sceneFactory_ = nullptr; 

public:
    static SceneManager* GetInstance();
    ~SceneManager();

    // 更新、描画
    void Update();
    void SkyBoxDraw();
    void ModelDraw();
    void SpriteDraw();
    void DrawShadow();
 /*   void ParticleDraw();*/

    void ChangeScene(const std::string& scenemane);

    ///===========================================================
    ///getter method
    ///===========================================================
    BaseScene* GetScene() const { return scene_.get(); } // ポインタを返す

    ///===========================================================
    ///setter method
    ///===========================================================
    void SetNextScene(std::unique_ptr<BaseScene> nextScene) { nextScene_ = std::move(nextScene); }
    void SetSceneFactory(AbstractSceneFactory* abstractfactory) {
        sceneFactory_ = abstractfactory;
    }
};
