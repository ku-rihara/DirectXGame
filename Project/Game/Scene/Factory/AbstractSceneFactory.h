#pragma once

#include"Scene/BaseScene.h"
#include<string>

/// <summary>
/// シーン工場
/// </summary>
class AbstractSceneFactory {

public:

	virtual ~AbstractSceneFactory() = default;

	// シーン生成
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;


};