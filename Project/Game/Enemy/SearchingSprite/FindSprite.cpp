#include "FindSprite.h"
#include "base/TextureManager.h"

void FindSprite::Init() {

    // スプライト生成
    sprite_.reset(Sprite::Create("Find.png"));

    // アンカーポイント、スケールセット
    sprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));
    sprite_->transform_.scale = Vector2::ZeroVector();
}

void FindSprite::Update() {

    // Posセット
    sprite_->transform_.pos = position_;
}

void FindSprite::Draw() {
    // 描画
    sprite_->Draw();
}

void FindSprite::SetScale(const Vector2& scale) {
    sprite_->transform_.scale = scale;
}