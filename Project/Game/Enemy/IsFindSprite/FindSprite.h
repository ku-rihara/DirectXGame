#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

class FindSprite {
private:
	// 座標
	Vector2 position_;

	// スプライト
	std::unique_ptr<Sprite> sprite_;
	
	bool isSpowned_ = false;
	float alpha_;

public:
	// 初期化
	void Init();
	// 更新
	void Update();
	// 描画
	void Draw();
	//getter
	bool GetIsSpowned() const { return isSpowned_; }
	// setter
	void SetPosition(Vector2 pos) { position_ = pos; }
	void SetIsSpowned(bool is) { isSpowned_ = is; }
	void SetAlpha(float a) { alpha_ = a; }
	void SetScale(const Vector2& scale);
};