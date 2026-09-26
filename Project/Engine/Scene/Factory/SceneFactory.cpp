#include "SceneFactory.h"
#include "Scene/EngineScene.h"

SceneFactory::SceneFactory() {
    // シーン名と生成処理をテーブルに登録する
    // エンジン機能確認用シーン
    sceneCreators_["ENGINE"] = []() {
        return std::make_unique<EngineScene>();
    };
}

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {
    // テーブルから生成関数を引いて実行する
    auto it = sceneCreators_.find(sceneName);
    if (it == sceneCreators_.end()) {
        return nullptr; // 未登録のシーン名
    }
    return it->second();
}
