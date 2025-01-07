#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

class HowToOperate {
private:
	// 座標
	Vector2 position_;
	// サイズ
	Vector2 size_;
	// スプライト
	std::unique_ptr<Sprite> sprite_;
	

public:
	// 初期化
	void Init();
	// 更新
	void Update();
	// 描画
	void Draw();
	void Debug();

};