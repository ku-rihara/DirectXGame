#include "LevelUI.h"
#include <imgui.h>

void LevelUI::Init(Digit digit) {

    // スプライト初期化（明示的インデックスで固有グループ）
    const std::string digitName = (digit == Digit::ONE) ? "0" : "1";
    sprite_.reset(KetaEngine::Sprite::Create("Number/Numbers.dds", true, digitName));
    if (sprite_) {
        sprite_->SetUVScale({kUVStep, 1.0f});
    }

    CreateGroupName(digit);

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    InitDigitCalculators();

    if (sprite_) {
        sprite_->transform_.scale = {0.0f, 0.0f};
    }
}

void LevelUI::Update(const Vector2& scale, float alpha) {
    sprite_->SetIsDraw(isVisible_);
    sprite_->transform_.scale    = Vector2(scale.x * kUVStep * scaleOffset_.x, scale.y * scaleOffset_.y);
    sprite_->transform_.pos      = position_ + positionOffset_;
    sprite_->SetUVPosition(Vector2(uvPosX_, 0.0f));
    sprite_->SetAlpha(alpha);
}

void LevelUI::CalculateNumber(int32_t value) {
    auto it = digitCalculators_.find(digit_);
    if (it != digitCalculators_.end()) {
        it->second(value);
    } else {
        valueForDigit_ = 0;
        isVisible_     = false;
    }
    uvPosX_ = static_cast<float>(valueForDigit_) * kUVStep;
}

void LevelUI::InitDigitCalculators() {
    digitCalculators_[Digit::ONE] = [this](int32_t value) {
        valueForDigit_ = value % 10;
        isVisible_     = (value != 0);
    };
    digitCalculators_[Digit::TWO] = [this](int32_t value) {
        valueForDigit_ = (value / 10) % 10;
        isVisible_     = (value >= 10);
    };
}

void LevelUI::CreateGroupName(Digit digit) {
    switch (digit) {
    case Digit::ONE: groupName_ = "LevelUI_OneDigit"; break;
    case Digit::TWO: groupName_ = "LevelUI_TwoDigit"; break;
    default:         groupName_ = "LevelUI_Unknown";  break;
    }
    digit_ = digit;
}

///=========================================================
/// パラメータ調整
///==========================================================
void LevelUI::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());
        ImGui::DragFloat2("Scale Offset",    &scaleOffset_.x,    0.01f);
        ImGui::DragFloat2("Position Offset", &positionOffset_.x, 0.1f);
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }
#endif
}

void LevelUI::RegisterParams() {
    globalParameter_->Regist(groupName_, "scaleOffset",    &scaleOffset_);
    globalParameter_->Regist(groupName_, "positionOffset", &positionOffset_);
}
