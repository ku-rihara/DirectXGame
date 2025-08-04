#include "EnemyHPBar.h"
#include"base/TextureManager.h"


void EnemyHPBar::Init(const Vector2& size) {

	// textureHandle取得
	uint32_t barHandle=TextureManager::GetInstance()->LoadTexture("Resources/Texture/boss_Hpbar01.png");
	uint32_t HpHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/boss_Hpbar02.png");
	
	// スプライト生成
    sprite_.reset(Sprite::Create(barHandle, {}, {1, 1, 1, 1}));
    hpSprite_.reset(Sprite::Create(HpHandle, {}, {1, 1, 1, 1}));

	// サイズセット
    size_ = size;
	sprite_->SetTextureSize(size_);
	hpSprite_->SetTextureSize(size_);

}

void EnemyHPBar::Update(int hp) {

	// alphaセット
	sprite_->SetAlpha(1.0f);
	hpSprite_->SetAlpha(1.0f);

	// positionセット
	hpSprite_->SetPosition(Vector2(position_.x, position_.y));
	hpSprite_->SetTextureSize(Vector2(float(max(hp,0.0f)), size_.y));

	sprite_->SetPosition(position_);
	sprite_->SetTextureSize(size_);
}

void EnemyHPBar::Draw() {
	
	// 描画
	hpSprite_->Draw();
	sprite_->Draw();
}
