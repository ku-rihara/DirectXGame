#pragma once

#include"Scene/BaseScene.h"

/// <summary>
/// シーン管理
/// </summary>
class SceneManager {
private:

	BaseScene* scene_ = nullptr;
	BaseScene* nextScene_ = nullptr;

public:

	~SceneManager();

	void Update();

	void ModelDraw();
	void SpriteDraw();
	void ParticleDraw();

	///===========================================================
	///getter method
	///===========================================================
	BaseScene* GetScene()const { return scene_; }
	///===========================================================
	///setter method
	///===========================================================
	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }
};