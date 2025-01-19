#include "NotFindSprite.h"
#include"base/TextureManager.h"


void NotFindSprite::Init() {

	uint32_t serchingSprite = TextureManager::GetInstance()->LoadTexture("Resources/Texture/NotFind.png");
	sprite_.reset(Sprite::Create(serchingSprite, {}, { 1,1,1,1 }));
	sprite_->SetAnchorPoint(Vector2(0.5f,0.5f));
	sprite_->SetScale(Vector2());
}

void NotFindSprite::Update() {

	sprite_->SetAlpha(1.0f);
	sprite_->SetPosition(Vector2(position_.x, position_.y));
}

void NotFindSprite::Draw() {
	// スプライトを描画
	sprite_->Draw();
}


void NotFindSprite::SetScale(const Vector2& scale) {
	sprite_->SetScale(scale);
}