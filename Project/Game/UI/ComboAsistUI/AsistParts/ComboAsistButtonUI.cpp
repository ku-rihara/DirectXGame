#include "ComboAsistButtonUI.h"
#include <XInput.h>
#include"Audio/Audio.h"

void ComboAsistButtonUI::Init(int32_t gamepadButton, bool isUnlocked, const LayoutParam& layout, const std::string& attackName) {
    gamepadButton_ = gamepadButton;
    isUnlocked_    = isUnlocked;
    layout_        = layout;
    attackName_    = attackName;
    shakePlayer_.Init();
    unlockParticlePlayer_.Init();

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

    // ロックUI
    // 最初から開放済みの場合は生成しない
    if (!isUnlocked_) {
        lockUI_.reset(KetaEngine::Sprite::Create("ComboSupportUI/AttackLock.dds", false));
        if (lockUI_) {
            lockUI_->SetLayerNum(layerNum + 1);
            lockUI_->SetUVScale({0.5f, 1.0f});
            lockUI_->SetUVPosition({0.0f, 0.0f});
            lockUI_->SetAnchorPoint({0.5f, 0.5f});
            lockUI_->SetAlpha(0.5f);
        }
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

    unlockParticlePlayer_.Update();
    unlockParticlePlayer_.SetTargetPosition({currentDisplayPos_.x, currentDisplayPos_.y, 0.0f});
    shakePlayer_.Update();

    if (lockUI_) {
        
        const Vector3& shakeOffset = shakePlayer_.GetTotalShakeOffset();
        lockUI_->transform_.pos = {
            currentDisplayPos_.x + shakeOffset.x,
            currentDisplayPos_.y + shakeOffset.y
        };

        // 開放シェイク終了タイミングで非表示 + パーティクル発生
        if (isUnlockShakePlaying_ && shakePlayer_.IsFinished()) {
            isUnlockShakePlaying_ = false;
            lockUI_->SetIsDraw(false);
            KetaEngine::Audio::GetInstance()->Play("AttackUnlockSE.mp3", 1.0f);
            unlockParticlePlayer_.Play("AttackUnlockEffect", "UI");
        }
    }
}

void ComboAsistButtonUI::ApplyLayout() {
    // 基準位置 + 列・行番号に応じたオフセット + スライドオフセット
    Vector2 pos(
        layout_.basePosition.x + columnNum_ * layout_.columnSpacing + slideOffsetX_,
        layout_.basePosition.y + rowNum_ * layout_.rowSpacing);

    targetPos_       = pos;
    needsLerpUpdate_ = true;
    SetScale({layout_.buttonScale, layout_.buttonScale});
}

void ComboAsistButtonUI::SetUnlocked(bool isUnlocked) {
    const bool justUnlocked = !isUnlocked_ && isUnlocked;
    isUnlocked_ = isUnlocked;

    if (lockUI_ && justUnlocked) {
        KetaEngine::Audio::GetInstance()->Play("PreUnlockES.mp3",1.0f);
        lockUI_->SetUVPosition({0.5f, 0.0f});
        isUnlockShakePlaying_ = true;
        shakePlayer_.Play("UnlockShake", "ComboAsistUI");
    }
}

void ComboAsistButtonUI::TryPlayPushScaling(const std::string& attackName) {
    if (attackName_ == attackName) {
        PlayPushScaling();
    }
}

void ComboAsistButtonUI::PlayScaleIn() {
    // uiSpriteのみアニメ再生
    if (uiSprite_) {
        uiSprite_->transform_.scale = {0.0f, 0.0f};
        uiSprite_->PlaySpriteEaseAnimation("ScaleInUI", "ComboAsistUI");
    }
}

void ComboAsistButtonUI::PlayScaleOut() {
    // uiSpriteのみアニメ再生
    if (uiSprite_) {
        uiSprite_->PlaySpriteEaseAnimation("ScaleOutUI", "ComboAsistUI");
    }
}

void ComboAsistButtonUI::SetVisible(bool visible) {
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
