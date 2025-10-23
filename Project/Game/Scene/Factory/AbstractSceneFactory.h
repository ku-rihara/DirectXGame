#pragma once

#include"Scene/BaseScene.h"
#include<string>

/// <summary>
/// シーン工場
/// </summary>
class AbstractSceneFactory {

public:

	virtual ~AbstractSceneFactory() = default;

	/// <summary>
	/// シーン作成
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns>作成したシーンのポインタ</returns>
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;


};