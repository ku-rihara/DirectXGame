#include "EnemyHPBar.h"
#include"base/TextureManager.h"


void EnemyHPBar::Init(const Vector2& size) {
	uint32_t barhandle=TextureManager::GetInstance()->LoadTexture("Resources/Texture/boss_Hpbar01.png");
	uint32_t Hphandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/boss_Hpbar02.png");
	size_ = size;
	
	sprite_.reset(Sprite::Create(barhandle, {}, { 1,1,1,1 }));
	hpSprite_.reset(Sprite::Create(Hphandle, {}, { 1,1,1,1 }));

	/*sprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));
	hpSprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));*/
	// スプライト
	sprite_->SetTextureSize(size_);
	hpSprite_->SetTextureSize(size_);

}

void EnemyHPBar::Update(int hp) {

	/*if (!isSpowned_) {
		hpSprite_->SetPosition(position_);
		hpSprite_->SetSize(size_);
	}*/

	sprite_->SetAlpha(1.0f);
	hpSprite_->SetAlpha(1.0f);

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
