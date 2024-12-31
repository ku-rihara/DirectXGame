#include "PlayerHandLeft.h"
/// imgui
#include<imgui.h>


PlayerHandLeft::PlayerHandLeft() {}

///=========================================================
///　初期化
///==========================================================
void PlayerHandLeft::Init() {

	// グループネーム
	groupName_ = "LeftHand";

	BaseObject::CreateModel("cube", ".obj");/// モデルセット
	BasePlayerHand::Init();

}

///=========================================================
///　更新
///==========================================================
void PlayerHandLeft::Update() {
	
	BasePlayerHand::Update();
	
}

void PlayerHandLeft::RailUpdate() {
	
}


///=========================================================
///　描画
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
		SaveAndLoad();
		ImGui::PopID();
	}
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