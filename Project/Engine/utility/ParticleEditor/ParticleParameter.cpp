// ParticleParameter.cpp

#include "ParticleParameter.h"
#include <imgui.h>
#include <format>
#include "Frame/Frame.h"
#include "ParticleManager.h"
#include"base/TextureManager.h"




void ParticleParameter::ResetParameters() {

	particleCount = 0;
	parameters_.lifeTime = 0.0f;
	parameters_.gravity = 0.0f;
	parameters_.baseColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	parameters_.colorDist.min = { 0.0f, 0.0f, 0.0f, 0.0f };
	parameters_.colorDist.max = { 0.0f, 0.0f, 0.0f, 0.0f };
	intervalTime_ = 1.0f;
	parameters_.targetPos = { 0.0f, 0.0f, 0.0f };
	groupParamaters_.isBillBord = true;
	parameters_.isScalerScale = true;
	parameters_.isRotateforDirection = false;
	selectedTexturePath_ = ""; // 初期化時にテクスチャパスをリセット
}

void ParticleParameter::Init() {

	ResetParameters();

	/// グローバル変数
	globalParameter_ = GlobalParameter::GetInstance();
	globalParameter_->CreateGroup(particleName_, false);

	AddParmGroup();
	ApplyGlobalParameter(particleName_);

	//Editor上で設定したテクスチャを適応

	///ビルボード、単発フラグ、イージングタイプの設定
	groupParamaters_.billBordType = static_cast<WorldTransform::BillboardType>(preBillBordType_);
	groupParamaters_.isShot = preIsShot_;
	parameters_.scaleEaseParm.easeType = static_cast<EaseType>(parameters_.scaleEaseParm.easeTypeInt);
}

void ParticleParameter::ParmLoadForImGui() {
	// ロードボタン
	if (ImGui::Button(std::format("Load {}", particleName_).c_str())) {
		globalParameter_->LoadFile(particleName_, folderName_);
		// セーブ完了メッセージ
		ImGui::Text("Load Successful: %s", (folderName_ + particleName_).c_str());
		ApplyGlobalParameter(particleName_);
	}
}

void ParticleParameter::ParmSaveForImGui() {
	// 保存ボタン
	if (ImGui::Button(std::format("Save {}", particleName_).c_str())) {
		globalParameter_->SaveFile(particleName_, folderName_);
		// セーブ完了メッセージ
		std::string message = std::format("{}.json saved.", folderName_ + particleName_);
		MessageBoxA(nullptr, message.c_str(), "GlobalParameter", 0);
	}
}

void ParticleParameter::AddParmGroup() {
	// Position
	globalParameter_->AddItem(particleName_, "Position Base", parameters_.emitPos, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "Position Max", parameters_.positionDist.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "Position Min", parameters_.positionDist.min, GlobalParameter::WidgetType::DragFloat3);

	// Scale
	globalParameter_->AddItem(particleName_, "Scale Max", parameters_.scaleDist.max, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "Scale Min", parameters_.scaleDist.min, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "ScaleV3 Max", parameters_.scaleDistV3.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "ScaleV3 Min", parameters_.scaleDistV3.min, GlobalParameter::WidgetType::DragFloat3);

	// Rotate
	globalParameter_->AddItem(particleName_, "Rotate Base", parameters_.baseRotate, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "Rotate Max", parameters_.rotateDist.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "Rotate Min", parameters_.rotateDist.min, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "RotateSpeed Max", parameters_.rotateSpeedDist.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "RotateSpeed Min", parameters_.rotateSpeedDist.min, GlobalParameter::WidgetType::DragFloat3);

	// Velocity
	globalParameter_->AddItem(particleName_, "Speed Max", parameters_.speedDist.max, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "Speed Min", parameters_.speedDist.min, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "Direction Max", parameters_.directionDist.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "Direction Min", parameters_.directionDist.min, GlobalParameter::WidgetType::DragFloat3);

	// Color
	globalParameter_->AddItem(particleName_, "BaseColor", parameters_.baseColor, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->AddItem(particleName_, "Color Max", parameters_.colorDist.max, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->AddItem(particleName_, "Color Min", parameters_.colorDist.min, GlobalParameter::WidgetType::ColorEdit4);

	// その他
	globalParameter_->AddItem(particleName_, "IntervalTime", intervalTime_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "Gravity", parameters_.gravity, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "LifeTime", parameters_.lifeTime, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "Particle Count", particleCount, GlobalParameter::WidgetType::SliderInt);

	/// frag
	globalParameter_->AddItem(particleName_, "isScalerScale", parameters_.isScalerScale, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->AddItem(particleName_, "preBillBordType_", preBillBordType_, GlobalParameter::WidgetType::SliderInt);
	globalParameter_->AddItem(particleName_, "isRotateforDirection", parameters_.isRotateforDirection, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->AddItem(particleName_, "isBillBord", groupParamaters_.isBillBord, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->AddItem(particleName_, "AdaptRotateIsX", groupParamaters_.adaptRotate_.isX_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->AddItem(particleName_, "AdaptRotateIsY", groupParamaters_.adaptRotate_.isY_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->AddItem(particleName_, "AdaptRotateIsZ", groupParamaters_.adaptRotate_.isZ_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->AddItem(particleName_, "isShot", preIsShot_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->AddItem(particleName_, "isAlphaNoMove", groupParamaters_.isAlphaNoMove, GlobalParameter::WidgetType::Checkbox);

	//easeParm
	globalParameter_->AddItem(particleName_, "scaleEaseParm.isScaleEase", parameters_.scaleEaseParm.isScaleEase, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.maxTime", parameters_.scaleEaseParm.maxTime, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.easeTypeInt", parameters_.scaleEaseParm.easeTypeInt, GlobalParameter::WidgetType::SliderInt);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueF.max", parameters_.scaleEaseParm.endValueF.max, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueF.min", parameters_.scaleEaseParm.endValueF.min, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueV3.max", parameters_.scaleEaseParm.endValueV3.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueV3.min", parameters_.scaleEaseParm.endValueV3.min, GlobalParameter::WidgetType::DragFloat3);

	// テクスチャ
	globalParameter_->AddItem(particleName_, "selectedTexturePath_", selectedTexturePath_, GlobalParameter::WidgetType::NONE);
}

void ParticleParameter::SetValues() {
	// Position
	globalParameter_->SetValue(particleName_, "Position Base", parameters_.emitPos, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "Position Max", parameters_.positionDist.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "Position Min", parameters_.positionDist.min, GlobalParameter::WidgetType::DragFloat3);

	// Scale
	globalParameter_->SetValue(particleName_, "Scale Max", parameters_.scaleDist.max, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "Scale Min", parameters_.scaleDist.min, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "ScaleV3 Max", parameters_.scaleDistV3.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "ScaleV3 Min", parameters_.scaleDistV3.min, GlobalParameter::WidgetType::DragFloat3);

	// Rotate
	globalParameter_->SetValue(particleName_, "Rotate Base", parameters_.baseRotate, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "Rotate Max", parameters_.rotateDist.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "Rotate Min", parameters_.rotateDist.min, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "RotateSpeed Max", parameters_.rotateSpeedDist.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "RotateSpeed Min", parameters_.rotateSpeedDist.min, GlobalParameter::WidgetType::DragFloat3);

	// Velocity
	globalParameter_->SetValue(particleName_, "Speed Max", parameters_.speedDist.max, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "Speed Min", parameters_.speedDist.min, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "Direction Max", parameters_.directionDist.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "Direction Min", parameters_.directionDist.min, GlobalParameter::WidgetType::DragFloat3);

	// Color
	globalParameter_->SetValue(particleName_, "BaseColor", parameters_.baseColor, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->SetValue(particleName_, "Color Max", parameters_.colorDist.max, GlobalParameter::WidgetType::ColorEdit4);
	globalParameter_->SetValue(particleName_, "Color Min", parameters_.colorDist.min, GlobalParameter::WidgetType::ColorEdit4);

	// その他
	globalParameter_->SetValue(particleName_, "IntervalTime", intervalTime_, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "Gravity", parameters_.gravity, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "LifeTime", parameters_.lifeTime, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "Particle Count", particleCount, GlobalParameter::WidgetType::SliderInt);
	/// frag
	globalParameter_->SetValue(particleName_, "isScalerScale", parameters_.isScalerScale, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->SetValue(particleName_, "preBillBordType_", preBillBordType_, GlobalParameter::WidgetType::SliderInt);
	globalParameter_->SetValue(particleName_, "isRotateforDirection", parameters_.isRotateforDirection, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->SetValue(particleName_, "isBillBord", groupParamaters_.isBillBord, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->SetValue(particleName_, "AdaptRotateIsX", groupParamaters_.adaptRotate_.isX_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->SetValue(particleName_, "AdaptRotateIsY", groupParamaters_.adaptRotate_.isY_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->SetValue(particleName_, "AdaptRotateIsZ", groupParamaters_.adaptRotate_.isZ_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->SetValue(particleName_, "isShot", preIsShot_, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->SetValue(particleName_, "isAlphaNoMove", groupParamaters_.isAlphaNoMove, GlobalParameter::WidgetType::Checkbox);

	//easeParm
	globalParameter_->SetValue(particleName_, "scaleEaseParm.isScaleEase", parameters_.scaleEaseParm.isScaleEase, GlobalParameter::WidgetType::Checkbox);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.maxTime", parameters_.scaleEaseParm.maxTime, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.easeTypeInt", parameters_.scaleEaseParm.easeTypeInt, GlobalParameter::WidgetType::SliderInt);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueF.max", parameters_.scaleEaseParm.endValueF.max, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueF.min", parameters_.scaleEaseParm.endValueF.min, GlobalParameter::WidgetType::DragFloat);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueV3.max", parameters_.scaleEaseParm.endValueV3.max, GlobalParameter::WidgetType::DragFloat3);
	globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueV3.min", parameters_.scaleEaseParm.endValueV3.min, GlobalParameter::WidgetType::DragFloat3);

	//テクスチャ
	globalParameter_->SetValue(particleName_, "selectedTexturePath_", selectedTexturePath_, GlobalParameter::WidgetType::NONE);
}

void ParticleParameter::ApplyGlobalParameter(const std::string& particleName) {
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

	// その他
	intervalTime_ = globalParameter_->GetValue<float>(particleName, "IntervalTime");
	parameters_.gravity = globalParameter_->GetValue<float>(particleName, "Gravity");
	parameters_.lifeTime = globalParameter_->GetValue<float>(particleName, "LifeTime");
	particleCount = globalParameter_->GetValue<int32_t>(particleName, "Particle Count");

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
	parameters_.scaleEaseParm.isScaleEase = globalParameter_->GetValue<bool>(particleName, "scaleEaseParm.isScaleEase");
	parameters_.scaleEaseParm.maxTime = globalParameter_->GetValue<float>(particleName, "scaleEaseParm.maxTime");
	parameters_.scaleEaseParm.easeTypeInt = globalParameter_->GetValue<int32_t>(particleName, "scaleEaseParm.easeTypeInt");
	parameters_.scaleEaseParm.endValueF.max = globalParameter_->GetValue<float>(particleName, "scaleEaseParm.endValueF.max");
	parameters_.scaleEaseParm.endValueF.min = globalParameter_->GetValue<float>(particleName, "scaleEaseParm.endValueF.min");
	parameters_.scaleEaseParm.endValueV3.max = globalParameter_->GetValue<Vector3>(particleName, "scaleEaseParm.endValueV3.max");
	parameters_.scaleEaseParm.endValueV3.min = globalParameter_->GetValue<Vector3>(particleName, "scaleEaseParm.endValueV3.min");

	selectedTexturePath_ = globalParameter_->GetValue<std::string>(particleName, "selectedTexturePath_");
	if (selectedTexturePath_ == "")return;
	// テクスチャのハンドルを取得
	ParticleManager::GetInstance()->SetTextureHandle(particleName_, TextureManager::GetInstance()->LoadTexture(selectedTexturePath_));
}

void ParticleParameter::ApplyTexture(const std::string& texturename) {
	//テクスチャ
	selectedTexturePath_ = textureFilePath_ + "/" + texturename + ".png";
	ParticleManager::GetInstance()->SetTextureHandle(particleName_, TextureManager::GetInstance()->LoadTexture(selectedTexturePath_));
}

