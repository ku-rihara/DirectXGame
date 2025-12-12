#include "NotFindSprite.h"
#include "base/TextureManager.h"

void NotFindSprite::Init() {
    
    // スプライト生成
    sprite_.reset(KetaEngine::Sprite::Create("NotFind.png", false));

    // アンカーポイント、スケールセット
    sprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));
    sprite_->transform_.scale = Vector2::ZeroVector();
}

void NotFindSprite::Update() {

    // alpha,Posセット
    sprite_->SetAlpha(1.0f);
    sprite_->transform_.pos = position_;
    sprite_->SetIsDraw(isDraw_);
}


void NotFindSprite::SetScale(const Vector2& scale) {
    sprite_->transform_.scale = scale;
}