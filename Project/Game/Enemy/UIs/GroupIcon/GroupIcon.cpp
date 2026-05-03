#include "GroupIcon.h"

void GroupIcon::Init() {
    sprite_     = nullptr;
    groupIndex_ = -1;
}

void GroupIcon::SetGroupIndex(int32_t index) {
    groupIndex_ = index;
    if (index < 0 || index >= kGroupCount) {
        sprite_ = nullptr;
        return;
    }
    sprite_.reset(KetaEngine::Sprite::Create(kTexturePaths[index], true));
}

void GroupIcon::Update(const Vector2& screenPos, bool isDraw) {
    if (!sprite_) {
        return;
    }
    sprite_->transform_.pos = screenPos;
    sprite_->SetIsDraw(isDraw);
}