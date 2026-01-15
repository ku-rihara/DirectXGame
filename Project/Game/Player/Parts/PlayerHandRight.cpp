#include "PlayerHandRight.h"
/// imgui
#include <imgui.h>

///==========================================================
/// 　初期化
///==========================================================
void PlayerHandRight::Init() {

    // グループネーム
    groupName_ = "RightHand";

    obj3d_.reset(KetaEngine::Object3d::CreateModel("RHand.obj"));
    obj3d_->GetModelMaterial()->GetMaterialData()->enableLighting         = 2;
    obj3d_->transform_.scale_                                   = {2, 2, 2};
    obj3d_->GetModelMaterial()->GetMaterialData()->environmentCoefficient = 0.15f;

    BasePlayerHand::Init();
}

///=========================================================
/// 　更新
///==========================================================
void PlayerHandRight::Update() {
    BasePlayerHand::EffectEmit("DefaultFireAuraBlue");
    BasePlayerHand::Update();
}

void PlayerHandRight::AdjustParam() {

    BasePlayerHand::SetValues();

    if (ImGui::CollapsingHeader("RightHand")) {
        ImGui::PushID("RightHand");
        BasePlayerHand::AdjustParamBase();

        SaveAndLoad();
        ImGui::PopID();
    }
}

void PlayerHandRight::DissolveAdapt(float dissolve) {
    BasePlayerHand::DissolveAdapt(dissolve);
}

///=====================================================
///  セーブロード
///=====================================================
void PlayerHandRight::SaveAndLoad() {

    BasePlayerHand::SaveAndLoad();
}

void PlayerHandRight::SetParent(KetaEngine::WorldTransform* parent) {
    BasePlayerHand::SetParent(parent);
}
