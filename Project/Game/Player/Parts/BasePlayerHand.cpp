#include "BasePlayerHand.h"

/// imgui
#include "base/TextureManager.h"
#include <imgui.h>
///=========================================================
/// 　初期化
///==========================================================
void BasePlayerHand::Init() {

    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    AddParamGroup();
    ApplyGlobalParameter();

    emitter_.reset(ParticleEmitter::CreateParticlePrimitive(groupName_, PrimitiveType::Plane, 300));
    uint32_t handle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/circle.png");
    emitter_->SetTextureHandle(handle);
}

///=========================================================
/// 　更新
///==========================================================
void BasePlayerHand::Update() {

    obj3d_->SetIsShadow(isShadow_);

    // エミッター更新
    emitter_->SetTargetPosition(obj3d_->transform_.GetWorldPos());
    emitter_->Update();
    emitter_->EditorUpdate();
    if (isEmit_) {
        emitter_->Emit();
    }

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
    globalParameter_->AddItem(groupName_, "RailRunSpeed", railRunSpeedThree_);
    globalParameter_->AddItem(groupName_, "RailRunSpeedF", railRunSpeedForth_);
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void BasePlayerHand::SetValues() {

    // グループを追加
    globalParameter_->CreateGroup(groupName_);

    // Position
    globalParameter_->SetValue(groupName_, "Translate", obj3d_->transform_.translation_);
    globalParameter_->SetValue(groupName_, "RailRunSpeed", railRunSpeedThree_);
    globalParameter_->SetValue(groupName_, "RailRunSpeedF", railRunSpeedForth_);
}

///=====================================================
///  ImGuiからパラメータを得る
///=====================================================
void BasePlayerHand::ApplyGlobalParameter() {
    // Position
    obj3d_->transform_.translation_ = globalParameter_->GetValue<Vector3>(groupName_, "Translate");
    railRunSpeedThree_              = globalParameter_->GetValue<float>(groupName_, "RailRunSpeed");
    railRunSpeedForth_              = globalParameter_->GetValue<float>(groupName_, "RailRunSpeedF");
}

///=====================================================
///  セーブロード
///=====================================================
void BasePlayerHand::SaveAndLoad() {

    globalParameter_->ParamSaveForImGui(groupName_);
    ParamLoadForImGui();
}
void BasePlayerHand::DissolveAdapt(float dissolve) {
    obj3d_->material_.SetDissolveEdgeColor(Vector3(0.6706f, 0.8824f, 0.9804f));
    obj3d_->material_.SetDissolveEdgeWidth(0.05f);
    obj3d_->material_.SetEnableDissolve(true);
    obj3d_->material_.SetDissolveThreshold(dissolve);
}

void BasePlayerHand::AdjustParamBase() {
    ImGui::SeparatorText("Param");
    ImGui::DragFloat3("Position", &obj3d_->transform_.translation_.x, 0.1f);
    ImGui::DragFloat("RailRunSpeedThree", &railRunSpeedThree_, 0.01f);
    ImGui::DragFloat("RailRunSpeedForth", &railRunSpeedForth_, 0.01f);
}

void BasePlayerHand::SetParent(WorldTransform* parent) {
    obj3d_->transform_.parent_ = parent;
}

void BasePlayerHand::SetBlendModeSub() {
    emitter_->SetBlendMode(BlendMode::Subtractive);
}
void BasePlayerHand::SetBlendModeAdd() {
    emitter_->SetBlendMode(BlendMode::Add);
}