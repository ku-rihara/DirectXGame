#include "EnemyHPBar.h"
#include"base/TextureManager.h"


void EnemyHPBar::Init(const Vector2& size) {
	TextureManager::GetInstance()->LoadTexture("boss_Hpbar01.png");
	TextureManager::GetInstance()->LoadTexture("boss_Hpbar02.png");
	size_ = size;
	// HPBarテクスチャハンドル
	sprite_ = std::make_unique<Sprite>();
	hpSprite_ = std::make_unique<Sprite>();

	sprite_.reset("boss_Hpbar01.png", {}, { 1,1,1,1 }, {});
	hpSprite_->Initialize("boss_Hpbar02.png", {}, { 1,1,1,1 }, {});

	// スプライト
	sprite_->SetSize(size_);
	hpSprite_->SetSize(size_);

}

void EnemyHPBar::Update(int hp) {

	/*if (!isSpowned_) {
		hpSprite_->SetPosition(position_);
		hpSprite_->SetSize(size_);
	}*/

	sprite_->SetAlpha(alpha_);
	hpSprite_->SetAlpha(alpha_);

	hpSprite_->SetPosition(Vector2(position_.x, position_.y));
	hpSprite_->SetSize(Vector2(float(hp), size_.y));

	sprite_->SetPosition(position_);
	sprite_->SetSize(size_);
}

void EnemyHPBar::Draw() {
	// スプライトを描画
	
	hpSprite_->Draw();
	sprite_->Draw();
}
