#pragma once

#include"2d/Sprite.h"


///std
#include<memory>

/// <summary>
/// ゲームシーン
/// </summary>
class TitleScene {
private:
	
	int handle_;
	std::unique_ptr<Sprite>titleSprite_;
	bool isFinished_;
public: 
	/// <summary>
	/// コンストクラタ
	/// </summary>
	TitleScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void ModelDraw();

	void SpriteDraw();

	void ParticleDraw();

	bool GetIsFinished()const { return isFinished_; }

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// ビュープロジェクション更新
	/// </summary>
	void ViewProjectionUpdate();
};