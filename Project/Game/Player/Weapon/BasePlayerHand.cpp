#include "BasePlayerHand.h"
/// imgui
#include<imgui.h>



///=========================================================
///　初期化
///==========================================================
void BasePlayerHand::Init() {

	BaseObject::Init();

	///* グローバルパラメータ
	globalParameter_ = GlobalParameter::GetInstance();
	globalParameter_->CreateGroup(groupName_, false);
	AddParmGroup();
	ApplyGlobalParameter();

	emitter_.reset(ParticleEmitter::CreateParticle(groupName_, "DebugCube", ".obj", 300, false));

}

///=========================================================
///　更新
///==========================================================
void BasePlayerHand::Update() {

	// エミッター更新
	emitter_->SetTargetPosition(GetWorldPosition());
	emitter_->Update();
	emitter_->Emit();

	
	//向いている方向を計算
	Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform_.rotation_.y);
	Vector3 forward = { 0, 0, 1 };
	Vector3 direction = TransformNormal(forward, rotateMatrix);
	direction_ = Vector3::Normalize(direction);

	BaseObject::Update();

}

///============================================================
///　描画
///==========================================================
void BasePlayerHand::Draw(const ViewProjection& viewProjection) {
	BaseObject::Draw(viewProjection);
}


///=================================================================================
/// ロード
///=================================================================================
void BasePlayerHand::ParmLoadForImGui() {

	// ロードボタン
	if (ImGui::Button(std::format("Load {}", groupName_).c_str())) {

		globalParameter_->LoadFile(groupName_);
		// セーブ完了メッセージ
		ImGui::Text("Load Successful: %s", groupName_.c_str());
		ApplyGlobalParameter();
	}
}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void BasePlayerHand::AddParmGroup() {
	globalParameter_->CreateGroup(groupName_, false);

	// Position
	//globalParameter_->AddSeparatorText("Position");
	globalParameter_->AddItem(groupName_, "Translate", transform_.translation_);

}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void BasePlayerHand::SetValues() {

	// グループを追加(GlobalParamaterで表示はしない)
	globalParameter_->CreateGroup(groupName_, false);

	// Position
	//globalParameter_->AddSeparatorText("Position");
	globalParameter_->SetValue(groupName_, "Translate", transform_.translation_);

}


///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void BasePlayerHand::ApplyGlobalParameter() {
	// Position
	transform_.translation_ = globalParameter_->GetValue<Vector3>(groupName_, "Translate");

}

///=====================================================
///  セーブロード
///=====================================================
void BasePlayerHand::SaveAndLoad() {

	globalParameter_->ParmSaveForImGui(groupName_);
	ParmLoadForImGui();
}