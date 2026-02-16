#include "BaseComboAsistUI.h"

void BaseComboAsistUI::Init() {
}

void BaseComboAsistUI::Update() {
}

void BaseComboAsistUI::SetPosition(const Vector2& pos) {
    if (uiSprite_) {
        uiSprite_->transform_.pos = pos;
    }
    if (outLineUI_) {
        outLineUI_->transform_.pos = pos;
    }
    if (activeOutLineUI_) {
        activeOutLineUI_->transform_.pos = pos;
    }
}

void BaseComboAsistUI::SetScale(float scale) {
    if (uiSprite_) {
        uiSprite_->transform_.scale.x = scale;
        uiSprite_->transform_.scale.y = scale;
    }
    if (outLineUI_) {
        outLineUI_->transform_.scale.x = scale;
        outLineUI_->transform_.scale.y = scale;
    }
    if (activeOutLineUI_) {
        activeOutLineUI_->transform_.scale.x = scale;
        activeOutLineUI_->transform_.scale.y = scale;
    }
}

void BaseComboAsistUI::SetRotation(float rotZ) {
    if (uiSprite_) {
        uiSprite_->transform_.rotate.z = rotZ;
    }
    if (outLineUI_) {
        outLineUI_->transform_.rotate.z = rotZ;
    }
    if (activeOutLineUI_) {
        activeOutLineUI_->transform_.rotate.z = rotZ;
    }
}

void BaseComboAsistUI::SetActiveOutLine(bool visible) {
    if (activeOutLineUI_) {
        activeOutLineUI_->SetIsDraw(visible);
        if (visible && uiSprite_) {
            activeOutLineUI_->transform_.scale = uiSprite_->transform_.scale;
        }
    }
}

void BaseComboAsistUI::PlayPushScaling() {
    if (uiSprite_) {
        uiSprite_->PlaySpriteEaseAnimation("PushScalingUI", "ComboAsistUI");
    }
}
