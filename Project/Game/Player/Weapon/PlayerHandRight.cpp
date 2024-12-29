#include "PlayerHandRight.h"
/// imgui
#include<imgui.h>


PlayerHandRight::PlayerHandRight() {}

///=========================================================
///　初期化
///==========================================================
void PlayerHandRight::Init() {
	// グループネーム
	groupName_ = "RightHand";
	threeComboName_ = "ThreeComboRail";

	BaseObject::CreateModel("cube", ".obj");/// モデルセット
	BasePlayerHand::Init();

	railManager_->Init(threeComboName_);

}

///=========================================================
///　更新
///==========================================================
void PlayerHandRight::Update() {
	
	BasePlayerHand::Update();

	railManager_->Update(0.2f);
}

///=========================================================
///　描画
///==========================================================
void PlayerHandRight::Draw(const ViewProjection& viewProjection) {
	BasePlayerHand::Draw(viewProjection);
	railManager_->Draw(viewProjection);
}


void PlayerHandRight::AdjustParm() {

	BasePlayerHand::SetValues();

	if (ImGui::CollapsingHeader("RightHand")) {
		ImGui::PushID("RightHand");
		ImGui::SeparatorText("Transform");
		ImGui::DragFloat3("Position", &transform_.translation_.x, 0.1f);
		railManager_->ImGuiEdit();
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
	railManager_->SetParent(parent);
}
