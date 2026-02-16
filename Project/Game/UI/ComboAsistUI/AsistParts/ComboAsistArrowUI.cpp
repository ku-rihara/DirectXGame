#include "ComboAsistArrowUI.h"

void ComboAsistArrowUI::Init() {
    uiSprite_.reset(KetaEngine::Sprite::Create("OperateUI/Arrow.dds", false));

    const int32_t layerNum = 10; // レイヤー番号
    if (uiSprite_) {
        uiSprite_->SetLayerNum(layerNum);
        uiSprite_->SetAnchorPoint({0.5f, 0.5f});
    }
}

void ComboAsistArrowUI::Update() {
    BaseComboAsistUI::Update();
}
