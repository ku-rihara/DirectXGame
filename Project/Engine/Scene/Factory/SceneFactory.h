#pragma once

#include"AbstractSceneFactory.h"

/// <summary>
/// シーン工場
/// </summary>
class SceneFactory :public AbstractSceneFactory {
public:

	///============================================================
	///public method
	///============================================================
	
	/// <summary>
	/// シーン作成
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns>作成したシーンのポインタ</returns>
	BaseScene* CreateScene(const std::string& sceneName)override;
};