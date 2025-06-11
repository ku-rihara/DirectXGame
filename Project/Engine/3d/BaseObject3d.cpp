#include"BaseObject3d.h"
#include"Dx/DirectXCommon.h"
#include"ModelManager.h"

void BaseObject3d::DebugImgui() {
#ifdef _DEBUG
	material_.DebugImGui();
#endif
}

void BaseObject3d::SetModel(const std::string& modelName) {

	//モデルを検索してセット
	model_=(ModelManager::GetInstance()->FindModel(modelName));
}

void BaseObject3d::CreateMaterialResource() {
	material_.CreateMaterialResource(DirectXCommon::GetInstance());


}