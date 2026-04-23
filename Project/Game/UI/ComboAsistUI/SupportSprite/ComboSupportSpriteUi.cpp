#include "ComboSupportSpriteUi.h"
#include "UI/ComboAsistUI/ComboAsistController.h"
#include "base/TextureManager.h"
#include <imgui.h>

void ComboSupportSpriteUi::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    for (int32_t i = 0; i < static_cast<int32_t>(BgType::COUNT); ++i) {
        bgSprites_[i].reset(KetaEngine::Sprite::Create(kBgTextures[i], true));
        bgSprites_[i]->transform_.scale = Vector2::ZeroVector();
    }

  /*  conditionSprite_.reset(KetaEngine::Sprite::Create(kConditionTexture, true));
    conditionSprite_->SetUVScale({0.5f, 1.0f});
    conditionSprite_->transform_.scale = Vector2::ZeroVector();*/
}

void ComboSupportSpriteUi::RegisterParams() {
    globalParameter_->Regist(groupName_, "bg0Offset", &bgOffsets_[0]);
    globalParameter_->Regist(groupName_, "bg1Offset", &bgOffsets_[1]);
    globalParameter_->Regist(groupName_, "conditionOffset", &conditionOffset_);
}

void ComboSupportSpriteUi::Update() {
    if (!pComboAsist_) {
        return;
    }
    UpdateBgPositions();
    UpdateConditionSprite();
}

void ComboSupportSpriteUi::UpdateBgPositions() {
    const Vector2& basePos = pComboAsist_->GetBasePosition();
    for (int32_t i = 0; i < static_cast<int32_t>(BgType::COUNT); ++i) {
        bgSprites_[i]->transform_.pos = basePos + bgOffsets_[i];
    }
}

void ComboSupportSpriteUi::UpdateConditionSprite() {
   /* const Vector2& basePos           = pComboAsist_->GetBasePosition();
    conditionSprite_->transform_.pos = basePos + conditionOffset_;

    using TriggerCondition = PlayerComboAttackData::TriggerCondition;
    const TriggerCondition condition = pComboAsist_->GetCurrentCondition();

    float uvPosX = (condition == TriggerCondition::AIR) ? 0.5f : 0.0f;
    conditionSprite_->SetUVPosition({uvPosX, 0.0f});*/
}

void ComboSupportSpriteUi::SetScale(const Vector2& scale) {
    for (auto& sprite : bgSprites_) {
        sprite->transform_.scale = scale;
    }
    if (conditionSprite_) {
        conditionSprite_->transform_.scale = scale;
    }
}

void ComboSupportSpriteUi::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("BG0 Offset", &bgOffsets_[0].x, 1.0f);
        ImGui::DragFloat2("BG1 Offset", &bgOffsets_[1].x, 1.0f);
        ImGui::DragFloat2("Condition Offset", &conditionOffset_.x, 1.0f);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
