#include "BasePlayerHand.h"

/// imgui
#include "base/TextureManager.h"
#include <imgui.h>
///=========================================================
/// 　初期化
///==========================================================
void BasePlayerHand::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    AddParamGroup();
    ApplyGlobalParameter();

    particlePlayer_ = std::make_unique<KetaEngine::ParticlePlayer>();
    particlePlayer_->Init();
    particlePlayer_->SetFollowingPos(&effectFollowPos_);
}

///=========================================================
/// 　更新
///==========================================================
void BasePlayerHand::Update() {

    obj3d_->SetIsShadow(isShadow_);

    // エミッター更新
    particlePlayer_->SetTargetPosition(obj3d_->transform_.GetWorldPos());
    effectFollowPos_ = obj3d_->transform_.GetWorldPos();
    particlePlayer_->Update();

    BaseObject::Update();
}

///=================================================================================
/// ロード
///=================================================================================
void BasePlayerHand::ParamLoadForImGui() {

    // ロードボタン
    if (ImGui::Button(std::format("Load {}", groupName_).c_str())) {

        globalParameter_->LoadFile(groupName_);
        // セーブ完了メッセージ
        ImGui::Text("Load Successful: %s", groupName_.c_str());
        ApplyGlobalParameter();
    }
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void BasePlayerHand::AddParamGroup() {
    globalParameter_->CreateGroup(groupName_);

    // Position
    globalParameter_->AddItem(groupName_, "Translate", obj3d_->transform_.translation_);
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void BasePlayerHand::SetValues() {

    // グループを追加
    globalParameter_->CreateGroup(groupName_);

    // Position
    globalParameter_->SetValue(groupName_, "Translate", obj3d_->transform_.translation_);
}

///=====================================================
///  ImGuiからパラメータを得る
///=====================================================
void BasePlayerHand::ApplyGlobalParameter() {
    // Position
    obj3d_->transform_.translation_ = globalParameter_->GetValue<Vector3>(groupName_, "Translate");
}

///=====================================================
///  セーブロード
///=====================================================
void BasePlayerHand::SaveAndLoad() {

    globalParameter_->ParamSaveForImGui(groupName_);
    ParamLoadForImGui();
}
void BasePlayerHand::DissolveAdapt(float dissolve) {
    obj3d_->GtModelMaterial()->GetMaterialData()->dissolveEdgeColor = Vector3(0.6706f, 0.8824f, 0.9804f);
    obj3d_->GtModelMaterial()->GetMaterialData()->dissolveEdgeWidth = 0.05f;
    obj3d_->GtModelMaterial()->GetMaterialData()->enableDissolve    = true;
    obj3d_->GtModelMaterial()->GetMaterialData()->dissolveThreshold = dissolve;
}

void BasePlayerHand::AdjustParamBase() {
    ImGui::SeparatorText("Param");
    ImGui::DragFloat3("Position", &obj3d_->transform_.translation_.x, 0.1f);
}

void BasePlayerHand::SetParent(KetaEngine::WorldTransform* parent) {
    obj3d_->transform_.parent_ = parent;
}

void BasePlayerHand::EffectEmit(const std::string& effectName) {
    if (isEmit_) {
        particlePlayer_->Play("Player", effectName);
    }
}