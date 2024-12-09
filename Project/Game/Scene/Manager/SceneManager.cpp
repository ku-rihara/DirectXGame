#include"SceneManager.h"

SceneManager::~SceneManager() {
    delete scene_;
}


void SceneManager::Update() {
    // 次のシーンが設定されている場合
    if (nextScene_) {
      
        if (scene_) {
            delete scene_;
        }
        scene_ = nextScene_;
        nextScene_ = nullptr;
        // 初期化処理を呼び出す
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