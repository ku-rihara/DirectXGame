#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include<format>
#include <imgui.h>


ParticleEmitter::ParticleEmitter() {}

ParticleEmitter* ParticleEmitter::CreateParticle(const std::string& name, const std::string& modelFilePath,
	const std::string& extension, const int32_t& maxnum) {

	auto emitter = std::make_unique<ParticleEmitter>();
	emitter->particleName_ = name;
	ParticleManager::GetInstance()->CreateParticleGroup(emitter->particleName_, modelFilePath, extension, maxnum);
	emitter->Init();
	return emitter.release();
}

void ParticleEmitter::Init() {
	particleCount_ = 0;
	lifeTime_ = 0.0f;
	baseColor_ = { 0, 0, 0, 0 };
	colorDist_.min = { 0, 0, 0, 0 };
	colorDist_.max = { 0, 0, 0, 0 };

	////JSON関連
	AddParmGroup();
	ApplyGlobalParameter();
}

void ParticleEmitter::Emit() {
	ParticleManager::GetInstance()->Emit(
		particleName_, basePos_, positionDist_, scaleDist_,
		velocityDist_, baseColor_, colorDist_, lifeTime_, particleCount_);
}

//void ParticleEmitter::ImGuiUpdate() {
//	if (ImGui::Begin("Particle Emitter Editor")) {
//		ImGui::DragFloat3("Base Position", &basePos_.x, 0.1f);
//		ImGui::DragFloat3("Position  Min", &positionDist_.min.x, 0.1f);
//		ImGui::DragFloat3("Position  Max", &positionDist_.max.x, 0.1f);
//		ImGui::DragFloat3("Scale  Min", &scaleDist_.min.x, 0.1f);
//		ImGui::DragFloat3("Scale  Max", &scaleDist_.max.x, 0.1f);
//		ImGui::DragFloat3("Velocity  Min", &velocityDist_.min.x, 0.1f);
//		ImGui::DragFloat3("Velocity  Max", &velocityDist_.max.x, 0.1f);
//		ImGui::SeparatorText("color");
//		ImGui::ColorEdit4("Base Color", &baseColor_.x);
//		ImGui::ColorEdit4("Color  Min", &colorDist_.min.x);
//		ImGui::ColorEdit4("Color  Max", &colorDist_.max.x);
//		ImGui::DragFloat("Lifetime", &lifeTime_, 0.1f);
//		ImGui::DragInt("Particle Count", &particleCount_, 1, 1, 100);
//
//		/// セーブロード
//		globalParameter_->ParmSaveForImGui(particleName_.c_str());
//		ParmLoadForImGui();
//	}
//	ImGui::End();
//}

void ParticleEmitter::ParmLoadForImGui() {
	// ロードボタン
	if (ImGui::Button(std::format("Load {}", particleName_).c_str())) {
		globalParameter_->LoadFile(particleName_);
		// セーブ完了メッセージ
		ImGui::Text("Load Successful: %s", particleName_.c_str());
		ApplyGlobalParameter();
	}
}

void ParticleEmitter::AddParmGroup() {

	globalParameter_ = GlobalParameter::GetInstance();
	const char* groupName = particleName_.c_str();

	// グループを追加
	globalParameter_->CreateGroup(groupName, true);

	// Position ツリーノード
	globalParameter_->AddSeparatorText("Position");
	globalParameter_->AddItem(groupName, "Position Base", basePos_, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(groupName, "Position Max", positionDist_.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(groupName, "Position Min", positionDist_.min, GlobalParameter::WidgetType::DragFloat3);

	// Scale ツリーノード
	globalParameter_->AddSeparatorText("Scale");
	globalParameter_->AddItem(groupName, "Scale Max", scaleDist_.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(groupName, "Scale Min", scaleDist_.min, GlobalParameter::WidgetType::DragFloat3);

	// Velocity ツリーノード
	globalParameter_->AddSeparatorText("Velocity");
	globalParameter_->AddItem(groupName, "Velocity Max", velocityDist_.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(groupName, "Velocity Min", velocityDist_.min, GlobalParameter::WidgetType::DragFloat3);

	// Color ツリーノード
	globalParameter_->AddSeparatorText("Color");
	globalParameter_->AddItem(groupName, "BaseColor", baseColor_, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->AddItem(groupName, "Color Max", colorDist_.max, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->AddItem(groupName, "Color Min", colorDist_.min, GlobalParameter::WidgetType::ColorEdit4);

	// その他のパラメータ
	globalParameter_->AddSeparatorText("etc");
	globalParameter_->AddItem(groupName, "LifeTime", lifeTime_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(groupName, "Particle Count", particleCount_, GlobalParameter::WidgetType::SliderInt);
}


///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void ParticleEmitter::ApplyGlobalParameter() {

	basePos_ = globalParameter_->GetValue<Vector3>(particleName_, "Position Base");
	positionDist_.min = globalParameter_->GetValue<Vector3>(particleName_, "Position Min");
	positionDist_.max = globalParameter_->GetValue<Vector3>(particleName_, "Position Max");
	scaleDist_.min = globalParameter_->GetValue<Vector3>(particleName_, "Scale Min");
	scaleDist_.max = globalParameter_->GetValue<Vector3>(particleName_, "Scale Max");
	velocityDist_.min = globalParameter_->GetValue<Vector3>(particleName_, "Velocity Min");
	velocityDist_.max = globalParameter_->GetValue<Vector3>(particleName_, "Velocity Max");
	baseColor_ = globalParameter_->GetValue<Vector4>(particleName_, "BaseColor");
	colorDist_.min = globalParameter_->GetValue<Vector4>(particleName_, "Color Min");
	colorDist_.max = globalParameter_->GetValue<Vector4>(particleName_, "Color Max");
	lifeTime_ = globalParameter_->GetValue<float>(particleName_, "LifeTime");
	particleCount_ = globalParameter_->GetValue<int32_t>(particleName_, "Particle Count");
}