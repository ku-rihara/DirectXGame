#include "OperateUI.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include <imgui.h>

void OperateUI::Init() {

    for (int32_t i = 0; i < static_cast<int32_t>(OperateButtonType::COUNT); ++i) {
        // スプライト生成
        uiParam_[i].sprite_.reset(KetaEngine::Sprite::Create(kButtonTextures[i], true));
        uiParam_[i].sprite_->transform_.scale = Vector2::ZeroVector();
       /* uiParam_[i].outLineSprite_.reset(KetaEngine::Sprite::Create(kOutLineTexture));
        uiParam_[i].uiFrame.reset(KetaEngine::Sprite::Create(frameTexture));*/
    }
}

void OperateUI::Update() {
}

void OperateUI::SetScale(const Vector2& scale) {
    for (auto& param : uiParam_) {
        param.sprite_->transform_.scale        = scale;
     /*   param.outLineSprite_->transform_.scale = scale;*/
    }
}


bool OperateUI::IsPushCondition(OperateButtonType type) {
    type;
    return true;
 }

void OperateUI::Debug() {
    ImGui::SeparatorText("OperateUI");
}
