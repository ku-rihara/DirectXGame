#pragma once

///baseScene
#include"BaseScene.h"

#include"TestObj/CollisionTest1.h"
#include"TestObj/CollisionTest2.h"
#include"TestObj/CollisionTest3.h"
/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene:public BaseScene {
private:

	std::unique_ptr<CollisionTest1>ctest1_;
	std::unique_ptr<CollisionTest2>ctest2_;
	std::unique_ptr<CollisionTest3>ctest3_;
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