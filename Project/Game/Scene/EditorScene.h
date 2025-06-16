#pragma once

///baseScene
#include"BaseScene.h"

#include"Plane/Plane.h"
#include"utility/ParticleEditor/ParticleEmitter.h"
#include<string>
#include<array>
/// <summary>
/// EditorScene
/// </summary>
class EditorScene :public BaseScene {
private:
	

public:
	///========================================================
	/// Constralt destract
	///========================================================
    EditorScene();
    ~EditorScene() override;

	///========================================================
	/// private method
	///========================================================

	/// 初期化、更新、描画
	void Init()override;
	void Update()override;
	void ModelDraw()override;
	void SpriteDraw()override;
    void SkyBoxDraw() override;

	void Debug()override;/// debug
	void ViewProjectionUpdate()override;
	void ViewProssess()override;



};