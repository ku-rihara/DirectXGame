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
	forthComboName_ = "ForthComboRail";

	BaseObject::CreateModel("cube",	".obj");/// モデルセット
	BasePlayerHand::Init();	

	/// rail
	threeComboRailManager_ = std::make_unique<RailManager>();
	threeComboRailManager_->Init(threeComboName_);

	forthComboRailManager_ = std::make_unique<RailManager>();
	forthComboRailManager_->Init(forthComboName_);

}

///=========================================================
///　更新
///==========================================================
void PlayerHandRight::Update() {
	forthComboRailManager_->Update(0.1f, RailManager::PositionMode::LOCAL, (Vector3(1, 1, 1)));
	BasePlayerHand::Update();
	
}

void PlayerHandRight::RailThreeComboUpdate(const float& speed) {
	/// 3コンボレール更新
	threeComboRailManager_->Update(speed, RailManager::PositionMode::LOCAL, (Vector3(1, 1, 1)));
	SetWorldPosition(threeComboRailManager_->GetPositionOnRail());
}

void PlayerHandRight::RailForthComboUpdate(const float& speed) {
	/// 4コンボレール更新
	forthComboRailManager_->Update(speed, RailManager::PositionMode::LOCAL, (Vector3(1, 1, 1)));
	SetWorldPosition(forthComboRailManager_->GetPositionOnRail());
}

///=========================================================
///　描画
///==========================================================
void PlayerHandRight::Draw(const ViewProjection& viewProjection) {
	BasePlayerHand::Draw(viewProjection);
	forthComboRailManager_->Draw(viewProjection);

}

void PlayerHandRight::AdjustParm() {

	BasePlayerHand::SetValues();

	if (ImGui::CollapsingHeader("RightHand")) {
		ImGui::PushID("RightHand");
		BasePlayerHand::AjustParmBase();
		threeComboRailManager_->ImGuiEdit();
		forthComboRailManager_->ImGuiEdit();
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
	forthComboRailManager_->SetParent(parent);
}