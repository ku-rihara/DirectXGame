#include "PlayerHandLeft.h"
/// imgui
#include <imgui.h>

///=========================================================
/// 　初期化
///==========================================================
void PlayerHandLeft::Init() {

    // グループネーム
    groupName_     = "LeftHand";
    railGroupName_ = "ThrustRail";

    BaseObject::CreateModel("LHand.obj"); /// モデルセット
    BasePlayerHand::Init();

    trustRailManager_ = std::make_unique<RailManager>();
    trustRailManager_->Init(railGroupName_);
}

///=========================================================
/// 　更新
///==========================================================
void PlayerHandLeft::Update() {

    BasePlayerHand::Update();
}

void PlayerHandLeft::RailForthComboUpdate(const float& speed) {
    /// 突き飛ばしコンボレール更新
    trustRailManager_->Update(speed, RailManager::PositionMode::LOCAL, (Vector3(1, 1, 1)));
    SetWorldPosition(trustRailManager_->GetPositionOnRail());
}

///=========================================================
/// 　描画
///==========================================================
void PlayerHandLeft::Draw(const ViewProjection& viewProjection) {
    BasePlayerHand::Draw(viewProjection);
}

///=====================================================
///  パラメータ調節
///=====================================================
void PlayerHandLeft::AdjustParm() {

    BasePlayerHand::SetValues(); /// setvalue

    if (ImGui::CollapsingHeader("LeftHand")) {
        ImGui::PushID("LeftHand");
        BasePlayerHand::AjustParmBase();
        trustRailManager_->ImGuiEdit();
        SaveAndLoad();
        ImGui::PopID();
    }
}

void PlayerHandLeft::DissolveAdapt(const float& dissolve) {
    BasePlayerHand::DissolveAdapt(dissolve);
}

///=====================================================
///  セーブロード
///=====================================================
void PlayerHandLeft::SaveAndLoad() {
    BasePlayerHand::SaveAndLoad();
}

void PlayerHandLeft::SetParent(WorldTransform* parent) {
    BasePlayerHand::SetParent(parent);
}

void PlayerHandLeft::SetRailParent(WorldTransform* parent) {
    trustRailManager_->SetParent(parent);
}