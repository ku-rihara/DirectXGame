#include "BaseComboAsistUI.h"
#include <cmath>

void BaseComboAsistUI::SetRowColumn(int32_t row, int32_t column) {
    rowNum_ = row;
    columnNum_ = column;
}

void BaseComboAsistUI::Update() {
    if (needsLerpUpdate_) {
        float dt = KetaEngine::Frame::DeltaTime();
        float t = 1.0f - std::exp(-lerpSpeed_ * dt);
        currentDisplayPos_ = Lerp(currentDisplayPos_, targetPos_, t);

        // 十分近づいたらスナップ
        Vector2 diff = { targetPos_.x - currentDisplayPos_.x, targetPos_.y - currentDisplayPos_.y };
        if (std::abs(diff.x) < 0.5f && std::abs(diff.y) < 0.5f) {
            currentDisplayPos_ = targetPos_;
            needsLerpUpdate_ = false;
        }
        SetPosition(currentDisplayPos_);
    }
}

void BaseComboAsistUI::ApplySlideOffset(float offsetX) {
    slideOffsetX_ = offsetX;
    ApplyLayout();
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

void BaseComboAsistUI::SnapToTarget() {
    currentDisplayPos_ = targetPos_;
    needsLerpUpdate_ = false;
    SetPosition(currentDisplayPos_);
}

void BaseComboAsistUI::PlayPushScaling() {
    if (uiSprite_) {
        uiSprite_->PlaySpriteEaseAnimation("PushScalingUI", "ComboAsistUI");
    }
}

void BaseComboAsistUI::SetVisible(bool visible) {
    if (uiSprite_) {
        uiSprite_->SetIsDraw(visible);
    }
    if (outLineUI_) {
        outLineUI_->SetIsDraw(visible);
    }
    if (activeOutLineUI_) {
    
        if (!visible) {
            activeOutLineUI_->SetIsDraw(false);
        }
    }
}
