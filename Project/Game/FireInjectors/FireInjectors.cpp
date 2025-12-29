#include "FireInjectors.h"
#include "Combo/Combo.h"
#include"Frame/Frame.h"
#include <imgui.h>

void FireInjectors::Init() {

    putObjForBlender_ = std::make_unique<KetaEngine::PutObjForBlender>();
    putObjForBlender_->LoadJsonFile("FireInjectors.json");
    putObjForBlender_->EasingAllReset();

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void FireInjectors::Update() {
    putObjForBlender_->EmitterAllUpdate();
    putObjForBlender_->EmitAll();
    putObjForBlender_->EmitterAllEdit();

    const int currentCombo = pCombo_->GetComboCount();

    if (currentCombo == 0) {
        prevComboCount_ = 0;
        return;
    }

    // 発射
    for (int i = prevComboCount_ + 1; i <= currentCombo; ++i) {
        if (i % fireShotComboNum_ == 0) {
            putObjForBlender_->StartRailEmitAll();
            break;
        }
    }

    prevComboCount_ = currentCombo;
}

void FireInjectors::Spawn() {
    putObjForBlender_->EasingUpdateSelectGroup(KetaEngine::Frame::DeltaTime(), 0);
}
void FireInjectors::Launch() {
    putObjForBlender_->EasingUpdateSelectGroup(KetaEngine::Frame::DeltaTime(), 1);
}


///=========================================================
/// バインド
///==========================================================
void FireInjectors::RegisterParams() {
    globalParameter_->Regist(groupName_, "fireShotComboNum", &fireShotComboNum_);
}

///=========================================================
/// パラメータ調整
///==========================================================
void FireInjectors::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::InputInt("fireShotComboNum", &fireShotComboNum_);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void FireInjectors::SetCombo(Combo* combo) {
    pCombo_ = combo;
}