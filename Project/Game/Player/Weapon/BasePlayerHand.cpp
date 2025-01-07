#include "BasePlayerHand.h"
/// imgui
#include<imgui.h>
#include"base/TextureManager.h"
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

	obj3d_->material_.materialData_->enableLighting = 3;

	emitter_.reset(ParticleEmitter::CreateParticle(groupName_, "plane", ".obj", 300, false));
	uint32_t handle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/circle.png");
	emitter_->SetTextureHandle(handle);
}

///=========================================================
///　更新
///==========================================================
void BasePlayerHand::Update() {

	// エミッター更新
	emitter_->SetTargetPosition(GetWorldPosition());
	emitter_->Update();
	emitter_->EditorUpdate();
	emitter_->Emit();

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
	globalParameter_->AddItem(groupName_, "RailRunSpeed", railRunSpeedThree_);
	globalParameter_->AddItem(groupName_, "RailRunSpeedF", railRunSpeedForth_);

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
	globalParameter_->SetValue(groupName_, "RailRunSpeed", railRunSpeedThree_);
	globalParameter_->SetValue(groupName_, "RailRunSpeedF", railRunSpeedForth_);

}


///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void BasePlayerHand::ApplyGlobalParameter() {
	// Position
	transform_.translation_ = globalParameter_->GetValue<Vector3>(groupName_, "Translate");
	railRunSpeedThree_ = globalParameter_->GetValue<float>(groupName_, "RailRunSpeed");
	railRunSpeedForth_ = globalParameter_->GetValue<float>(groupName_, "RailRunSpeedF");
}

///=====================================================
///  セーブロード
///=====================================================
void BasePlayerHand::SaveAndLoad() {

	globalParameter_->ParmSaveForImGui(groupName_);
	ParmLoadForImGui();
}

void BasePlayerHand::AjustParmBase() {
	ImGui::SeparatorText("Param");
	ImGui::DragFloat3("Position", &transform_.translation_.x, 0.1f);
	ImGui::DragFloat("RailRunSpeedThree", &railRunSpeedThree_, 0.01f);
	ImGui::DragFloat("RailRunSpeedForth", &railRunSpeedForth_, 0.01f);
}

void BasePlayerHand::SetParent(WorldTransform* parent) {
	transform_.parent_ = parent;
}