#include "NextAttackUI.h"
#include <XInput.h>
#include <imgui.h>

void NextAttackUI::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void NextAttackUI::Update(float deltaTime) {
    if (state_ == State::Closing) {
        closeTimer_ -= deltaTime;
        if (closeTimer_ <= 0.0f) {
            HideAllSlots();
            state_ = State::Hidden;
        }
    }

    if (state_ != State::Hidden) {
        UpdateSlotTransforms();
    }
}

void NextAttackUI::Open(const std::vector<ComboAttackAction::NextAttackCandidate>& candidates) {
    // 既存スプライトを破棄
    for (auto& slot : slots_) {
        slot.sprite.reset();
        slot.isActive = false;
    }

    int32_t count = static_cast<int32_t>(candidates.size());
    if (count > kMaxCandidates) {
        count = kMaxCandidates;
    }

    for (int32_t i = 0; i < count; ++i) {
        if (!candidates[i].branch) {
            continue;
        }

        const int32_t gamepadButton   = candidates[i].branch->GetGamepadButton();
        const std::string textureName = GetButtonTexture(gamepadButton);

        slots_[i].sprite.reset(KetaEngine::Sprite::Create(textureName, false));
        if (slots_[i].sprite) {
            slots_[i].sprite->SetAnchorPoint({0.5f, 0.5f});
            slots_[i].sprite->SetIsDraw(true);
            slots_[i].sprite->PlaySpriteEaseAnimation("NextAttackOpen", "NextAttackUI");
        }
        slots_[i].isActive = true;
    }

    state_      = State::Showing;
    closeTimer_ = 0.0f;

    UpdateSlotTransforms();
}

void NextAttackUI::Close() {
    if (state_ == State::Hidden) {
        return;
    }

    for (auto& slot : slots_) {
        if (slot.isActive && slot.sprite) {
            slot.sprite->PlaySpriteEaseAnimation("NextAttackClose", "NextAttackUI");
        }
    }

    state_      = State::Closing;
    closeTimer_ = closeAnimDuration_;
}

void NextAttackUI::UpdateSlotTransforms() {
    for (int32_t i = 0; i < kMaxCandidates; ++i) {
        if (!slots_[i].isActive || !slots_[i].sprite) {
            continue;
        }
        slots_[i].sprite->transform_.pos   = basePos_ + slotOffsets_[i];
        slots_[i].sprite->transform_.scale = spriteScale_;
    }
}

void NextAttackUI::HideAllSlots() {
    for (auto& slot : slots_) {
        if (slot.sprite) {
            slot.sprite->SetIsDraw(false);
        }
        slot.isActive = false;
    }
}

std::string NextAttackUI::GetButtonTexture(int32_t gamepadButton) {
    if (gamepadButton == XINPUT_GAMEPAD_Y) {
        return "ButtomUI/OperateY.dds";
    }
    return "ButtomUI/OperateX.dds";
}

///=========================================================
/// パラメータ調整
///=========================================================
void NextAttackUI::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("基準位置", &basePos_.x, 0.5f);
        ImGui::DragFloat2("スプライトスケール", &spriteScale_.x, 0.5f);
        ImGui::DragFloat("Closeアニメ待機時間", &closeAnimDuration_, 0.01f, 0.0f, 3.0f);

        ImGui::SeparatorText("スロットオフセット");
        for (int32_t i = 0; i < kMaxCandidates; ++i) {
            ImGui::PushID(i);
            ImGui::DragFloat2(("スロット" + std::to_string(i)).c_str(), &slotOffsets_[i].x, 0.5f);
            ImGui::PopID();
        }

        globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
        globalParameter_->ParamLoadForImGui(groupName_, folderPath_);

        ImGui::PopID();
    }
#endif
}

void NextAttackUI::RegisterParams() {
    globalParameter_->Regist(groupName_, "basePos",           &basePos_);
    globalParameter_->Regist(groupName_, "spriteScale",       &spriteScale_);
    globalParameter_->Regist(groupName_, "closeAnimDuration", &closeAnimDuration_);

    for (int32_t i = 0; i < kMaxCandidates; ++i) {
        globalParameter_->Regist(groupName_, "slotOffset" + std::to_string(i), &slotOffsets_[i]);
    }
}
