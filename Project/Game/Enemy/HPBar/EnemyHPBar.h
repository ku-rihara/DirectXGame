#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

class EnemyHPBar {
private:
	// 座標
	Vector2 position_;
	// サイズ
	Vector2 size_;
	// スプライト
	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Sprite> hpSprite_;

	bool isSpowned_ = false;

	float alpha_;

public:
	// 初期化
	void Init(const Vector2& size);
	// 更新
	void Update(int hp);
	// 描画
	void Draw();
	//getter
	bool GetIsSpowned() const { return isSpowned_; }
	// setter
	void SetPosition(Vector2 pos) { position_ = pos; }
	void SetSize(Vector2 size) { size_ = size; }
	void SetIsSpowned(bool is) { isSpowned_ = is; }
	void SetAlpha(float a) { alpha_ = a; }
};