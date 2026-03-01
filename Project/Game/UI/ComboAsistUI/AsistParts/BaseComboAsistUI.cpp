#include "BaseComboAsistUI.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationPlayer.h"
#include <cmath>

void BaseComboAsistUI::SetRowColumn(int32_t row, int32_t column) {
    rowNum_    = row;
    columnNum_ = column;
}

void BaseComboAsistUI::Update() {
    if (needsLerpUpdate_) {
        float dt           = KetaEngine::Frame::DeltaTime();
        float t            = 1.0f - std::exp(-lerpSpeed_ * dt);
        currentDisplayPos_ = Lerp(currentDisplayPos_, targetPos_, t);

        // 十分近づいたらスナップ
        Vector2 diff = {targetPos_.x - currentDisplayPos_.x, targetPos_.y - currentDisplayPos_.y};
        if (std::abs(diff.x) < 0.5f && std::abs(diff.y) < 0.5f) {
            currentDisplayPos_ = targetPos_;
            needsLerpUpdate_   = false;
        }
        SetPosition(currentDisplayPos_);
    }

    // ScaleOutアニメーション完了後に非表示化
    if (isScaleOutPlaying_ && uiSprite_) {
        auto* player = uiSprite_->GetSpriteEaseAnimationPlayer();
        if (player && player->IsFinished()) {
            SetVisible(false);
            isScaleOutPlaying_ = false;
        }
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

void BaseComboAsistUI::SetScale(const Vector2& scale) {
    baseScale_ = scale;
    Vector2 s  = baseScale_ * extraScale_;
    if (uiSprite_) {
        uiSprite_->transform_.scale = s;
    }
    if (outLineUI_) {
        outLineUI_->transform_.scale = s;
    }
    if (activeOutLineUI_) {
        activeOutLineUI_->transform_.scale = s;
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
    needsLerpUpdate_   = false;
    SetPosition(currentDisplayPos_);
}

void BaseComboAsistUI::PlayPushScaling() {
    if (uiSprite_) {
        uiSprite_->PlaySpriteEaseAnimation("PushScalingUI", "ComboAsistUI");
        activeOutLineUI_->PlaySpriteEaseAnimation("PushScalingUI", "ComboAsistUI");
    }
}

void BaseComboAsistUI::SetRangeVisible(bool inRange) {
    if (inRange == isInRange_) {
        return;
    }
    // ScaleOut再生中に範囲内扱いされても無視（シフト後に自動解決）
    if (inRange && isScaleOutPlaying_) {
        return;
    }
    isInRange_ = inRange;

    if (inRange) {
        // 範囲内に入る: 表示してからScaleIn再生
        SetVisible(true);
        PlayScaleIn();
    } else {
        // 範囲外に出る: ScaleOut再生、完了後にUpdate()で非表示化
        isScaleOutPlaying_ = true;
        PlayScaleOut();
    }
}

void BaseComboAsistUI::PlayScaleIn() {
    if (uiSprite_) {
        uiSprite_->transform_.scale = {0.0f, 0.0f};
        uiSprite_->PlaySpriteEaseAnimation("ScaleInUI", "ComboAsistUI");
    }
    if (outLineUI_) {
        outLineUI_->transform_.scale = {0.0f, 0.0f};
        outLineUI_->PlaySpriteEaseAnimation("ScaleInUI", "ComboAsistUI");
    }
}

void BaseComboAsistUI::PlayScaleOut() {
    if (uiSprite_) {
        uiSprite_->PlaySpriteEaseAnimation("ScaleOutUI", "ComboAsistUI");
    }
    if (outLineUI_) {
        outLineUI_->PlaySpriteEaseAnimation("ScaleOutUI", "ComboAsistUI");
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

void BaseComboAsistUI::SetTargetPosY(float y) {
    targetPos_.y     = y;
    needsLerpUpdate_ = true;
}

void BaseComboAsistUI::SetExtraScale(float extraScale) {
    extraScale_ = extraScale;
}