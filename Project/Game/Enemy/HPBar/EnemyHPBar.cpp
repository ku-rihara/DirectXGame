#include "EnemyHPBar.h"
#include"base/TextureManager.h"


void EnemyHPBar::Init(const Vector2& size) {
	uint32_t barhandle=TextureManager::GetInstance()->LoadTexture("boss_Hpbar01.png");
	uint32_t Hphandle = TextureManager::GetInstance()->LoadTexture("boss_Hpbar02.png");
	size_ = size;
	// HPBarテクスチャハンドル
	sprite_ = std::make_unique<Sprite>();
	hpSprite_ = std::make_unique<Sprite>();
	
	sprite_.reset(Sprite::Create(barhandle, {}, { 1,1,1,1 }));
	hpSprite_.reset(Sprite::Create(Hphandle, {}, { 1,1,1,1 }));

	// スプライト
	sprite_->SetTextureSize(size_);
	hpSprite_->SetTextureSize(size_);

}

void EnemyHPBar::Update(int hp) {

	/*if (!isSpowned_) {
		hpSprite_->SetPosition(position_);
		hpSprite_->SetSize(size_);
	}*/

	sprite_->SetAlpha(alpha_);
	hpSprite_->SetAlpha(alpha_);

	hpSprite_->SetPosition(Vector2(position_.x, position_.y));
	hpSprite_->SetTextureSize(Vector2(float(hp), size_.y));

	sprite_->SetPosition(position_);
	sprite_->SetTextureSize(size_);
}

void EnemyHPBar::Draw() {
	// スプライトを描画
	
	hpSprite_->Draw();
	sprite_->Draw();
}
