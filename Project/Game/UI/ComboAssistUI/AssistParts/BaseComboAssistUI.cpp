#include "BaseComboAssistUI.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationPlayer.h"
#include <cmath>

void BaseComboAssistUI::SetRowColumn(int32_t row, int32_t column) {
    rowNum_    = row;
    columnNum_ = column;
}

void BaseComboAssistUI::Update() {
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

void BaseComboAssistUI::ApplySlideOffset(float offsetX) {
    slideOffsetX_ = offsetX;
    ApplyLayout();
}

void BaseComboAssistUI::SetLockUIScale(const Vector2& scale) {
    if (lockUI_) {
        lockUI_->transform_.scale = scale;
    }
}

void BaseComboAssistUI::SetCurrentPosition(const Vector2& pos) {
    currentDisplayPos_ = pos;
    targetPos_         = pos;
    needsLerpUpdate_   = false;
    SetPosition(pos);
}

void BaseComboAssistUI::SetPosition(const Vector2& pos) {
    if (uiSprite_) {
        uiSprite_->transform_.pos = pos;
    }
    if (lockUI_) {
        lockUI_->transform_.pos = pos;
    }
    if (activeOutLineUI_) {
        activeOutLineUI_->transform_.pos = pos;
    }
}

void BaseComboAssistUI::SetScale(const Vector2& scale) {
    baseScale_ = scale;
    Vector2 s  = baseScale_ * extraScale_;
    if (uiSprite_) {
        uiSprite_->transform_.scale = s;
    }
    if (lockUI_) {
        lockUI_->transform_.scale = s;
    }
    if (activeOutLineUI_) {
        activeOutLineUI_->transform_.scale = s;
    }
}

void BaseComboAssistUI::SetRotation(float rotZ) {
    if (uiSprite_) {
        uiSprite_->transform_.rotate.z = rotZ;
    }
    if (lockUI_) {
        lockUI_->transform_.rotate.z = rotZ;
    }
    if (activeOutLineUI_) {
        activeOutLineUI_->transform_.rotate.z = rotZ;
    }
}

void BaseComboAssistUI::SetActiveOutLine(bool visible) {
    if (activeOutLineUI_) {
        activeOutLineUI_->SetIsDraw(visible);
        if (visible && uiSprite_) {
            activeOutLineUI_->transform_.scale = uiSprite_->transform_.scale;
        }
    }
}

void BaseComboAssistUI::SnapToTarget() {
    currentDisplayPos_ = targetPos_;
    needsLerpUpdate_   = false;
    SetPosition(currentDisplayPos_);
}

void BaseComboAssistUI::SnapRangeState(bool inRange) {
    isInRange_         = inRange;
    isScaleOutPlaying_ = false;
    SetVisible(inRange);
}

void BaseComboAssistUI::PlayPushScaling() {
    if (uiSprite_) {
        uiSprite_->PlaySpriteEaseAnimation("PushScalingUI", "ComboAssistUI");
        activeOutLineUI_->PlaySpriteEaseAnimation("PushScalingUI", "ComboAssistUI");
    }
}

void BaseComboAssistUI::SetRangeVisible(bool inRange) {
    if (inRange == isInRange_) {
        return;
    }
    // ScaleOut再生中に範囲内扱いされても無視
    if (inRange && isScaleOutPlaying_) {
        return;
    }
    isInRange_ = inRange;

    if (inRange) {
        // 範囲内に入る
        SetVisible(true);
        PlayScaleIn();
    } else {
        // 範囲外に出る
        isScaleOutPlaying_ = true;
        PlayScaleOut();
    }
}

void BaseComboAssistUI::PlayScaleIn() {
    if (uiSprite_) {
        uiSprite_->transform_.scale = {0.0f, 0.0f};
        uiSprite_->PlaySpriteEaseAnimation("ScaleInUI", "ComboAssistUI");
    }
    if (lockUI_) {
        lockUI_->transform_.scale = {0.0f, 0.0f};
        lockUI_->PlaySpriteEaseAnimation("ScaleInUI", "ComboAssistUI");
    }
}

void BaseComboAssistUI::PlayScaleOut() {
    if (uiSprite_) {
        uiSprite_->PlaySpriteEaseAnimation("ScaleOutUI", "ComboAssistUI");
    }
    if (lockUI_) {
        lockUI_->PlaySpriteEaseAnimation("ScaleOutUI", "ComboAssistUI");
    }
}

void BaseComboAssistUI::SetVisible(bool visible) {
    if (!visible) {
        // 非表示にする際は範囲状態をリセットする
        // これにより再表示時に SetRangeVisible(true) が正しく機能する
        isInRange_         = false;
        isScaleOutPlaying_ = false;
    }
    if (uiSprite_) {
        uiSprite_->SetIsDraw(visible);
    }
    if (lockUI_) {
        lockUI_->SetIsDraw(visible);
    }
    if (activeOutLineUI_) {
        if (!visible) {
            activeOutLineUI_->SetIsDraw(false);
        }
    }
}

void BaseComboAssistUI::SetTargetPosY(float y) {
    targetPos_.y     = y;
    needsLerpUpdate_ = true;
}

void BaseComboAssistUI::SetExtraScale(float extraScale) {
    extraScale_ = extraScale;
}