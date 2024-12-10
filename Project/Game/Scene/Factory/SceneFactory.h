#pragma once

#include"AbstractSceneFactory.h"

//このゲーム用のシーン工場

class SceneFactory :public AbstractSceneFactory {
public:

	///============================================================
	///public method
	///============================================================
	
	BaseScene* CreateScene(const std::string& sceneName)override;
};