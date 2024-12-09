#pragma once

#include"AbstractSceneFactory.h"

//このゲーム用のシーン工場

class SceneFactory :
	public AbstractSceneFactory {
public:

	/// シーン生成
	BaseScene* CreateScene(const std::string& sceneName)override;
};