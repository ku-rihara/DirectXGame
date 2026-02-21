#include "ComboAsistButtonUI.h"
#include <XInput.h>

void ComboAsistButtonUI::Init(int32_t gamepadButton, bool isUnlocked, int32_t row, int32_t col, const LayoutParam& layout) {
    gamepadButton_ = gamepadButton;
    isUnlocked_    = isUnlocked;
    rowNum_        = row;
    columnNum_     = col;
    layout_        = layout;

    const int32_t layerNum = 20;

    // ボタンに応じたテクスチャを選択
    if (gamepadButton == XINPUT_GAMEPAD_Y) {
        uiSprite_.reset(KetaEngine::Sprite::Create("OperateUI/OperateY.dds", false));
    } else {
        uiSprite_.reset(KetaEngine::Sprite::Create("OperateUI/OperateX.dds", false));
    }

    if (uiSprite_) {
        uiSprite_->SetLayerNum(layerNum);
        uiSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    // ロックオーバーレイ
    outLineUI_.reset(KetaEngine::Sprite::Create("OperateUI/Locked.dds", false));
    if (outLineUI_) {
        outLineUI_->SetLayerNum(layerNum + 1);
        outLineUI_->SetIsDraw(!isUnlocked_);
        outLineUI_->SetAnchorPoint({0.5f, 0.5f});
    }

    // 攻撃発動アウトライン
    activeOutLineUI_.reset(KetaEngine::Sprite::Create("OperateUI/OutLine.dds", false));
    if (activeOutLineUI_) {
        activeOutLineUI_->SetLayerNum(layerNum + 2);
        activeOutLineUI_->SetIsDraw(false);
        activeOutLineUI_->SetAnchorPoint({0.5f, 0.5f});
    }

    ApplyLayout();
    currentDisplayPos_ = targetPos_;
    SetPosition(currentDisplayPos_);
    needsLerpUpdate_ = false;
}

void ComboAsistButtonUI::Update() {
    BaseComboAsistUI::Update();
}

void ComboAsistButtonUI::ApplyLayout() {
    Vector2 pos(
        layout_.basePosition.x + columnNum_ * layout_.columnSpacing + slideOffsetX_,
        layout_.basePosition.y + rowNum_ * (layout_.rowSpacing + layout_.branchYOffset));

    targetPos_       = pos;
    needsLerpUpdate_ = true;
    SetScale(layout_.buttonScale);
}

void ComboAsistButtonUI::SetUnlocked(bool isUnlocked) {
    isUnlocked_ = isUnlocked;
    if (outLineUI_) {
        outLineUI_->SetIsDraw(!isUnlocked_);
    }
}

void ComboAsistButtonUI::SetVisible(bool visible) {
    if (uiSprite_) {
        uiSprite_->SetIsDraw(visible);
    }
    if (outLineUI_) {
        outLineUI_->SetIsDraw(visible && !isUnlocked_);
    }
    if (activeOutLineUI_) {
        if (!visible) {
            activeOutLineUI_->SetIsDraw(false);
        }
    }
}