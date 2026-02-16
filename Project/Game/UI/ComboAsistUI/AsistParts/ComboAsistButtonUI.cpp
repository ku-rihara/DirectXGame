#include "ComboAsistButtonUI.h"
#include <XInput.h>

void ComboAsistButtonUI::Init(int32_t gamepadButton, bool isUnlocked) {
    gamepadButton_ = gamepadButton;
    isUnlocked_    = isUnlocked;

    const int32_t layerNum = 10; // レイヤー番号

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

void ComboAsistButtonUI::SetUnlocked(bool isUnlocked) {
    isUnlocked_ = isUnlocked;
    if (outLineUI_) {
        outLineUI_->SetIsDraw(!isUnlocked_);
    }
}
