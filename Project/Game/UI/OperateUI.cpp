#include "OperateUI.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include "input/Input.h"
#include "input/InputData.h"
#include <imgui.h>

void OperateUI::Init() {

    for (int32_t i = 0; i < static_cast<int32_t>(OperateButtonType::COUNT); ++i) {
        // スプライト生成
        uiParam_[i].sprite_.reset(KetaEngine::Sprite::Create(kButtonTextures[i], true));
        uiParam_[i].sprite_->transform_.scale = Vector2::ZeroVector();
       /* uiParam_[i].outLineSprite_.reset(KetaEngine::Sprite::Create(kOutLineTexture));
        uiParam_[i].uiFrame.reset(KetaEngine::Sprite::Create(frameTexture));*/
    }

    // GlobalParameter
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void OperateUI::Update() {

    for (int32_t i = 0; i < static_cast<int32_t>(OperateButtonType::COUNT); ++i) {
        auto& sprite = uiParam_[i].sprite_;
        if (!sprite) {
            continue;
        }

        if (IsPushCondition(static_cast<OperateButtonType>(i))) {
            // 押下中: スケール縮小 + 個別カラー適用
            sprite->transform_.scale = currentScale_ * pressScaleMultiplier_;
            sprite->SetColor(pressColors_[i]);
        } else {
            // 通常: 基準スケール + 白（カラー変化なし）
            sprite->transform_.scale = currentScale_;
            sprite->SetColor({1.0f, 1.0f, 1.0f});
        }
    }
}

void OperateUI::SetScale(const Vector2& scale) {
    currentScale_ = scale;
}

///=========================================================
/// 押下判定
///=========================================================
bool OperateUI::IsPushCondition(OperateButtonType type) {
    // ボタン種別ごとに対応するゲームパッドボタンをマッピング
    static constexpr GamepadButton kButtonMap[] = {
        GamepadButton::X,  // OperateButtonType::X
        GamepadButton::Y,  // OperateButtonType::Y
        GamepadButton::B,  // OperateButtonType::B
        GamepadButton::A,  // OperateButtonType::A
        GamepadButton::LB, // OperateButtonType::LB
    };

    const int32_t idx = static_cast<int32_t>(type);
    return KetaEngine::Input::IsPressPad(0, kButtonMap[idx]);
}

///=========================================================
/// パラメータ登録
///=========================================================
void OperateUI::RegisterParams() {
    // 押下時スケール倍率（共通）
    globalParameter_->Regist(groupName_, "pressScaleMultiplier", &pressScaleMultiplier_);

    // 押下時カラー（ボタンごとに個別）
    const char* buttonNames[] = {"X", "Y", "B", "A", "LB"};
    for (int32_t i = 0; i < static_cast<int32_t>(OperateButtonType::COUNT); ++i) {
        std::string key = std::string("pressColor_") + buttonNames[i];
        globalParameter_->Regist(groupName_, key, &pressColors_[i]);
    }
}

///=========================================================
/// デバッグ
///=========================================================
void OperateUI::Debug() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("OperateUI")) {
        ImGui::PushID("OperateUI");

        ImGui::SeparatorText("Press Reaction");
        ImGui::DragFloat("Press Scale Multiplier", &pressScaleMultiplier_, 0.01f, 0.1f, 1.0f);

        ImGui::SeparatorText("Press Colors");
        const char* buttonNames[] = {"X", "Y", "B", "A", "LB"};
        for (int32_t i = 0; i < static_cast<int32_t>(OperateButtonType::COUNT); ++i) {
            std::string label = std::string("Color ") + buttonNames[i];
            ImGui::ColorEdit3(label.c_str(), &pressColors_[i].x);
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
