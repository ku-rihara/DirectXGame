#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include <imgui.h>


ParticleEmitter::ParticleEmitter() {}

///=================================================================================
///パーティクル作成
///=================================================================================
ParticleEmitter* ParticleEmitter::CreateParticle(const std::string& name, const std::string& modelFilePath,
	const std::string& extension, const int32_t& maxnum) {

	auto emitter = std::make_unique<ParticleEmitter>();
	emitter->particleName_ = name;
	ParticleManager::GetInstance()->CreateParticleGroup(emitter->particleName_, modelFilePath, extension, maxnum);
	emitter->Init();
	return emitter.release();
}

///=================================================================================
///初期化
///=================================================================================
void ParticleEmitter::Init() {
	particleCount_ = 0;
	lifeTime_ = 0.0f;
	gravity_ = 0.0f;
	baseColor_ = { 0, 0, 0, 0 };
	colorDist_.min = { 0, 0, 0, 0 };
	colorDist_.max = { 0, 0, 0, 0 };

	////JSON関連
	AddParmGroup();
	ApplyGlobalParameter();
}

///=================================================================================
///パラメータをグループに追加
///=================================================================================
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
	globalParameter_->AddItem(groupName, "Scale Max", scaleDist_.max, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(groupName, "Scale Min", scaleDist_.min, GlobalParameter::WidgetType::DragFloat);

	// Rotate ツリーノード
	globalParameter_->AddSeparatorText("Rotate");
	globalParameter_->AddItem(groupName, "Rotate Base", baseRotate_, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(groupName, "Rotate Max", rotateDist_.max, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(groupName, "Rotate Min", rotateDist_.min, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(groupName, "RotateSpeed Base", baseRotateSpeed_, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(groupName, "RotateSpeed Max", rotateSpeedDist_.max, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(groupName, "RotateSpeed Min", rotateSpeedDist_.min, GlobalParameter::WidgetType::SlideAngle);

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
	globalParameter_->AddSeparatorText("Prm");
	globalParameter_->AddItem(groupName, "Gravity", gravity_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(groupName, "LifeTime", lifeTime_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(groupName, "Particle Count", particleCount_, GlobalParameter::WidgetType::SliderInt);
}

///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void ParticleEmitter::ApplyGlobalParameter() {

	// Position
	basePos_ = globalParameter_->GetValue<Vector3>(particleName_,"Position Base");
	positionDist_.min = globalParameter_->GetValue<Vector3>(particleName_, "Position Min");
	positionDist_.max = globalParameter_->GetValue<Vector3>(particleName_, "Position Max");

	// Scale
	scaleDist_.min = globalParameter_->GetValue<float>(particleName_, "Scale Min");
	scaleDist_.max = globalParameter_->GetValue<float>(particleName_, "Scale Max");

	// Rotate
	baseRotate_ = globalParameter_->GetValue<Vector3>(particleName_, "Rotate Base");
	rotateDist_.min = globalParameter_->GetValue<Vector3>(particleName_, "Rotate Min");
	rotateDist_.max = globalParameter_->GetValue<Vector3>(particleName_, "Rotate Max");

	// Rotate Speed
	baseRotateSpeed_ = globalParameter_->GetValue<Vector3>(particleName_, "RotateSpeed Base");
	rotateSpeedDist_.min = globalParameter_->GetValue<Vector3>(particleName_, "RotateSpeed Min");
	rotateSpeedDist_.max = globalParameter_->GetValue<Vector3>(particleName_, "RotateSpeed Max");

	// Velocity
	velocityDist_.min = globalParameter_->GetValue<Vector3>(particleName_, "Velocity Min");
	velocityDist_.max = globalParameter_->GetValue<Vector3>(particleName_, "Velocity Max");

	// Color
	baseColor_ = globalParameter_->GetValue<Vector4>(particleName_, "BaseColor");
	colorDist_.min = globalParameter_->GetValue<Vector4>(particleName_, "Color Min");
	colorDist_.max = globalParameter_->GetValue<Vector4>(particleName_, "Color Max");

	// その他のパラメータ
	gravity_ = globalParameter_->GetValue<float>(particleName_, "Gravity");
	lifeTime_ = globalParameter_->GetValue<float>(particleName_, "LifeTime");
	particleCount_ = globalParameter_->GetValue<int32_t>(particleName_, "Particle Count");
}

///=================================================================================
///エミット
///=================================================================================
void ParticleEmitter::Emit() {
	ParticleManager::GetInstance()->Emit(
		particleName_, basePos_, positionDist_, scaleDist_,
		velocityDist_, baseColor_, colorDist_, lifeTime_,gravity_,baseRotate_,
		baseRotateSpeed_,rotateDist_,rotateSpeedDist_, particleCount_);
}

