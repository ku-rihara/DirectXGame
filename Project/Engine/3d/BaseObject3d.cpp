#include"BaseObject3d.h"
#include"base/DirectXCommon.h"
#include"ModelManager.h"

void BaseObject3d::DebugImgui() {
#ifdef _DEBUG
	material_.DebugImGui();
#endif
}

void BaseObject3d::SetModel(const std::string& modelName, const std::string& extension) {

	//モデルを検索してセット
	model_=(ModelManager::GetInstance()->FindModel(modelName, extension));
}

void BaseObject3d::CreateMaterialResource() {
	material_.CreateMaterialResource(DirectXCommon::GetInstance());
}

void BaseObject3d::SetLightMode(uint32_t num) {
	material_.materialData_->enableLighting = num;
}