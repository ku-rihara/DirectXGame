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
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

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
/// パラメータ登録
///=================================================================================
void BasePlayerHand::RegisterParams() {
    globalParameter_->Regist(groupName_, "Translate", &obj3d_->transform_.translation_);
}

void BasePlayerHand::AdjustParamBase() {
    ImGui::SeparatorText("Param");
    ImGui::DragFloat3("Position", &obj3d_->transform_.translation_.x, 0.1f);
    globalParameter_->ParamSaveForImGui(groupName_);
    globalParameter_->ParamLoadForImGui(groupName_);
}


void BasePlayerHand::DissolveAdapt(float dissolve) {
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveEdgeColor = Vector3(0.6706f, 0.8824f, 0.9804f);
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveEdgeWidth = 0.05f;
    obj3d_->GetModelMaterial()->GetMaterialData()->enableDissolve    = true;
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveThreshold = dissolve;
}

void BasePlayerHand::SetParent(KetaEngine::WorldTransform* parent) {
    obj3d_->transform_.parent_ = parent;
}

void BasePlayerHand::EffectEmit(const std::string& effectName) {
    if (isEmit_) {
        particlePlayer_->Play(effectName,"Player");
    }
}