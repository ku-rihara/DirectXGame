#include "NotFindSprite.h"
#include"base/TextureManager.h"


void NotFindSprite::Init() {
    // テクスチャハンドル
	uint32_t searchingSprite = TextureManager::GetInstance()->LoadTexture("Resources/Texture/NotFind.png");

	// スプライト生成
	sprite_.reset(Sprite::Create(searchingSprite, {}, { 1,1,1,1 }));

	// アンカーポイント、スケールセット
	sprite_->SetAnchorPoint(Vector2(0.5f,0.5f));
	sprite_->SetScale(Vector2());
}

void NotFindSprite::Update() {

	// alpha,Posセット
	sprite_->SetAlpha(1.0f);
	sprite_->SetPosition(Vector2(position_.x, position_.y));
}

void NotFindSprite::Draw() {
	// 描画
	sprite_->Draw();
}


void NotFindSprite::SetScale(const Vector2& scale) {
	sprite_->SetScale(scale);
}