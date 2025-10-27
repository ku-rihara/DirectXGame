#include "FindSprite.h"
#include "base/TextureManager.h"

void FindSprite::Init() {

    // スプライト生成
    sprite_.reset(Sprite::Create("Find.png",false));

    // アンカーポイント、スケールセット
    sprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));
    sprite_->transform_.scale = Vector2::ZeroVector();
}

void FindSprite::Update() {
    // Posセット
    sprite_->transform_.pos = position_;
    sprite_->SetIsDraw(isDraw_);
}


void FindSprite::SetScale(const Vector2& scale) {
    sprite_->transform_.scale = scale;
}