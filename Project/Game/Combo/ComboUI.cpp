#include "ComboUI.h"
#include "base/TextureManager.h"
#include <cstdint>
#include <imgui.h>

void ComboUI::Init() {
    int32_t textureId = TextureManager::GetInstance()->LoadTexture("Resources/Texture/Number/Numbers.png");

    /// Sprite create
    sprite_.reset(Sprite::Create(textureId, Vector2::UnitVector(), Vector4::kWHITE()));

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncGroupFromUI(groupName_);
}

void ComboUI::Update() {
    sprite_->SetPosition(position_);
    sprite_->SetUVTranslate(Vector2(uvPosX_, 0.0f));
}
void ComboUI::Draw() {
}

///=========================================================
/// パラメータ調整
///==========================================================
void ComboUI::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {

        ImGui::DragFloat2("position", &position_.x, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void ComboUI::BindParams() {
    globalParameter_->Bind(groupName_, "position", &position_);
}

// 　桁数による数字の決定
void ComboUI::CalculateNumber(const int32_t& value) {

    switch (comboDigit_) {
    case ComboDigit::ONE:
        // 小数点以下第1位を取得
        valueForDigit_ = static_cast<int32_t>(value) % 10;
        break;

    case ComboDigit::TWO:
        // 整数部の第2位を取得
        valueForDigit_ = (static_cast<int32_t>(value) / 10) % 10;
        break;

    case ComboDigit::THREE:
        // 整数部の第2位を取得
        valueForDigit_ = (static_cast<int32_t>(value) / 100) % 10;
        break;
    default:
        valueForDigit_ = 0;
    }

    // UV座標のX位置を計算
    uvPosX_ = static_cast<float>(valueForDigit_) * 0.1f;
}

