#include"SceneFactory.h"
#include"Scene/TitleScene.h"
#include"Scene/GameScene.h"
#include"Scene/EditorScene.h"
#include"Scene/GameResultScene.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {
	// 次のシーンを生成
	std::unique_ptr<BaseScene> newScene = nullptr;

	if (sceneName =="TITLE") {
		newScene = std::make_unique<TitleScene>();
	}
	else if (sceneName == "GAMEPLAY") {
		newScene = std::make_unique<GameScene>();
	} else if (sceneName == "EDITOR") {
        newScene = std::make_unique<EditorScene>();
    } else if (sceneName == "RESULT") {
        newScene = std::make_unique<GameResultScene>();
    }

	return newScene;
}