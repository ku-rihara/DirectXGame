#include "KillCountUI.h"
#include "base/TextureManager.h"
#include <cstdint>
#include <imgui.h>

void KillCountUI::Init(const KillCountDigit& digit) {

    /// Sprite create（明示的インデックスで固有グループ）
    std::string digitName;
    switch (digit) {
    case KillCountDigit::ONE:   digitName = "2"; break;
    case KillCountDigit::TWO:   digitName = "3"; break;
    case KillCountDigit::THREE: digitName = "4"; break;
    }
    sprite_.reset(KetaEngine::Sprite::Create("Number/Numbers.dds", true, digitName));

    CreateGroupName(digit);

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    InitDigitCalculators();

    isVisible_ = false;
    if (sprite_) {
        sprite_->transform_.scale = {0.0f, 0.0f};
    }
}

void KillCountUI::Update(const Vector2& scale, float alpha) {

    sprite_->SetIsDraw(isVisible_);

    sprite_->transform_.scale    = Vector2(scale.x * kUVScaleStep * scaleOffset_.x, scale.y * scaleOffset_.y);
    sprite_->transform_.rotate.y = rotateY_;
    sprite_->transform_.pos      = position_ + positionOffset_;
    sprite_->SetUVPosition(Vector2(uvPosX_, 0.0f));
    sprite_->SetAlpha(alpha);
}

///=========================================================
/// パラメータ調整
///==========================================================
void KillCountUI::AdjustParam() {

#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("Scale Offset", &scaleOffset_.x, 0.01f);
        ImGui::DragFloat2("Position Offset", &positionOffset_.x, 0.1f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void KillCountUI::RegisterParams() {
    globalParameter_->Regist(groupName_, "scaleOffset", &scaleOffset_);
    globalParameter_->Regist(groupName_, "positionOffset", &positionOffset_);
}

void KillCountUI::InitDigitCalculators() {
    // 1の位の計算
    digitCalculators_[KillCountDigit::ONE] = [this](int32_t value) {
        valueForDigit_ = value % 10;
        isVisible_     = (value != 0);
    };

    // 10の位の計算
    digitCalculators_[KillCountDigit::TWO] = [this](int32_t value) {
        valueForDigit_ = (value / 10) % 10;
        isVisible_     = (value >= 10);
    };

    // 100の位の計算
    digitCalculators_[KillCountDigit::THREE] = [this](int32_t value) {
        valueForDigit_ = (value / 100) % 10;
        isVisible_     = (value >= 100);
    };
}

// 桁数による数字の決定
void KillCountUI::CalculateNumber(int32_t value) {
    auto it = digitCalculators_.find(killCountDigit_);

    if (it != digitCalculators_.end()) {
        it->second(value);
    } else {
        valueForDigit_ = 0;
        isVisible_     = false;
    }

    // UV座標のX位置を計算
    uvPosX_ = static_cast<float>(valueForDigit_) * kUVScaleStep;
}

void KillCountUI::CreateGroupName(const KillCountDigit& digit) {
    switch (digit) {
    case KillCountDigit::ONE:
        groupName_ = "KillCountUI_OneDigit";
        break;

    case KillCountDigit::TWO:
        groupName_ = "KillCountUI_TwoDigit";
        break;

    case KillCountDigit::THREE:
        groupName_ = "KillCountUI_ThreeDigit";
        break;
    }
    killCountDigit_ = digit;
}
