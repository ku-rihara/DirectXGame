#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include"ParticleCommon.h"
#include"Frame/Frame.h"
#include"base/SrvManager.h"
#include"MathFunction.h"

#include <imgui.h>
#include<format>

ParticleEmitter::ParticleEmitter() {}

///=================================================================================
///パーティクル作成
///=================================================================================
ParticleEmitter* ParticleEmitter::CreateParticle(const std::string& name, const std::string& modelFilePath,
	const std::string& extension, const int32_t& maxnum) {

	auto emitter = std::make_unique<ParticleEmitter>();
	emitter->particleName = name;
	ParticleManager::GetInstance()->CreateParticleGroup(emitter->particleName, modelFilePath, extension, maxnum);
	emitter->Init();
	return emitter.release();
}

///=================================================================================
///初期化
///=================================================================================
void ParticleEmitter::Init() {

	particleCount = 0;
	parameters_.lifeTime = 0.0f;
	parameters_.gravity = 0.0f;
	parameters_.baseColor = { 0, 0, 0, 0 };
	parameters_.colorDist.min = { 0, 0, 0, 0 };
	parameters_.colorDist.max = { 0, 0, 0, 0 };
	intervalTime_ = 1.0f;
	parameters_.targetPos = { 0,0,0 };
	groupParamaters_.isBillBord = true;
	parameters_.isRotateforDirection = false;

	// レールマネージャー
	railManager_ = std::make_unique<RailManager>();
	railManager_->Init(particleName);


	/// 発生位置可視化オブジェ
	obj3d_.reset(Object3d::CreateModel("DebugCube", ".obj"));
	emitBoxTransform_.Init();

	/// グローバル変数
	globalParameter_ = GlobalParameter::GetInstance();
	globalParameter_->CreateGroup(particleName, false);

	AddParmGroup();
	ApplyGlobalParameter();
}

///=================================================================================
/// ロード
///=================================================================================
void ParticleEmitter::ParmLoadForImGui() {

	// ロードボタン
	if (ImGui::Button(std::format("Load {}",particleName).c_str())) {

		globalParameter_->LoadFile(particleName);
		// セーブ完了メッセージ
		ImGui::Text("Load Successful: %s",particleName.c_str());
		ApplyGlobalParameter();
	}
}


///=================================================================================
/// テクスチャ切り替え
///=================================================================================
void ParticleEmitter::SetTextureHandle(const uint32_t& handle) {

	ParticleManager::GetInstance()->SetTextureHandle(particleName, handle);

}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void ParticleEmitter::AddParmGroup() {

	// Position
	//globalParameter_->AddSeparatorText("Position");
	globalParameter_->AddItem(particleName, "Position Base", parameters_.emitPos);
	globalParameter_->AddItem(particleName, "Position Max", parameters_.positionDist.max);
	globalParameter_->AddItem(particleName, "Position Min", parameters_.positionDist.min);

	// Scale
	/*globalParameter_->AddSeparatorText("Scale");*/
	globalParameter_->AddItem(particleName, "Scale Max", parameters_.scaleDist.max);
	globalParameter_->AddItem(particleName, "Scale Min", parameters_.scaleDist.min);

	// Rotate
	/*globalParameter_->AddSeparatorText("Rotate");*/
	globalParameter_->AddItem(particleName, "Rotate Base", parameters_.baseRotate);
	globalParameter_->AddItem(particleName, "Rotate Max", parameters_.rotateDist.max);
	globalParameter_->AddItem(particleName, "Rotate Min", parameters_.rotateDist.min);
	globalParameter_->AddItem(particleName, "RotateSpeed Max", parameters_.rotateSpeedDist.max);
	globalParameter_->AddItem(particleName, "RotateSpeed Min", parameters_.rotateSpeedDist.min);

	// Velocity
	/*globalParameter_->AddSeparatorText("Velocity");*/
	globalParameter_->AddItem(particleName, "Velocity Max", parameters_.velocityDist.max);
	globalParameter_->AddItem(particleName, "Velocity Min", parameters_.velocityDist.min);

	// Color
	/*globalParameter_->AddSeparatorText("Color");*/
	globalParameter_->AddItem(particleName, "BaseColor", parameters_.baseColor);
	globalParameter_->AddItem(particleName, "Color Max", parameters_.colorDist.max);
	globalParameter_->AddItem(particleName, "Color Min", parameters_.colorDist.min);

	// その他
	/*globalParameter_->AddSeparatorText("Prm");*/
	globalParameter_->AddItem(particleName, "IntervalTime", intervalTime_);
	globalParameter_->AddItem(particleName, "Gravity", parameters_.gravity);
	globalParameter_->AddItem(particleName, "LifeTime", parameters_.lifeTime);
	globalParameter_->AddItem(particleName, "Particle Count", particleCount);

	///rail 
	globalParameter_->AddItem(particleName, "isMoveForRail", isMoveForRail_);
	globalParameter_->AddItem(particleName, "moveSpeed", moveSpeed_);

}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void ParticleEmitter::SetValues() {
	// Position
		//globalParameter_->AddSeparatorText("Position");
	globalParameter_->SetValue(particleName, "Position Base", parameters_.emitPos);
	globalParameter_->SetValue(particleName, "Position Max", parameters_.positionDist.max);
	globalParameter_->SetValue(particleName, "Position Min", parameters_.positionDist.min);

	// Scale
	/*globalParameter_->AddSeparatorText("Scale");*/
	globalParameter_->SetValue(particleName, "Scale Max", parameters_.scaleDist.max);
	globalParameter_->SetValue(particleName, "Scale Min", parameters_.scaleDist.min);

	// Rotate
	/*globalParameter_->AddSeparatorText("Rotate");*/
	globalParameter_->SetValue(particleName, "Rotate Base", parameters_.baseRotate);
	globalParameter_->SetValue(particleName, "Rotate Max", parameters_.rotateDist.max);
	globalParameter_->SetValue(particleName, "Rotate Min", parameters_.rotateDist.min);
	globalParameter_->SetValue(particleName, "RotateSpeed Max", parameters_.rotateSpeedDist.max);
	globalParameter_->SetValue(particleName, "RotateSpeed Min", parameters_.rotateSpeedDist.min);

	// Velocity
	/*globalParameter_->AddSeparatorText("Velocity");*/
	globalParameter_->SetValue(particleName, "Velocity Max", parameters_.velocityDist.max);
	globalParameter_->SetValue(particleName, "Velocity Min", parameters_.velocityDist.min);

	// Color
	/*globalParameter_->AddSeparatorText("Color");*/
	globalParameter_->SetValue(particleName, "BaseColor", parameters_.baseColor);
	globalParameter_->SetValue(particleName, "Color Max", parameters_.colorDist.max);
	globalParameter_->SetValue(particleName, "Color Min", parameters_.colorDist.min);

	// その他
	/*globalParameter_->AddSeparatorText("Prm");*/
	globalParameter_->SetValue(particleName, "IntervalTime", intervalTime_);
	globalParameter_->SetValue(particleName, "Gravity", parameters_.gravity);
	globalParameter_->SetValue(particleName, "LifeTime", parameters_.lifeTime);
	globalParameter_->SetValue(particleName, "Particle Count", particleCount);

	///rail 
	globalParameter_->SetValue(particleName, "isMoveForRail", isMoveForRail_);
	globalParameter_->SetValue(particleName, "moveSpeed", moveSpeed_);

}


///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void ParticleEmitter::ApplyGlobalParameter() {

	// Position
	parameters_.emitPos = globalParameter_->GetValue<Vector3>(particleName, "Position Base");
	parameters_.positionDist.min = globalParameter_->GetValue<Vector3>(particleName, "Position Min");
	parameters_.positionDist.max = globalParameter_->GetValue<Vector3>(particleName, "Position Max");

	// Scale
	parameters_.scaleDist.min = globalParameter_->GetValue<float>(particleName, "Scale Min");
	parameters_.scaleDist.max = globalParameter_->GetValue<float>(particleName, "Scale Max");

	// Rotate
	parameters_.baseRotate = globalParameter_->GetValue<Vector3>(particleName, "Rotate Base");
	parameters_.rotateDist.min = globalParameter_->GetValue<Vector3>(particleName, "Rotate Min");
	parameters_.rotateDist.max = globalParameter_->GetValue<Vector3>(particleName, "Rotate Max");

	// Rotate Speed
	parameters_.rotateSpeedDist.min = globalParameter_->GetValue<Vector3>(particleName, "RotateSpeed Min");
	parameters_.rotateSpeedDist.max = globalParameter_->GetValue<Vector3>(particleName, "RotateSpeed Max");


	// Velocity
	parameters_.velocityDist.min = globalParameter_->GetValue<Vector3>(particleName, "Velocity Min");
	parameters_.velocityDist.max = globalParameter_->GetValue<Vector3>(particleName, "Velocity Max");

	// Color
	parameters_.baseColor = globalParameter_->GetValue<Vector4>(particleName, "BaseColor");
	parameters_.colorDist.min = globalParameter_->GetValue<Vector4>(particleName, "Color Min");
	parameters_.colorDist.max = globalParameter_->GetValue<Vector4>(particleName, "Color Max");

	// その他のパラメータ
	intervalTime_ = globalParameter_->GetValue<float>(particleName, "IntervalTime");
	parameters_.gravity = globalParameter_->GetValue<float>(particleName, "Gravity");
	parameters_.lifeTime = globalParameter_->GetValue<float>(particleName, "LifeTime");
	particleCount = globalParameter_->GetValue<int32_t>(particleName, "Particle Count");

	///rail	
	isMoveForRail_ = globalParameter_->GetValue<bool>(particleName, "isMoveForRail");
	moveSpeed_ = globalParameter_->GetValue<float>(particleName, "moveSpeed");
}

///=================================================================================
///エミット
///=================================================================================
void ParticleEmitter::Emit() {

	//　発生座標のパターン切り替え
	if (isMoveForRail_) {
		parameters_.emitPos = railManager_->GetWorldTransform().GetWorldPos();
	}
	else {
		parameters_.emitPos = parameters_.emitPos;
	}


	currentTime_ += Frame::DeltaTime();// 時間加算

	if (currentTime_ >= intervalTime_) {//　間隔ごとに発動

		ParticleManager::GetInstance()->Emit(
			particleName, parameters_,groupParamaters_,particleCount);
            currentTime_ = 0.0f;// 時間を戻す
	}
}


///=================================================================================
///ImGui更新
///=================================================================================
void ParticleEmitter::Update() {
	// レール更新
	railManager_->Update(moveSpeed_);

	UpdateEmitTransform();

	SetValues();
}

void ParticleEmitter::EditorUpdate() {
#ifdef _DEBUG

	ImGui::Begin(particleName.c_str());

	// Color
	if (ImGui::CollapsingHeader("Color")) {
		ImGui::SeparatorText("Base Color:");
		ImGui::ColorEdit4("Base", &parameters_.baseColor.x);

		ImGui::SeparatorText("Color Range:");
		ImGui::ColorEdit4("Max", &parameters_.colorDist.max.x);
		ImGui::ColorEdit4("Min", &parameters_.colorDist.min.x);
	}

	/// rail
	if (ImGui::CollapsingHeader("MoveForRail")) {
		ImGui::SeparatorText("Paramater");
		ImGui::Checkbox("isMoveForRail", &isMoveForRail_);
		ImGui::DragFloat("moveSpeed", &moveSpeed_, 0.001f);

		ImGui::SeparatorText("ControlPoints:");
		railManager_->ImGuiEdit();
	}

	// Position
	if (ImGui::CollapsingHeader("Position")) {
		ImGui::SeparatorText("Position Base:");
		ImGui::DragFloat3("Base", &parameters_.emitPos.x, 0.1f);

		ImGui::SeparatorText("Position Range:");
		ImGui::DragFloat3("Position Max", &parameters_.positionDist.max.x, 0.1f);
		ImGui::DragFloat3("Position Min", &parameters_.positionDist.min.x, 0.1f);
	}


	// Velocity
	if (ImGui::CollapsingHeader("Velocity")) {
		ImGui::SeparatorText("Velocity Range:");
		ImGui::DragFloat3("Velocity Max", &parameters_.velocityDist.max.x, 0.1f);
		ImGui::DragFloat3("Velocity Min", &parameters_.velocityDist.min.x, 0.1f);
	}

	// Scale
	if (ImGui::CollapsingHeader("Scale")) {
		ImGui::SeparatorText("Scale Range:");
		ImGui::DragFloat("Scale Max", &parameters_.scaleDist.max, 0.1f);
		ImGui::DragFloat("Scale Min", &parameters_.scaleDist.min, 0.1f);
	}

	// Rotate
	if (ImGui::CollapsingHeader("Rotate(Degree)")) {

		ImGui::DragFloat3("BaseRotate", &parameters_.baseRotate.x, 0.1f, 0, 360);
		ImGui::DragFloat3("Rotate Max", &parameters_.rotateDist.max.x, 0.1f, 0, 360);
		ImGui::DragFloat3("Rotate Min", &parameters_.rotateDist.min.x, 0.1f, 0, 360);

	}

	if (ImGui::CollapsingHeader("Rotate Speed(Degree)")) {
		ImGui::DragFloat3("Rotate Speed Max", &parameters_.rotateSpeedDist.max.x, 0.1f, 0, 720);
		ImGui::DragFloat3("Rotate Speed Min", &parameters_.rotateSpeedDist.min.x, 0.1f, 0, 720);
	}


	// その他のパラメータ
	if (ImGui::CollapsingHeader("etcParamater")) {
		ImGui::DragFloat("IntervalTime", &intervalTime_, 0.01f, 0.01f, 100.0f);
		ImGui::DragFloat("Gravity", &parameters_.gravity, 0.1f);
		ImGui::DragFloat("LifeTime", &parameters_.lifeTime, 0.1f);
		ImGui::SliderInt("Particle Count", &particleCount, 1, 100);
	}

	globalParameter_->ParmSaveForImGui(particleName);
	ParmLoadForImGui();

	ImGui::End();
#endif // _DEBUG
}

void ParticleEmitter::UpdateEmitTransform() {
	emitBoxTransform_.translation_ = parameters_.emitPos;
	// スケールを範囲の大きさで設定
	emitBoxTransform_.scale_ = {
		parameters_.positionDist.max.x - parameters_.positionDist.min.x,
		parameters_.positionDist.max.y - parameters_.positionDist.min.y,
		parameters_.positionDist.max.z - parameters_.positionDist.min.z
	};
	railManager_->SetScale(emitBoxTransform_.scale_);

	emitBoxTransform_.UpdateMatrix();
}


void ParticleEmitter::RailDraw(const ViewProjection& viewProjection) {
	viewProjection;
	/*railManager_->RailDraw(viewProjection);*/
}
void ParticleEmitter::DebugDraw(const ViewProjection& viewProjection) {
#ifdef _DEBUG

	if (isMoveForRail_) {// レールに沿うエミット位置
		railManager_->Draw(viewProjection);

	}
	else {// レールに沿わないエミット位置
		obj3d_->Draw(emitBoxTransform_, viewProjection);
	}
#endif // _DEBUG
}


void ParticleEmitter::SetParentBasePos(WorldTransform* parent) {
	emitBoxTransform_.parent_ = parent;
}

void  ParticleEmitter::SetBlendMode(const BlendMode& blendmode) {
	groupParamaters_.blendMode = blendmode;
}