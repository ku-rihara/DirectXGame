#include"SceneFactory.h"
#include"Scene/TitleScene.h"
#include"Scene/GameScene.h"
#include"Scene/EditorScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName) {
	// 次のシーンを生成
	BaseScene* newScene = nullptr;

	if (sceneName =="TITLE") {
		newScene = new TitleScene();
	}
	else if (sceneName == "GAMEPLAY") {
		newScene = new GameScene();
	} else if (sceneName == "EDITOR") {
        newScene = new EditorScene();
    }

	return newScene;
}