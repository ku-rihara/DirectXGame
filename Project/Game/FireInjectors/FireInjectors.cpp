#include "FireInjectors.h"
#include"Combo/Combo.h"
#include <imgui.h>

void FireInjectors::Init() {

    putObjForBlender_ = std::make_unique<PutObjForBlender>();

    putObjForBlender_->LoadJsonFile("FireInjectors.json");
    putObjForBlender_->PutObject();

     // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void FireInjectors::Update() {
    putObjForBlender_->EmitterAllUpdate();
    putObjForBlender_->EmitAll();
    putObjForBlender_->EmitterAllEdit();

    const int currentCombo = pCombo_->GetComboCount();

    if (currentCombo == 0) {
        prevComboCount_    = 0;
        return;
    }

    // コンボが一気に飛んだ場合も検出できるように
    for (int i = prevComboCount_ + 1; i <= currentCombo; ++i) {
        if (i % fireShotComboNum_ == 0) {
            putObjForBlender_->StartRailEmitAll();
            break;
        }
    }

    prevComboCount_ = currentCombo;
}



void FireInjectors::Draw(const ViewProjection& viewProjection) {
    putObjForBlender_->DrawAll(viewProjection);
}


///=========================================================
/// バインド
///==========================================================
void FireInjectors::BindParams() {
    globalParameter_->Bind(groupName_, "fireShotComboNum", &fireShotComboNum_);
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