#include "ModelManager.h"

using namespace KetaEngine;

ModelManager* ModelManager::instance_ = nullptr;
std::map<std::string, std::unique_ptr<Model>> ModelManager::models_;

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

Model* ModelManager::LoadModel(const std::string& modelName) {
    // 読み込み済みモデルを検索
    if (models_.contains(modelName)) {
        // 読み込み済みなら早期リターン
        return nullptr;
    }
    Model* model = new Model();
    model->CreateModel(modelName);
    // モデルをmapコンテナに格納する
    models_.insert(std::make_pair(modelName, std::move(model)));

    return model;
}


void ModelManager::LoadAllModels() {
    const std::filesystem::path modelDir = "Resources/Model";
    if (!std::filesystem::exists(modelDir)) {
        return;
    }
    // カテゴリフォルダを巡回 (Resources/Model/Category/ModelName/ModelName.ext)
    for (const auto& category : std::filesystem::directory_iterator(modelDir)) {
        if (!category.is_directory()) {
            continue;
        }
        std::string categoryName = category.path().filename().string();
        for (const auto& modelFolder : std::filesystem::directory_iterator(category.path())) {
            if (!modelFolder.is_directory()) {
                continue;
            }
            for (const auto& file : std::filesystem::directory_iterator(modelFolder.path())) {
                if (!file.is_regular_file()) {
                    continue;
                }
                const auto& ext = file.path().extension();
                if (ext == ".gltf" || ext == ".obj") {
                    std::string modelFileName = categoryName + "/" + file.path().filename().string();
                    LoadModel(modelFileName);
                    break;
                }
            }
        }
    }
}

Model* ModelManager::FindModel(const std::string& modelName) {
    // 読み込み済モデルを検索
    if (models_.contains(modelName)) {
        // 読み込み済モデルをreturn 
        return models_.at(modelName).get();
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
