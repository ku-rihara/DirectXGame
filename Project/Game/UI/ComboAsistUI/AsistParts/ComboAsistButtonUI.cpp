#include "ComboAsistButtonUI.h"
#include <XInput.h>

void ComboAsistButtonUI::Init(int32_t gamepadButton, bool isUnlocked, const LayoutParam& layout, const std::string& attackName) {
    gamepadButton_ = gamepadButton;
    isUnlocked_    = isUnlocked;
    layout_        = layout;
    attackName_    = attackName;

    const int32_t layerNum = 30;

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

void ComboAsistButtonUI::TryPlayPushScaling(const std::string& attackName) {
    if (attackName_ == attackName) {
        PlayPushScaling();
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