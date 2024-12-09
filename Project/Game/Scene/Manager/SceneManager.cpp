#include "SceneManager.h"
#include <cassert>

// シングルトンインスタンスの取得
SceneManager* SceneManager::GetInstance() {
    static SceneManager instance; // 静的インスタンス
    return &instance;
}

SceneManager::~SceneManager() {
    // unique_ptrによる自動解放
}

void SceneManager::Update() {
   
    // 次のシーンが設定されている場合
    if (nextScene_) {
        scene_ = std::move(nextScene_);
        scene_->Init();
    }

    // 現在のシーンを更新
    if (scene_) {
        scene_->Update();
    }
}

void SceneManager::ModelDraw() {
    if (scene_) {
        scene_->ModelDraw();
    }
}

void SceneManager::SpriteDraw() {
    if (scene_) {
        scene_->SpriteDraw();
    }
}

void SceneManager::ParticleDraw() {
    if (scene_) {
        scene_->ParticleDraw();
    }
}

void SceneManager::ChangeScene(const std::string& scenemane) {
    assert(sceneFactory_);
    assert(!nextScene_);

    // 次のシーンを生成
    nextScene_ = std::unique_ptr<BaseScene>(sceneFactory_->CreateScene(scenemane));

  

   
}
