#include "FindSprite.h"
#include"base/TextureManager.h"


void FindSprite::Init() {
	// テクスチャハンドル
	uint32_t findSprite =TextureManager::GetInstance()->LoadTexture("Resources/Texture/Find.png");

	// スプライト生成
	sprite_.reset(Sprite::Create(findSprite, {}, { 1,1,1,1 }));

	// アンカーポイント、スケールセット
	sprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));
	sprite_->SetScale(Vector2());
}

void FindSprite::Update() {

	//alpha,Posセット
	sprite_->SetAlpha(1.0f);
	sprite_->SetPosition(position_);
}

void FindSprite::Draw() {
	// 描画
	sprite_->Draw();
}


void FindSprite::SetScale(const Vector2& scale) {
	sprite_->SetScale(scale);
}