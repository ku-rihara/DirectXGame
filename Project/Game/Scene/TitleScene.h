#pragma once

///baseScene
#include"BaseScene.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene:public BaseScene {
private:


public:
	///========================================================
	/// Constralt destract
	///========================================================
	TitleScene();
	~TitleScene()override;

	///========================================================
	/// private method
	///========================================================

	/// 初期化、更新、描画
	void Init()override;
	void Update()override;
	void ModelDraw()override;
	void SpriteDraw()override;
	void ParticleDraw()override;

	void Debug()override;/// debug
	void ViewProjectionUpdate()override;
	void ViewProssess()override;

};