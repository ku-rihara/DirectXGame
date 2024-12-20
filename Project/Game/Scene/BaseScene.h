#pragma once

//DirectX
#include "base/DirectXCommon.h"
#include"base/TextureManager.h"

//Transform
#include"3d/ViewProjection.h"
#include"3d/WorldTransform.h"
#include"3d/Object3d.h"

#include "2d/Sprite.h"
#include "audio/Audio.h"
#include"input/Input.h"

//etc
#include"utility/Debug/DebugCamera.h"
#include<memory>

class BaseScene {


protected:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	TextureManager* textureManager_ = nullptr;

	ViewProjection viewProjection_;

	std::unique_ptr<DebugCamera>debugCamera_;
	bool isDebugCameraActive_;

public:

	///========================================================
	/// Constralt destract
	///========================================================
	BaseScene() = default;
	virtual ~BaseScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual	void Init();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual	void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual	void ModelDraw() = 0;
	virtual	void SpriteDraw() = 0;
	virtual	void ParticleDraw() = 0;

	/// <summary>
	/// デバッグ表示
	/// </summary>
	virtual	void Debug();
	virtual	void ViewProjectionUpdate();
	virtual	void ViewProssess() = 0;

	///========================================================
	/// getter method
	///========================================================
	const ViewProjection& GetViewProjection()const { return viewProjection_; }

	///========================================================
	/// setter method
	///========================================================


};