#pragma once

#include"AbstractSceneFactory.h"
#include<memory>

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
	std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName)override;
};