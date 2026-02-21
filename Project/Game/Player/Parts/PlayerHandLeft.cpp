#include "PlayerHandLeft.h"
#include "Light/LightingType.h"
/// imgui
#include <imgui.h>

///=========================================================
/// 　初期化
///==========================================================
void PlayerHandLeft::Init() {

    // グループネーム
    groupName_ = "LeftHand";

    obj3d_.reset(KetaEngine::Object3d::CreateModel("Player/LHand.obj"));
    obj3d_->transform_.scale_                                             = {2, 2, 2};
    obj3d_->GetModelMaterial()->GetMaterialData()->enableLighting         = static_cast<int32_t>(KetaEngine::LightingType::HalfLambert);
    obj3d_->GetModelMaterial()->GetMaterialData()->environmentCoefficient = 0.15f;

    BasePlayerHand::Init();
}

///=========================================================
/// 　更新
///==========================================================
void PlayerHandLeft::Update() {
    BasePlayerHand::EffectEmit("DefaultFireAura");
    BasePlayerHand::Update();
}

///=====================================================
///  パラメータ調節
///=====================================================
void PlayerHandLeft::AdjustParam() {

    if (ImGui::CollapsingHeader("LeftHand")) {
        ImGui::PushID("LeftHand");
        BasePlayerHand::AdjustParamBase();
        ImGui::PopID();
    }
}

void PlayerHandLeft::DissolveAdapt(float dissolve) {
    BasePlayerHand::DissolveAdapt(dissolve);
}


void PlayerHandLeft::SetParent(KetaEngine::WorldTransform* parent) {
    BasePlayerHand::SetParent(parent);
}
