#include "PlayerHandRight.h"
/// imgui
#include<imgui.h>


PlayerHandRight::PlayerHandRight() {}

///==========================================================
///　初期化
///==========================================================
void PlayerHandRight::Init() {
	// グループネーム
	groupName_      = "RightHand";
	threeComboName_ = "ThreeComboRail";
	forthComboName_ = "StopPunchRail";

	BaseObject::CreateModel("cube",	".obj");/// モデルセット
	BasePlayerHand::Init();	

	/// rail
	threeComboRailManager_ = std::make_unique<RailManager>();
	threeComboRailManager_->Init(threeComboName_);

	stopPunchRailManager_ = std::make_unique<RailManager>();
	stopPunchRailManager_->Init(forthComboName_);
}

///=========================================================
///　更新
///==========================================================
void PlayerHandRight::Update() {
	stopPunchRailManager_->Update(0.1f, RailManager::PositionMode::LOCAL, (Vector3(1, 1, 1)));
	BasePlayerHand::Update();
	
}

void PlayerHandRight::RailThreeComboUpdate(const float& speed) {
	/// 3コンボレール更新
	threeComboRailManager_->Update(speed, RailManager::PositionMode::LOCAL, (Vector3(1, 1, 1)));
	SetWorldPosition(threeComboRailManager_->GetPositionOnRail());
}

void PlayerHandRight::RailForthComboUpdate(const float& speed) {
	/// 4コンボレール更新
	stopPunchRailManager_->Update(speed, RailManager::PositionMode::LOCAL, (Vector3(1, 1, 1)));
	SetWorldPosition(stopPunchRailManager_->GetPositionOnRail());
}

///=========================================================
///　描画
///==========================================================
void PlayerHandRight::Draw(const ViewProjection& viewProjection) {
	BasePlayerHand::Draw(viewProjection);
	stopPunchRailManager_->Draw(viewProjection);

}

void PlayerHandRight::AdjustParm() {

	BasePlayerHand::SetValues();

	if (ImGui::CollapsingHeader("RightHand")) {
		ImGui::PushID("RightHand");
		BasePlayerHand::AjustParmBase();
		threeComboRailManager_->ImGuiEdit();
		stopPunchRailManager_->ImGuiEdit();
		SaveAndLoad();
		ImGui::PopID();
	}
}

///=====================================================
///  セーブロード
///=====================================================
void PlayerHandRight::SaveAndLoad() {

	BasePlayerHand::SaveAndLoad();
}


void PlayerHandRight::SetParent(WorldTransform* parent) {
	BasePlayerHand::SetParent(parent);
}

void PlayerHandRight::SetRailParent(WorldTransform* parent) {
	threeComboRailManager_->SetParent(parent);
	stopPunchRailManager_->SetParent(parent);
}