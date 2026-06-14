#include "SceneFactory.h"
#include "Scene/BattleTestScene.h"
#include "Scene/GameResultScene.h"
#include "Scene/GameScene.h"
#include "Scene/TitleScene.h"

SceneFactory::SceneFactory() {
    // シーン名と生成処理をテーブルに登録する
    // タイトルシーン
    sceneCreators_["TITLE"] = []() {
        return std::make_unique<TitleScene>();
    };
    // ゲームシーン
    sceneCreators_["GAMEPLAY"] = []() {
        return std::make_unique<GameScene>();
    };
    // バトルテストシーン
    sceneCreators_["BATTLE_TEST"] = []() {
        return std::make_unique<BattleTestScene>();
    };
    // リザルトシーン
    sceneCreators_["RESULT"] = []() {
        return std::make_unique<GameResultScene>();
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
