#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

#include"Easing.h"

class TitleRogo {
private:
	enum class Step{
		PUNCH,
		RUSH,
		BOTTON,
		END,
	};

	Step step_;
// 座標
	Vector2 punchPosition_;
	Vector2 rushPosition_;
	Vector2 bottonPosition_;
	// サイズ
	Vector2 size_;
	// スプライト
	std::unique_ptr<Sprite> punchSprite_;
	std::unique_ptr<Sprite> rushSprite_;
	std::unique_ptr<Sprite> bottomSprite_;

	Easingw<Vector2> punchEase_;
	Easingw<Vector2> rushEase_;
	Easingw<Vector2> bottomEase_;
	
	Vector2 punchSpriteScale_;
    Vector2 rushSpriteScale_;
    Vector2 bottomSpriteScale_;

public:
	// 初期化
	void Init();
	// 更新
	void Update();
	// 描画
	void Draw();
	void Debug();

	void EasingSet();

};