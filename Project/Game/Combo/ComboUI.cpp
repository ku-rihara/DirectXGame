#include "ComboUI.h"
#include "base/TextureManager.h"
#include <cstdint>
#include <imgui.h>

void ComboUI::Init(const ComboDigit& digit) {

    /// Sprite create
    sprite_.reset(Sprite::Create("Number/Numbers.png"));
   
    CreateGroupName(digit);

    isVisible_ = true;
}

void ComboUI::Update(const Vector2& scale, const float& alpha) {

    sprite_->SetIsDraw(isVisible_);

  
    sprite_->transform_.scale = (Vector2(scale.x * 0.1f, scale.y));
    sprite_->uvTransform_.pos = (Vector2(uvPosX_, 0.0f));
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

// 　桁数による数字の決定
void ComboUI::CalculateNumber(const int32_t& value) {

    switch (comboDigit_) {
    case ComboDigit::ONE:
        // 小数点以下第1位を取得
        valueForDigit_ = static_cast<int32_t>(value) % 10;
        if (value == 0) {
            isVisible_ = false;
        } else {
            isVisible_ = true;
        }
        break;

    case ComboDigit::TWO:
        // 整数部の第2位を取得
        valueForDigit_ = (static_cast<int32_t>(value) / 10) % 10;
        if (value < 10) {
            isVisible_ = false;
        } else {
            isVisible_ = true;
        }
        break;

    case ComboDigit::THREE:
        // 整数部の第2位を取得
        valueForDigit_ = (static_cast<int32_t>(value) / 100) % 10;
        if (value < 100) {
            isVisible_ = false;
        } else {
            isVisible_ = true;
        }
        break;
    default:
        valueForDigit_ = 0;
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
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}