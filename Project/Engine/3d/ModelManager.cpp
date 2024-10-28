#include "ModelManager.h"

ModelManager* ModelManager::instance_ = nullptr;
std::map<std::string, std::unique_ptr<Model>> ModelManager::models_; // staticな変数として定義

// シングルトンインスタンス取得
ModelManager* ModelManager::GetInstance() {
    if (!instance_) {
        instance_ = new ModelManager();
    }
    return instance_;
}

void ModelManager::Initialize(DirectXCommon* dxCommon) {
    modelCommon = std::make_unique<ModelCommon>();
    modelCommon->Init(dxCommon);
}

Model* ModelManager::LoadModel(const std::string& modelName, const std::string& extension) {
    // 読み込み済みモデルを検索
    if (models_.contains(modelName + extension)) {
        // 読み込み済みなら早期リターン
        return nullptr;
    }
    Model* model = new Model();
    model->CreateModel(modelName, extension);
    // モデルをmapコンテナに格納する
    models_.insert(std::make_pair(modelName + extension, std::move(model)));

    return model;
}

//void ModelManager::LoadModelParticle(const std::string& modelName, const std::string& extension) {
//    //// 読み込み済みモデルを検索
//    //if (models_.contains(modelName + extension)) {
//    //    // 読み込み済みなら早期リターン
//    //    return;
//    //}
//    //std::unique_ptr<Model> model = std::make_unique<Model>();
//    //model->CreateModel(modelName, extension);
//    //// モデルをmapコンテナに格納する
//    //models_.insert(std::make_pair(modelName + extension, std::move(model)));
//}

Model* ModelManager::FindModel(const std::string& modelName, const std::string& extension) {
    // 読み込み済モデルを検索
    if (models_.contains(modelName + extension)) {
        // 読み込み済モデルをreturn 
        return models_.at(modelName + extension).get();
    }
    // ファイル名一致なし
    return nullptr;
}

// 終了
void ModelManager::Finalize() {
    models_.clear(); // モデルデータを解放
    modelCommon.reset(); // ModelCommonを解放
    delete instance_;
    instance_ = nullptr; // インスタンスをnullptrにする
}
