#include "ComboUI.h"
#include "base/TextureManager.h"
#include <cstdint>
#include <imgui.h>

void ComboUI::Init(const ComboDigit& digit) {

    /// Sprite create
    sprite_.reset(KetaEngine::Sprite::Create("Number/Numbers.dds"));

    CreateGroupName(digit);

    InitDigitCalculators();

    isVisible_ = true;
}

void ComboUI::Update(const Vector2& scale, float alpha) {

    sprite_->SetIsDraw(isVisible_);

    sprite_->transform_.scale = (Vector2(scale.x * 0.1f, scale.y));
    sprite_->transform_.pos   = position_; 
    sprite_->SetUVPosition((Vector2(uvPosX_, 0.0f)));
    sprite_->SetAlpha(alpha);
}

///=========================================================
/// パラメータ調整
///==========================================================
void ComboUI::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("position", &position_.x, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void ComboUI::RegisterParams() {
    globalParameter_->Regist(groupName_, "position", &position_);
}

void ComboUI::InitDigitCalculators() {
    // 1の位の計算
    digitCalculators_[ComboDigit::ONE] = [this](int32_t value) {
        valueForDigit_ = value % 10;
        isVisible_     = (value != 0);
    };

    // 10の位の計算
    digitCalculators_[ComboDigit::TWO] = [this](int32_t value) {
        valueForDigit_ = (value / 10) % 10;
        isVisible_     = (value >= 10);
    };

    // 100の位の計算
    digitCalculators_[ComboDigit::THREE] = [this](int32_t value) {
        valueForDigit_ = (value / 100) % 10;
        isVisible_     = (value >= 100);
    };
}

// 　桁数による数字の決定
void ComboUI::CalculateNumber(int32_t value) {
    // 対応する計算関数を実行
    auto it = digitCalculators_.find(comboDigit_);

    if (it != digitCalculators_.end()) {
        it->second(value);
    } else {
        // デフォルト処理
        valueForDigit_ = 0;
        isVisible_     = false;
    }

    // UV座標のX位置を計算
    uvPosX_ = static_cast<float>(valueForDigit_) * 0.1f;
}
void ComboUI::CreateGroupName(const ComboDigit& digit) {
    switch (digit) {
    case ComboDigit::ONE:
        groupName_ = "ComboUI_OneDigit";
        break;

    case ComboDigit::TWO:
        groupName_ = "ComboUI_TwoDigit";
        break;

    case ComboDigit::THREE:
        groupName_ = "ComboUI_ThreeDigit";
        break;
    }
    comboDigit_ = digit;

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}