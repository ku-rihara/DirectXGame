#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include"ParticleCommon.h"
#include"Frame/Frame.h"
#include"ParticleManager.h"


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

/// =================================================================================
/// 初期化
/// =================================================================================
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
	parameters_.isScalerScale = true;
	parameters_.isRotateforDirection = false;

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
	ApplyGlobalParameter(particleName_);

	groupParamaters_.billBordType = static_cast<WorldTransform::BillboardType>(preBillBordType_);
	groupParamaters_.isShot = preIsShot_;
	parameters_.scaleEaseParm.easeType = static_cast<EaseType>(parameters_.scaleEaseParm.easeTypeInt);
}

///=================================================================================
/// ロード
///=================================================================================
void ParticleEmitter::ParmLoadForImGui() {

	// ロードボタン
	if (ImGui::Button(std::format("Load {}", particleName_).c_str())) {

		globalParameter_->LoadFile(particleName_, folderName_);
		// セーブ完了メッセージ
		ImGui::Text("Load Successful: %s", (folderName_ + particleName_).c_str());
		ApplyGlobalParameter(particleName_);
	}
}

void ParticleEmitter::ParmSaveForImGui() {
	// 保存ボタン
	if (ImGui::Button(std::format("Save {}", particleName_).c_str())) {
		globalParameter_->SaveFile(particleName_, folderName_);
		// セーブ完了メッセージ
		std::string message = std::format("{}.json saved.", folderName_ + particleName_);
		MessageBoxA(nullptr, message.c_str(), "GlobalParameter", 0);

		ParticleManager::GetInstance()->SetAllParticleFile();
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
	globalParameter_->AddItem(particleName_, "Position Base", parameters_.emitPos);
	globalParameter_->AddItem(particleName_, "Position Max", parameters_.positionDist.max);
	globalParameter_->AddItem(particleName_, "Position Min", parameters_.positionDist.min);

	// Scale
	/*globalParameter_->AddSeparatorText("Scale");*/
	globalParameter_->AddItem(particleName_, "Scale Max", parameters_.scaleDist.max);
	globalParameter_->AddItem(particleName_, "Scale Min", parameters_.scaleDist.min);
	globalParameter_->AddItem(particleName_, "ScaleV3 Max", parameters_.scaleDistV3.max);
	globalParameter_->AddItem(particleName_, "ScaleV3 Min", parameters_.scaleDistV3.min);

	// Rotate
	/*globalParameter_->AddSeparatorText("Rotate");*/
	globalParameter_->AddItem(particleName_, "Rotate Base", parameters_.baseRotate);
	globalParameter_->AddItem(particleName_, "Rotate Max", parameters_.rotateDist.max);
	globalParameter_->AddItem(particleName_, "Rotate Min", parameters_.rotateDist.min);
	globalParameter_->AddItem(particleName_, "RotateSpeed Max", parameters_.rotateSpeedDist.max);
	globalParameter_->AddItem(particleName_, "RotateSpeed Min", parameters_.rotateSpeedDist.min);

	// Velocity
	/*globalParameter_->AddSeparatorText("Velocity");*/
	globalParameter_->AddItem(particleName_, "Speed Max", parameters_.speedDist.max);
	globalParameter_->AddItem(particleName_, "Speed Min", parameters_.speedDist.min);

	globalParameter_->AddItem(particleName_, "Direction Max", parameters_.directionDist.max);
	globalParameter_->AddItem(particleName_, "Direction Min", parameters_.directionDist.min);


	// Color
	/*globalParameter_->AddSeparatorText("Color");*/
	globalParameter_->AddItem(particleName_, "BaseColor", parameters_.baseColor);
	globalParameter_->AddItem(particleName_, "Color Max", parameters_.colorDist.max);
	globalParameter_->AddItem(particleName_, "Color Min", parameters_.colorDist.min);

	// その他
	/*globalParameter_->AddSeparatorText("Prm");*/
	globalParameter_->AddItem(particleName_, "IntervalTime", intervalTime_);
	globalParameter_->AddItem(particleName_, "Gravity", parameters_.gravity);
	globalParameter_->AddItem(particleName_, "LifeTime", parameters_.lifeTime);
	globalParameter_->AddItem(particleName_, "Particle Count", particleCount);

	///rail 
	globalParameter_->AddItem(particleName_, "isMoveForRail", isMoveForRail_);
	globalParameter_->AddItem(particleName_, "moveSpeed", moveSpeed_);

	/// frag
	globalParameter_->AddItem(particleName_, "isScalerScale", parameters_.isScalerScale);
	globalParameter_->AddItem(particleName_, "preBillBordType_", preBillBordType_);
	globalParameter_->AddItem(particleName_, "isRotateforDirection", parameters_.isRotateforDirection);
	globalParameter_->AddItem(particleName_, "isBillBord", groupParamaters_.isBillBord);
	globalParameter_->AddItem(particleName_, "AdaptRotateIsX", groupParamaters_.adaptRotate_.isX_);
	globalParameter_->AddItem(particleName_, "AdaptRotateIsY", groupParamaters_.adaptRotate_.isY_);
	globalParameter_->AddItem(particleName_, "AdaptRotateIsZ", groupParamaters_.adaptRotate_.isZ_);
	globalParameter_->AddItem(particleName_, "isShot", preIsShot_);
	globalParameter_->AddItem(particleName_, "isAlphaNoMove", groupParamaters_.isAlphaNoMove);

	//easeParm
	globalParameter_->AddItem(particleName_, "scaleEaseParm.isScaleEase", parameters_.scaleEaseParm.isScaleEase);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.maxTime", parameters_.scaleEaseParm.maxTime);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.easeTypeInt", parameters_.scaleEaseParm.easeTypeInt);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueF.max", parameters_.scaleEaseParm.endValueF.max);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueF.min", parameters_.scaleEaseParm.endValueF.min);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueV3.max", parameters_.scaleEaseParm.endValueV3.max);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueV3.min", parameters_.scaleEaseParm.endValueV3.min);
}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void ParticleEmitter::SetValues() {
	// Position
		//globalParameter_->AddSeparatorText("Position");
	globalParameter_->SetValue(particleName_, "Position Base", parameters_.emitPos);
	globalParameter_->SetValue(particleName_, "Position Max", parameters_.positionDist.max);
	globalParameter_->SetValue(particleName_, "Position Min", parameters_.positionDist.min);

	// Scale
	/*globalParameter_->AddSeparatorText("Scale");*/
	globalParameter_->SetValue(particleName_, "Scale Max", parameters_.scaleDist.max);
	globalParameter_->SetValue(particleName_, "Scale Min", parameters_.scaleDist.min);
	globalParameter_->SetValue(particleName_, "ScaleV3 Max", parameters_.scaleDistV3.max);
	globalParameter_->SetValue(particleName_, "ScaleV3 Min", parameters_.scaleDistV3.min);
	// Rotate
	/*globalParameter_->AddSeparatorText("Rotate");*/
	globalParameter_->SetValue(particleName_, "Rotate Base", parameters_.baseRotate);
	globalParameter_->SetValue(particleName_, "Rotate Max", parameters_.rotateDist.max);
	globalParameter_->SetValue(particleName_, "Rotate Min", parameters_.rotateDist.min);
	globalParameter_->SetValue(particleName_, "RotateSpeed Max", parameters_.rotateSpeedDist.max);
	globalParameter_->SetValue(particleName_, "RotateSpeed Min", parameters_.rotateSpeedDist.min);

	// Velocity
	/*globalParameter_->AddSeparatorText("Velocity");*/
	globalParameter_->SetValue(particleName_, "Speed Max", parameters_.speedDist.max);
	globalParameter_->SetValue(particleName_, "Speed Min", parameters_.speedDist.min);

	globalParameter_->SetValue(particleName_, "Direction Max", parameters_.directionDist.max);
	globalParameter_->SetValue(particleName_, "Direction Min", parameters_.directionDist.min);


	// Color
	/*globalParameter_->AddSeparatorText("Color");*/
	globalParameter_->SetValue(particleName_, "BaseColor", parameters_.baseColor);
	globalParameter_->SetValue(particleName_, "Color Max", parameters_.colorDist.max);
	globalParameter_->SetValue(particleName_, "Color Min", parameters_.colorDist.min);

	// その他
	/*globalParameter_->AddSeparatorText("Prm");*/
	globalParameter_->SetValue(particleName_, "IntervalTime", intervalTime_);
	globalParameter_->SetValue(particleName_, "Gravity", parameters_.gravity);
	globalParameter_->SetValue(particleName_, "LifeTime", parameters_.lifeTime);
	globalParameter_->SetValue(particleName_, "Particle Count", particleCount);

	///rail 
	globalParameter_->SetValue(particleName_, "isMoveForRail", isMoveForRail_);
	globalParameter_->SetValue(particleName_, "moveSpeed", moveSpeed_);

	/// frag
	globalParameter_->SetValue(particleName_, "isScalerScale", parameters_.isScalerScale);
	globalParameter_->SetValue(particleName_, "preBillBordType_", preBillBordType_);
	globalParameter_->SetValue(particleName_, "isRotateforDirection", parameters_.isRotateforDirection);
	globalParameter_->SetValue(particleName_, "isBillBord", groupParamaters_.isBillBord);
	globalParameter_->SetValue(particleName_, "AdaptRotateIsX", groupParamaters_.adaptRotate_.isX_);
	globalParameter_->SetValue(particleName_, "AdaptRotateIsY", groupParamaters_.adaptRotate_.isY_);
	globalParameter_->SetValue(particleName_, "AdaptRotateIsZ", groupParamaters_.adaptRotate_.isZ_);
	globalParameter_->SetValue(particleName_, "isShot", preIsShot_);
	globalParameter_->SetValue(particleName_, "isAlphaNoMove", groupParamaters_.isAlphaNoMove);

	//easeParm
	globalParameter_->SetValue(particleName_, "scaleEaseParm.isScaleEase", parameters_.scaleEaseParm.isScaleEase);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.maxTime", parameters_.scaleEaseParm.maxTime);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.easeTypeInt", parameters_.scaleEaseParm.easeTypeInt);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueF.max", parameters_.scaleEaseParm.endValueF.max);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueF.min", parameters_.scaleEaseParm.endValueF.min);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueV3.max", parameters_.scaleEaseParm.endValueV3.max);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueV3.min", parameters_.scaleEaseParm.endValueV3.min);
}


///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void ParticleEmitter::ApplyGlobalParameter(const std::string& particleName) {

	// Position
	parameters_.emitPos = globalParameter_->GetValue<Vector3>(particleName, "Position Base");
	parameters_.positionDist.min = globalParameter_->GetValue<Vector3>(particleName, "Position Min");
	parameters_.positionDist.max = globalParameter_->GetValue<Vector3>(particleName, "Position Max");

	// Scale
	parameters_.scaleDist.min = globalParameter_->GetValue<float>(particleName, "Scale Min");
	parameters_.scaleDist.max = globalParameter_->GetValue<float>(particleName, "Scale Max");
	parameters_.scaleDistV3.min = globalParameter_->GetValue<Vector3>(particleName, "ScaleV3 Min");
	parameters_.scaleDistV3.max = globalParameter_->GetValue<Vector3>(particleName, "ScaleV3 Max");

	// Rotate
	parameters_.baseRotate = globalParameter_->GetValue<Vector3>(particleName, "Rotate Base");
	parameters_.rotateDist.min = globalParameter_->GetValue<Vector3>(particleName, "Rotate Min");
	parameters_.rotateDist.max = globalParameter_->GetValue<Vector3>(particleName, "Rotate Max");

	// Rotate Speed
	parameters_.rotateSpeedDist.min = globalParameter_->GetValue<Vector3>(particleName, "RotateSpeed Min");
	parameters_.rotateSpeedDist.max = globalParameter_->GetValue<Vector3>(particleName, "RotateSpeed Max");


	// Velocity
	parameters_.speedDist.min = globalParameter_->GetValue<float>(particleName, "Speed Min");
	parameters_.speedDist.max = globalParameter_->GetValue<float>(particleName, "Speed Max");
	parameters_.directionDist.min = globalParameter_->GetValue<Vector3>(particleName, "Direction Min");
	parameters_.directionDist.max = globalParameter_->GetValue<Vector3>(particleName, "Direction Max");


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

	parameters_.isScalerScale = globalParameter_->GetValue<bool>(particleName, "isScalerScale");
	parameters_.isRotateforDirection = globalParameter_->GetValue<bool>(particleName, "isRotateforDirection");
	preBillBordType_ = globalParameter_->GetValue<int32_t>(particleName, "preBillBordType_");
	groupParamaters_.isBillBord = globalParameter_->GetValue<bool>(particleName, "isBillBord");
	groupParamaters_.adaptRotate_.isX_ = globalParameter_->GetValue<bool>(particleName, "AdaptRotateIsX");
	groupParamaters_.adaptRotate_.isY_ = globalParameter_->GetValue<bool>(particleName, "AdaptRotateIsY");
	groupParamaters_.adaptRotate_.isZ_ = globalParameter_->GetValue<bool>(particleName, "AdaptRotateIsZ");
	preIsShot_ = globalParameter_->GetValue<bool>(particleName, "isShot");
	groupParamaters_.isAlphaNoMove = globalParameter_->GetValue<bool>(particleName, "isAlphaNoMove");

	///easeParm
	parameters_.scaleEaseParm.isScaleEase= globalParameter_->GetValue<bool>(particleName, "scaleEaseParm.isScaleEase");
	parameters_.scaleEaseParm.maxTime = globalParameter_->GetValue<float>(particleName, "scaleEaseParm.maxTime");
	parameters_.scaleEaseParm.easeTypeInt = globalParameter_->GetValue<int32_t>(particleName, "scaleEaseParm.easeTypeInt");
	parameters_.scaleEaseParm.endValueF.max = globalParameter_->GetValue<float>(particleName, "scaleEaseParm.endValueF.max");
	parameters_.scaleEaseParm.endValueF.min = globalParameter_->GetValue<float>(particleName, "scaleEaseParm.endValueF.min");
	parameters_.scaleEaseParm.endValueV3.max = globalParameter_->GetValue<Vector3>(particleName, "scaleEaseParm.endValueV3.max");
	parameters_.scaleEaseParm.endValueV3.min = globalParameter_->GetValue<Vector3>(particleName, "scaleEaseParm.endValueV3.min");
}

///=================================================================================
///エミット
///=================================================================================
void ParticleEmitter::Emit() {

	//　発生座標のパターン切り替え
	if (isMoveForRail_) {
		parameters_.emitPos = railManager_->GetWorldTransform().GetWorldPos();
	} else {
		parameters_.emitPos = parameters_.emitPos;
	}


	currentTime_ += Frame::DeltaTime();// 時間加算

	if (currentTime_ >= intervalTime_ || groupParamaters_.isShot) {//　間隔ごとに発動

		ParticleManager::GetInstance()->Emit(
			particleName_, parameters_, groupParamaters_, particleCount);
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
	ImGui::PushID(particleName_.c_str());

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
		ImGui::DragFloat("Velocity Max", &parameters_.speedDist.max, 0.1f);
		ImGui::DragFloat("Velocity Min", &parameters_.speedDist.min, 0.1f);

		ImGui::SeparatorText("Direction Range:");
		ImGui::DragFloat3("Direction Max", &parameters_.directionDist.max.x, 0.01f,-1.0f,1.0f);
		ImGui::DragFloat3("Direction Min", &parameters_.directionDist.min.x, 0.01f, -1.0f, 1.0f);
	}

	// scale Parm
	ScaleParmEditor();

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
		ImGui::DragFloat("LifeTime", &parameters_.lifeTime, 0.01f);
		ImGui::SliderInt("Particle Count", &particleCount, 1, 100);
	}

	// チェックが有効なら、BillboardType の設定を表示
	if (ImGui::CollapsingHeader("BillBoard")) {

		// IsBillBoard のチェックボックス
		ImGui::Checkbox("IsBillBoard", &groupParamaters_.isBillBord);


		ImGui::SeparatorText("IsRotateAdapt");
		ImGui::Checkbox("IsX", &groupParamaters_.adaptRotate_.isX_);
		ImGui::Checkbox("IsY", &groupParamaters_.adaptRotate_.isY_);
		ImGui::Checkbox("IsZ", &groupParamaters_.adaptRotate_.isZ_);

		ImGui::SeparatorText("BillBordType");

		const char* items[] = {"XYZ","X", "Y", "Z"}; // ビルボードの種類
		// ビルボードの種類を選択するコンボボックス
		if (ImGui::Combo("Billboard Type", &preBillBordType_, items, IM_ARRAYSIZE(items))) {
			// 選択した値を反映
			groupParamaters_.billBordType = static_cast<WorldTransform::BillboardType>(preBillBordType_);
		}
	}

	// その他のパラメータ
	if (ImGui::CollapsingHeader("Frag")) {

		// IsRotateforDirection のチェックボックス
		ImGui::Checkbox("IsRotateforDirection", &parameters_.isRotateforDirection);
		ImGui::Checkbox("IsShot", &preIsShot_);
		ImGui::Checkbox("isAlphaNoMove", &groupParamaters_.isAlphaNoMove);
	}

	// パーティクル切り替え
	ParticleChange();



	ParmSaveForImGui();
	ParmLoadForImGui();

	ImGui::PopID();
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
	viewProjection;
#ifdef _DEBUG
	
	if (isMoveForRail_) {// レールに沿うエミット位置
		railManager_->Draw(viewProjection);

	} else {// レールに沿わないエミット位置
		obj3d_->Draw(emitBoxTransform_, viewProjection);
	}
#endif // _DEBUG
}




void ParticleEmitter::ScaleParmEditor() {
	// Scale
	if (ImGui::CollapsingHeader("Scale")) {
		ImGui::SeparatorText("Scale Mode");

		ImGui::Checkbox("IsScalerScale", &parameters_.isScalerScale);
		ImGui::Checkbox("IsEasingMode", &parameters_.scaleEaseParm.isScaleEase);

		if (parameters_.isScalerScale) {
			ImGui::SeparatorText("Scaler Range");
			ImGui::DragFloat("Scale Max", &parameters_.scaleDist.max, 0.1f);
			ImGui::DragFloat("Scale Min", &parameters_.scaleDist.min, 0.1f);
		
		} else {
			ImGui::SeparatorText("V3 Range");
			ImGui::DragFloat3("ScaleV3 Max", reinterpret_cast<float*>(&parameters_.scaleDistV3.max), 0.1f);
			ImGui::DragFloat3("ScaleV3 Min", reinterpret_cast<float*>(&parameters_.scaleDistV3.min), 0.1f);
		}

		//EaseParm
		if (parameters_.scaleEaseParm.isScaleEase) {
			
			if (parameters_.isScalerScale) {
				ImGui::SeparatorText("EaseRange Float");
				ImGui::DragFloat("EaseValue Min", &parameters_.scaleEaseParm.endValueF.min, 0.1f);
				ImGui::DragFloat("EaseValue Max", &parameters_.scaleEaseParm.endValueF.max, 0.1f);
			}

			else {
				ImGui::SeparatorText("EaseRange V3");
				ImGui::DragFloat3("EaseValueV3 Max", reinterpret_cast<float*>(&parameters_.scaleEaseParm.endValueV3.max), 0.1f);
				ImGui::DragFloat3("EaseValueV3 Min", reinterpret_cast<float*>(&parameters_.scaleEaseParm.endValueV3.min), 0.1f);
			}
			//イージングパラメータ
			ImGui::SeparatorText("Ease Paramater");
			ImGui::DragFloat("maxTime", &parameters_.scaleEaseParm.maxTime,0.01f);
			ImGui::SeparatorText("EaseType");
			// イージング種類
			const char* easeItems[] = { "InSine","OutSine", "OutBack","OutQuint"};
			// ビルボードの種類を選択するコンボボックス
			if (ImGui::Combo("Easing Type", &parameters_.scaleEaseParm.easeTypeInt, easeItems, IM_ARRAYSIZE(easeItems))) {
				// 選択した値を反映
				parameters_.scaleEaseParm.easeType = static_cast<EaseType>(parameters_.scaleEaseParm.easeTypeInt);
			}
		}

		
	}
}

///------------------------------------------------------------------------------------------------
/// パーティクル切り替え
///------------------------------------------------------------------------------------------------
void ParticleEmitter::ParticleChange() {
	static int selectedIndex = 0;  // 現在選択中のインデックス
	std::vector<std::string>filenames = ParticleManager::GetInstance()->GetParticleFiles();

	// リストに追加
	if (!filenames.empty()) {
		std::vector<const char*> names;
		for (const auto& file : filenames) {
			names.push_back(file.c_str());

			/*	if (file != particleName_) continue;*/

		}
		if (ImGui::CollapsingHeader("SelectParticle")) {
			// 選択
			ImGui::ListBox("Particles", &selectedIndex, names.data(), static_cast<int>(names.size()));

			// 適応
			if (ImGui::Button("Apply")) {
				ApplyGlobalParameter(filenames[selectedIndex]);

				globalParameter_->LoadFile(filenames[selectedIndex], folderName_);
				// セーブ完了メッセージ
				ImGui::Text("Load Successful: %s", (folderName_ + filenames[selectedIndex]).c_str());
				ApplyGlobalParameter(filenames[selectedIndex]);

			}
		}
		// NotFind
	} else {
		ImGui::Text("No particle files found.");
	}
}

/// <summary>
/// setter
/// </summary>
/// <param name="parent"></param>

void ParticleEmitter::SetParentBasePos(WorldTransform* parent) {
	emitBoxTransform_.parent_ = parent;
}

void  ParticleEmitter::SetBlendMode(const ParticleCommon::BlendMode& blendmode) {
	groupParamaters_.blendMode = blendmode;
}

void  ParticleEmitter::SetBillBordType(const WorldTransform::BillboardType& billboardType) {
	groupParamaters_.billBordType = billboardType;
}

void ParticleEmitter::SetParentTransform(const WorldTransform* transform) {
	parameters_.parentTransform = transform;
}