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
	targetPos_ = { 0,0,0 };
	isBillBord_ = true;
	isRotateforDirection_ = false;

	// レールマネージャー
	railManager_ = std::make_unique<RailManager>();
	railManager_->Init(particleName_);


	/// 発生位置可視化オブジェ
	obj3d_.reset(Object3d::CreateModel("DebugCube", ".obj"));
	emitBoxTransform_.Init();

	/// グローバル変数
	globalParameter_ = GlobalParameter::GetInstance();
	globalParameter_->CreateGroup(particleName_, false);

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
/// テクスチャ切り替え
///=================================================================================
void ParticleEmitter::SetTextureHandle(const uint32_t& handle) {

	ParticleManager::GetInstance()->SetTextureHandle(particleName_, handle);
	
}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void ParticleEmitter::AddParmGroup() {

	// Position
	//globalParameter_->AddSeparatorText("Position");
	globalParameter_->AddItem(particleName_, "Position Base", basePos_);
	globalParameter_->AddItem(particleName_, "Position Max", positionDist_.max);
	globalParameter_->AddItem(particleName_, "Position Min", positionDist_.min);

	// Scale
	/*globalParameter_->AddSeparatorText("Scale");*/
	globalParameter_->AddItem(particleName_, "Scale Max", scaleDist_.max);
	globalParameter_->AddItem(particleName_, "Scale Min", scaleDist_.min);

	// Rotate
	/*globalParameter_->AddSeparatorText("Rotate");*/
	globalParameter_->AddItem(particleName_, "Rotate Base", baseRotate_);
	globalParameter_->AddItem(particleName_, "Rotate Max", rotateDist_.max);
	globalParameter_->AddItem(particleName_, "Rotate Min", rotateDist_.min);
	globalParameter_->AddItem(particleName_, "RotateSpeed Max", rotateSpeedDist_.max);
	globalParameter_->AddItem(particleName_, "RotateSpeed Min", rotateSpeedDist_.min);

	// Velocity
	/*globalParameter_->AddSeparatorText("Velocity");*/
	globalParameter_->AddItem(particleName_, "Velocity Max", velocityDist_.max);
	globalParameter_->AddItem(particleName_, "Velocity Min", velocityDist_.min);

	// Color
	/*globalParameter_->AddSeparatorText("Color");*/
	globalParameter_->AddItem(particleName_, "BaseColor", baseColor_);
	globalParameter_->AddItem(particleName_, "Color Max", colorDist_.max);
	globalParameter_->AddItem(particleName_, "Color Min", colorDist_.min);

	// その他
	/*globalParameter_->AddSeparatorText("Prm");*/
	globalParameter_->AddItem(particleName_, "IntervalTime", intervalTime_);
	globalParameter_->AddItem(particleName_, "Gravity", gravity_ );
	globalParameter_->AddItem(particleName_, "LifeTime", lifeTime_);
	globalParameter_->AddItem(particleName_, "Particle Count", particleCount_);

	///rail 
	globalParameter_->AddItem(particleName_, "isMoveForRail", isMoveForRail_);
	globalParameter_->AddItem(particleName_, "moveSpeed", moveSpeed_);

}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void ParticleEmitter::SetValues() {
	// Position
		//globalParameter_->AddSeparatorText("Position");
	globalParameter_->SetValue(particleName_, "Position Base", basePos_);
	globalParameter_->SetValue(particleName_, "Position Max", positionDist_.max);
	globalParameter_->SetValue(particleName_, "Position Min", positionDist_.min);

	// Scale
	/*globalParameter_->AddSeparatorText("Scale");*/
	globalParameter_->SetValue(particleName_, "Scale Max", scaleDist_.max);
	globalParameter_->SetValue(particleName_, "Scale Min", scaleDist_.min);

	// Rotate
	/*globalParameter_->AddSeparatorText("Rotate");*/
	globalParameter_->SetValue(particleName_, "Rotate Base", baseRotate_);
	globalParameter_->SetValue(particleName_, "Rotate Max", rotateDist_.max);
	globalParameter_->SetValue(particleName_, "Rotate Min", rotateDist_.min);
	globalParameter_->SetValue(particleName_, "RotateSpeed Max", rotateSpeedDist_.max);
	globalParameter_->SetValue(particleName_, "RotateSpeed Min", rotateSpeedDist_.min);

	// Velocity
	/*globalParameter_->AddSeparatorText("Velocity");*/
	globalParameter_->SetValue(particleName_, "Velocity Max", velocityDist_.max);
	globalParameter_->SetValue(particleName_, "Velocity Min", velocityDist_.min);

	// Color
	/*globalParameter_->AddSeparatorText("Color");*/
	globalParameter_->SetValue(particleName_, "BaseColor", baseColor_);
	globalParameter_->SetValue(particleName_, "Color Max", colorDist_.max);
	globalParameter_->SetValue(particleName_, "Color Min", colorDist_.min);

	// その他
	/*globalParameter_->AddSeparatorText("Prm");*/
	globalParameter_->SetValue(particleName_, "IntervalTime", intervalTime_);
	globalParameter_->SetValue(particleName_, "Gravity", gravity_);
	globalParameter_->SetValue(particleName_, "LifeTime", lifeTime_);
	globalParameter_->SetValue(particleName_, "Particle Count", particleCount_);

	///rail 
	globalParameter_->SetValue(particleName_, "isMoveForRail", isMoveForRail_);
	globalParameter_->SetValue(particleName_, "moveSpeed", moveSpeed_);

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
		emitPos_ = railManager_->GetWorldTransform().GetWorldPos();
	}
	else {
		emitPos_ = basePos_;
	}

	currentTime_ += Frame::DeltaTimeRate();// 時間加算

	if (currentTime_ >= intervalTime_) {//　間隔ごとに発動

		ParticleManager::GetInstance()->Emit(
			particleName_, targetPos_+emitPos_, positionDist_, scaleDist_,
			velocityDist_, baseColor_, colorDist_, lifeTime_, gravity_, toRadian(baseRotate_),
			 V3MinMax(toRadian(rotateDist_.min), toRadian(rotateDist_.max)), V3MinMax(toRadian(rotateSpeedDist_.min), toRadian(rotateSpeedDist_.max))
			, particleCount_,isBillBord_,isRotateforDirection_, blendMode_);

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
		ImGui::Checkbox("isMoveForRail", &isMoveForRail_);
		ImGui::DragFloat("moveSpeed", &moveSpeed_, 0.001f);

		ImGui::SeparatorText("ControlPoints:");
		railManager_->ImGuiEdit();
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
	if (ImGui::CollapsingHeader("Rotate(Degree)")) {

		ImGui::DragFloat3("BaseRotate", &baseRotate_.x,0.1f, 0, 360);
		ImGui::DragFloat3("Rotate Max", &rotateDist_.max.x, 0.1f, 0, 360);
		ImGui::DragFloat3("Rotate Min", &rotateDist_.min.z, 0.1f, 0, 360);

	}
	
	if (ImGui::CollapsingHeader("Rotate Speed(Degree)")) {
		ImGui::DragFloat3("Rotate Speed Max", &rotateSpeedDist_.max.x, 0.1f, 0, 720);
		ImGui::DragFloat3("Rotate Speed Min", &rotateSpeedDist_.min.x, 0.1f, 0, 720);
	}
	

	// その他のパラメータ
	if (ImGui::CollapsingHeader("etcParamater")) {
		ImGui::DragFloat("IntervalTime", &intervalTime_, 0.01f, 0.01f, 100.0f);
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
	emitBoxTransform_.translation_ = basePos_;
	// スケールを範囲の大きさで設定
	emitBoxTransform_.scale_ = {
		positionDist_.max.x - positionDist_.min.x,
		positionDist_.max.y - positionDist_.min.y,
		positionDist_.max.z - positionDist_.min.z
	};
	railManager_->SetScale(emitBoxTransform_.scale_);

	emitBoxTransform_.UpdateMatrix();
}


void ParticleEmitter::RailDraw(const ViewProjection& viewProjection) {
	viewProjection;
	/*railManager_->RailDraw(viewProjection);*/
}
void ParticleEmitter::DebugDraw(const ViewProjection& viewProjection) {
	viewProjection;
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
	blendMode_ = blendmode;
}