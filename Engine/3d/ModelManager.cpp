#include"ModelManager.h"

ModelManager* ModelManager::instance_ = nullptr;

// シングルトンインスタンス取得
ModelManager* ModelManager::GetInstance() {
	if (!instance_) {
		instance_ = new ModelManager();
	}
	return instance_;
}
//初期化
void  ModelManager::Initialize(DirectXCommon* dxCommon) {
	modelCommon = std::make_unique<ModelCommon>();
	modelCommon->Init(dxCommon);
}
//ロード
void ModelManager::LoadModel(const std::string& modelName, const std::string& extension) {
	std::unique_ptr<Model>model = std::make_unique<Model>();
	//読み込み済みモデルを検索
	if (models_.contains(modelName + extension)) {
		//読み込み済みなら早期リターン
		return;
	}
	model->CreateModel(modelName, extension);
	//モデルをmapコンテナに格納する
	models_.insert(std::make_pair(modelName + extension, std::move(model)));
}
//パーティクルロード
void ModelManager::LoadModelParticle(const std::string& modelName, const std::string& extension) {
	//読み込み済みモデルを検索
	if (models_.contains(modelName + extension)) {
		//読み込み済みなら早期リターン
		return;
	}
	std::unique_ptr<Model>model = std::make_unique<Model>();
	model->CreateModel(modelName, extension);
	model->CreateBufferCommon();
	//モデルをmapコンテナに格納する
	models_.insert(std::make_pair(modelName + extension, std::move(model)));
}
//モデル検索
Model* ModelManager::FindModel(const std::string& modelName, const std::string& extension) {
	//読み込み済モデルを検索
	if (models_.contains(modelName + extension)) {
		//読み込み済モデルをreturn 
		return models_.at(modelName + extension).get();
	}
	//ファイル名一致なし
	return nullptr;
}

// 終了
void ModelManager::Finalize() {

	models_.clear(); // モデルデータを解放
	modelCommon.reset(); // ModelCommonを解放
	delete instance_;
	instance_ = nullptr; // インスタンスをnullptrにする
}