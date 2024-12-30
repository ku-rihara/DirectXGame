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

	combo3MoveObj_.reset(Object3d::CreateModel("cube", ".obj"));
	
}

///=========================================================
///　更新
///==========================================================
void PlayerHandRight::Update() {

	///*Todo
	/// ワールド座標しか取得できない
	
	BasePlayerHand::Update();

	railManager_->Update(0.2f,RailManager::PositionMode::LOCAL,(Vector3(1,1,1)));

	combo3MovePos_ = transform_.parent_->translation_+railManager_->GetWorldPos();
}

///=========================================================
///　描画
///==========================================================
void PlayerHandRight::Draw(const ViewProjection& viewProjection) {
	BasePlayerHand::Draw(viewProjection);
	railManager_->Draw(viewProjection);
	combo3MoveObj_->Draw(combo3MovePos_, viewProjection);
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
