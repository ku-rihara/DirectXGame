#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include"Frame/Frame.h"
#include"base/SrvManager.h"

#include <imgui.h>
#include<format>

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
	intervalTime_ = 1.0f;

	// レールマネージャー
	railManager_ = std::make_unique<EmitRailManager>();
	railManager_->Init(SrvManager::GetInstance());

	/// 制御点マネージャー
	emitControlPosManager_ = std::make_unique<EmitControlPosManager>();
	emitControlPosManager_->LoadFromFile(particleName_);

	/// JSON関連
	AddParmGroup();
	ApplyGlobalParameter();
}

///=================================================================================
/// ロード
///=================================================================================
void ParticleEmitter::ParmLoadForImGui() {

	// ロードボタン
	if (ImGui::Button(std::format("Load {}", particleName_).c_str())) {

		globalParameter_->LoadFile(particleName_);
		// セーブ完了メッセージ
		ImGui::Text("Load Successful: %s", particleName_.c_str());
		ApplyGlobalParameter();
	}
}

///=================================================================================
///パラメータをグループに追加
///=================================================================================
void ParticleEmitter::AddParmGroup() {

	globalParameter_ = GlobalParameter::GetInstance();
	const char* groupName = particleName_.c_str();

	// グループを追加(GlobalParamaterで表示はしない)
	globalParameter_->CreateGroup(groupName, false);

	// Position
	//globalParameter_->AddSeparatorText("Position");
	globalParameter_->AddItem(groupName, "Position Base", basePos_, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(groupName, "Position Max", positionDist_.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(groupName, "Position Min", positionDist_.min, GlobalParameter::WidgetType::DragFloat3);

	// Scale
	/*globalParameter_->AddSeparatorText("Scale");*/
	globalParameter_->AddItem(groupName, "Scale Max", scaleDist_.max, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(groupName, "Scale Min", scaleDist_.min, GlobalParameter::WidgetType::DragFloat);

	// Rotate
	/*globalParameter_->AddSeparatorText("Rotate");*/
	globalParameter_->AddItem(groupName, "Rotate Base", baseRotate_, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(groupName, "Rotate Max", rotateDist_.max, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(groupName, "Rotate Min", rotateDist_.min, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(groupName, "RotateSpeed Base", baseRotateSpeed_, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(groupName, "RotateSpeed Max", rotateSpeedDist_.max, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(groupName, "RotateSpeed Min", rotateSpeedDist_.min, GlobalParameter::WidgetType::SlideAngle);

	///rail 
	globalParameter_->AddItem(groupName, "isMoveForRail",isMoveForRail_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->AddItem(groupName, "moveSpeed", moveSpeed_, GlobalParameter::WidgetType::DragFloat);

	// Velocity
	/*globalParameter_->AddSeparatorText("Velocity");*/
	globalParameter_->AddItem(groupName, "Velocity Max", velocityDist_.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(groupName, "Velocity Min", velocityDist_.min, GlobalParameter::WidgetType::DragFloat3);

	// Color
	/*globalParameter_->AddSeparatorText("Color");*/
	globalParameter_->AddItem(groupName, "BaseColor", baseColor_, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->AddItem(groupName, "Color Max", colorDist_.max, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->AddItem(groupName, "Color Min", colorDist_.min, GlobalParameter::WidgetType::ColorEdit4);

	// その他
	/*globalParameter_->AddSeparatorText("Prm");*/
	globalParameter_->AddItem(groupName, "IntervalTime", intervalTime_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(groupName, "Gravity", gravity_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(groupName, "LifeTime", lifeTime_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(groupName, "Particle Count", particleCount_, GlobalParameter::WidgetType::SliderInt);
}

///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void ParticleEmitter::ApplyGlobalParameter() {

	// Position
	basePos_ = globalParameter_->GetValue<Vector3>(particleName_, "Position Base");
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

	///rail
	moveSpeed_ = globalParameter_->GetValue<float>(particleName_, "moveSpeed");
	isMoveForRail_ = globalParameter_->GetValue<bool>(particleName_, "isMoveForRail");


	// Velocity
	velocityDist_.min = globalParameter_->GetValue<Vector3>(particleName_, "Velocity Min");
	velocityDist_.max = globalParameter_->GetValue<Vector3>(particleName_, "Velocity Max");

	// Color
	baseColor_ = globalParameter_->GetValue<Vector4>(particleName_, "BaseColor");
	colorDist_.min = globalParameter_->GetValue<Vector4>(particleName_, "Color Min");
	colorDist_.max = globalParameter_->GetValue<Vector4>(particleName_, "Color Max");

	// その他のパラメータ
	intervalTime_ = globalParameter_->GetValue<float>(particleName_, "IntervalTime");
	gravity_ = globalParameter_->GetValue<float>(particleName_, "Gravity");
	lifeTime_ = globalParameter_->GetValue<float>(particleName_, "LifeTime");
	particleCount_ = globalParameter_->GetValue<int32_t>(particleName_, "Particle Count");

}

///=================================================================================
///エミット
///=================================================================================
void ParticleEmitter::Emit() {

	//　発生座標のパターン切り替え
	if (isMoveForRail_) {
		emitPos_ = railManager_->GetWorldPos();
	}
	else {
		emitPos_ = basePos_;
	}


	currentTime_ += Frame::DeltaTime();// 時間加算

	if (currentTime_ >= intervalTime_) {//　間隔ごとに発動

		ParticleManager::GetInstance()->Emit(
			particleName_, emitPos_, positionDist_, scaleDist_,
			velocityDist_, baseColor_, colorDist_, lifeTime_, gravity_, baseRotate_,
			baseRotateSpeed_, rotateDist_, rotateSpeedDist_, particleCount_);

		currentTime_ = 0.0f;// 時間を戻す
	}
}


///=================================================================================
///ImGui更新
///=================================================================================
void ParticleEmitter::EditorUpdate() {
#ifdef _DEBUG


	/*railManager_->Update(emitControlPosManager_->GetPositions(), moveSpeed_);*/

	ImGui::Begin(particleName_.c_str());

	// Color
	if (ImGui::CollapsingHeader("Color")) {
		ImGui::SeparatorText("Base Color:");
		ImGui::ColorEdit4("Base", &baseColor_.x);

		ImGui::SeparatorText("Color Range:");
		ImGui::ColorEdit4("Max", &colorDist_.max.x);
		ImGui::ColorEdit4("Min", &colorDist_.min.x);
	}

	/// rail
	if (ImGui::CollapsingHeader("MoveForRail")) {
		ImGui::SeparatorText("Paramater");
		ImGui::Checkbox("isMoveForRail",&isMoveForRail_);
		ImGui::DragFloat("moveSpeed", &moveSpeed_,0.001f);

		ImGui::SeparatorText("ControlPoints:");
		emitControlPosManager_->ImGuiUpdate(particleName_);
	}

	// Position
	if (ImGui::CollapsingHeader("Position")) {
		ImGui::SeparatorText("Position Base:");
		ImGui::DragFloat3("Base", &basePos_.x, 0.1f);

		ImGui::SeparatorText("Position Range:");
		ImGui::DragFloat3("Position Max", &positionDist_.max.x, 0.1f);
		ImGui::DragFloat3("Position Min", &positionDist_.min.x, 0.1f);
	}


	// Velocity
	if (ImGui::CollapsingHeader("Velocity")) {
		ImGui::SeparatorText("Velocity Range:");
		ImGui::DragFloat3("Velocity Max", &velocityDist_.max.x, 0.1f);
		ImGui::DragFloat3("Velocity Min", &velocityDist_.min.x, 0.1f);
	}

	// Scale
	if (ImGui::CollapsingHeader("Scale")) {
		ImGui::SeparatorText("Scale Range:");
		ImGui::DragFloat("Scale Max", &scaleDist_.max, 0.1f);
		ImGui::DragFloat("Scale Min", &scaleDist_.min, 0.1f);
	}

	// Rotate
	if (ImGui::CollapsingHeader("Rotate")) {
		ImGui::SeparatorText("Base Rotation:");
		if (ImGui::CollapsingHeader("Base Rotate (Angle)")) {
			ImGui::SliderAngle("X", &baseRotate_.x, 0, 360);
			ImGui::SliderAngle("Y", &baseRotate_.y, 0, 360);
			ImGui::SliderAngle("Z", &baseRotate_.z, 0, 360);

		}

		ImGui::SeparatorText("Rotation Range:");
		if (ImGui::CollapsingHeader("Rotate Dist (Angle)")) {
			ImGui::SliderAngle("Max X", &rotateDist_.max.x, 0, 360);
			ImGui::SliderAngle("Max Y", &rotateDist_.max.y, 0, 360);
			ImGui::SliderAngle("Max Z", &rotateDist_.max.z, 0, 360);
			ImGui::SliderAngle("Min X", &rotateDist_.min.x, 0, 360);
			ImGui::SliderAngle("Min Y", &rotateDist_.min.y, 0, 360);
			ImGui::SliderAngle("Min Z", &rotateDist_.min.z, 0, 360);

		}

		ImGui::SeparatorText("Rotation Speed:");
		if (ImGui::CollapsingHeader("Rotate Speed (Angle)")) {
			ImGui::SliderAngle("Base", &baseRotateSpeed_.x, 0, 360);
			ImGui::SliderAngle("Max", &rotateSpeedDist_.max.x, 0, 360);
			ImGui::SliderAngle("Min", &rotateSpeedDist_.min.x, 0, 360);

		}
	}

	// その他のパラメータ
	if (ImGui::CollapsingHeader("etcParamater")) {
		ImGui::DragFloat("IntervalTime", &intervalTime_, 0.1f);
		ImGui::DragFloat("Gravity", &gravity_, 0.1f);
		ImGui::DragFloat("LifeTime", &lifeTime_, 0.1f);
		ImGui::SliderInt("Particle Count", &particleCount_, 1, 100);
	}

	globalParameter_->ParmSaveForImGui(particleName_);
	ParmLoadForImGui();

	ImGui::End();
#endif // _DEBUG
}


void ParticleEmitter::RailDraw(const ViewProjection& viewProjection) {
	railManager_->RailDraw(viewProjection);
}
void ParticleEmitter::PositionDraw(const ViewProjection& viewProjection) {
	railManager_->Draw(viewProjection);
}