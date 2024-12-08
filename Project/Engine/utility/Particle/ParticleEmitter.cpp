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
	const std::string& extension, const int32_t& maxnum,const bool& isFirst) {

	auto emitter = std::make_unique<ParticleEmitter>();
	emitter->particleName_ = name;
	ParticleManager::GetInstance()->CreateParticleGroup(emitter->particleName_, modelFilePath, extension, maxnum);
	emitter->Init(isFirst);
	return emitter.release();
}

///=================================================================================
///初期化
///=================================================================================
void ParticleEmitter::Init(const bool& isFirst) {

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

	/// 発生位置可視化オブジェ
	emitObj_.reset(Object3d::CreateModel("DebugCube", ".obj"));
	emitTransform_.Init();

	if (isFirst) {
		SetValues();
	}
	else {
		/// JSON関連
		AddParmGroup();
	}

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
/// テクスチャ切り替え
///=================================================================================
void ParticleEmitter::SetTextureHandle(const uint32_t& handle) {

	ParticleManager::GetInstance()->SetTextureHandle(particleName_, handle);
	
}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void ParticleEmitter::AddParmGroup() {

	globalParameter_ = GlobalParameter::GetInstance();
	//const char* groupName = particleName_.c_str();

	// グループを追加(GlobalParamaterで表示はしない)
	globalParameter_->CreateGroup(particleName_, false);

	// Position
	//globalParameter_->AddSeparatorText("Position");
	globalParameter_->AddItem(particleName_, "Position Base", basePos_, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "Position Max", positionDist_.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "Position Min", positionDist_.min, GlobalParameter::WidgetType::DragFloat3);

	// Scale
	/*globalParameter_->AddSeparatorText("Scale");*/
	globalParameter_->AddItem(particleName_, "Scale Max", scaleDist_.max, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "Scale Min", scaleDist_.min, GlobalParameter::WidgetType::DragFloat);

	// Rotate
	/*globalParameter_->AddSeparatorText("Rotate");*/
	globalParameter_->AddItem(particleName_, "Rotate Base", baseRotate_, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(particleName_, "Rotate Max", rotateDist_.max, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(particleName_, "Rotate Min", rotateDist_.min, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(particleName_, "RotateSpeed Base", baseRotateSpeed_, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(particleName_, "RotateSpeed Max", rotateSpeedDist_.max, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->AddItem(particleName_, "RotateSpeed Min", rotateSpeedDist_.min, GlobalParameter::WidgetType::SlideAngle);

	// Velocity
	/*globalParameter_->AddSeparatorText("Velocity");*/
	globalParameter_->AddItem(particleName_, "Velocity Max", velocityDist_.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "Velocity Min", velocityDist_.min, GlobalParameter::WidgetType::DragFloat3);

	// Color
	/*globalParameter_->AddSeparatorText("Color");*/
	globalParameter_->AddItem(particleName_, "BaseColor", baseColor_, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->AddItem(particleName_, "Color Max", colorDist_.max, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->AddItem(particleName_, "Color Min", colorDist_.min, GlobalParameter::WidgetType::ColorEdit4);

	// その他
	/*globalParameter_->AddSeparatorText("Prm");*/
	globalParameter_->AddItem(particleName_, "IntervalTime", intervalTime_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "Gravity", gravity_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "LifeTime", lifeTime_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "Particle Count", particleCount_, GlobalParameter::WidgetType::SliderInt);

	///rail 
	globalParameter_->AddItem(particleName_, "isMoveForRail", isMoveForRail_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->AddItem(particleName_, "moveSpeed", moveSpeed_, GlobalParameter::WidgetType::DragFloat);

}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void ParticleEmitter::SetValues() {

	globalParameter_ = GlobalParameter::GetInstance();
	//const char* groupName = particleName_.c_str();

	// グループを追加(GlobalParamaterで表示はしない)
	globalParameter_->CreateGroup(particleName_, false);

	// Position
	//globalParameter_->AddSeparatorText("Position");
	globalParameter_->SetValue(particleName_, "Position Base", basePos_, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "Position Max", positionDist_.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "Position Min", positionDist_.min, GlobalParameter::WidgetType::DragFloat3);

	// Scale
	/*globalParameter_->AddSeparatorText("Scale");*/
	globalParameter_->SetValue(particleName_, "Scale Max", scaleDist_.max, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "Scale Min", scaleDist_.min, GlobalParameter::WidgetType::DragFloat);

	// Rotate
	/*globalParameter_->AddSeparatorText("Rotate");*/
	globalParameter_->SetValue(particleName_, "Rotate Base", baseRotate_, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->SetValue(particleName_, "Rotate Max", rotateDist_.max, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->SetValue(particleName_, "Rotate Min", rotateDist_.min, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->SetValue(particleName_, "RotateSpeed Base", baseRotateSpeed_, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->SetValue(particleName_, "RotateSpeed Max", rotateSpeedDist_.max, GlobalParameter::WidgetType::SlideAngle);
	globalParameter_->SetValue(particleName_, "RotateSpeed Min", rotateSpeedDist_.min, GlobalParameter::WidgetType::SlideAngle);

	// Velocity
	/*globalParameter_->AddSeparatorText("Velocity");*/
	globalParameter_->SetValue(particleName_, "Velocity Max", velocityDist_.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "Velocity Min", velocityDist_.min, GlobalParameter::WidgetType::DragFloat3);

	// Color
	/*globalParameter_->AddSeparatorText("Color");*/
	globalParameter_->SetValue(particleName_, "BaseColor", baseColor_, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->SetValue(particleName_, "Color Max", colorDist_.max, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->SetValue(particleName_, "Color Min", colorDist_.min, GlobalParameter::WidgetType::ColorEdit4);

	// その他
	/*globalParameter_->AddSeparatorText("Prm");*/
	globalParameter_->SetValue(particleName_, "IntervalTime", intervalTime_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "Gravity", gravity_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "LifeTime", lifeTime_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "Particle Count", particleCount_, GlobalParameter::WidgetType::SliderInt);

	///rail 
	globalParameter_->SetValue(particleName_, "isMoveForRail", isMoveForRail_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->SetValue(particleName_, "moveSpeed", moveSpeed_, GlobalParameter::WidgetType::DragFloat);

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

	///rail
	
	isMoveForRail_ = globalParameter_->GetValue<bool>(particleName_, "isMoveForRail");
	moveSpeed_ = globalParameter_->GetValue<float>(particleName_, "moveSpeed");
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

	// レール更新
	railManager_->Update(emitControlPosManager_->GetPositions(), moveSpeed_);

	UpdateEmitTransform();

	SetValues();


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
		ImGui::DragFloat("IntervalTime", &intervalTime_, 0.01f,0.2f,100.0f);
		ImGui::DragFloat("Gravity", &gravity_, 0.1f);
		ImGui::DragFloat("LifeTime", &lifeTime_, 0.1f);
		ImGui::SliderInt("Particle Count", &particleCount_, 1, 100);
	}

	globalParameter_->ParmSaveForImGui(particleName_);
	ParmLoadForImGui();

	ImGui::End();
#endif // _DEBUG
}

void ParticleEmitter::UpdateEmitTransform() {
	emitTransform_.translation_ = basePos_;
	// スケールを範囲の大きさで設定
	emitTransform_.scale_ = {
		positionDist_.max.x - positionDist_.min.x,
		positionDist_.max.y - positionDist_.min.y,
		positionDist_.max.z - positionDist_.min.z
	};
	railManager_->SetScale(emitTransform_.scale_);

	emitTransform_.UpdateMatrix();
}


void ParticleEmitter::RailDraw(const ViewProjection& viewProjection) {
	viewProjection;
	/*railManager_->RailDraw(viewProjection);*/
}
void ParticleEmitter::PositionDraw(const ViewProjection& viewProjection) {

	if (isMoveForRail_) {// レールに沿うエミット位置
		railManager_->Draw(viewProjection);
		emitControlPosManager_->Draw(viewProjection);
	}
	else {// レールに沿わないエミット位置
		emitObj_->Draw(emitTransform_, viewProjection);
	}
}