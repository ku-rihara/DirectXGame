#include"BaseObject3d.h"
#include"DirectXCommon.h"
#include"ModelManager.h"

void BaseObject3d::DebugImgui() {
	if (model_) {
		model_->DebugImGui();

	}
}

void BaseObject3d::SetModel(const std::string& modelName, const std::string& extension) {

	//モデルを検索してセット
	model_=(ModelManager::GetInstance()->FindModel(modelName, extension));
}