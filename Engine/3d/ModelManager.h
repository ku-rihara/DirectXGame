#pragma once
#include "Model.h"
#include <map>
#include <memory>
#include <string>

/// <summary>
/// モデルマネージャー
/// </summary>
class ModelManager {
private:
    static ModelManager* instance_;
    static std::map<std::string, std::unique_ptr<Model>> models_; // staticに変更
    std::unique_ptr<ModelCommon> modelCommon = nullptr;

    ModelManager() = default;
    ~ModelManager() = default;
    ModelManager(const ModelManager&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;

public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon"></param>
    void Initialize(DirectXCommon* dxCommon);

    /// <summary>
    /// モデルファイル読み込み (static)
    /// </summary>
    /// <param name="modelName"></param>
    /// <param name="extension"></param>
    static Model* LoadModel(const std::string& modelName, const std::string& extension);

    /// <summary>
    /// モデルパーティクルファイル読み込み
    /// </summary>
    /// <param name="modelName"></param>
    /// <param name="extension"></param>
    void LoadModelParticle(const std::string& modelName, const std::string& extension);

    /// <summary>
    /// モデルの検索
    /// </summary>
    /// <param name="modelName"></param>
    /// <returns></returns>
    Model* FindModel(const std::string& modelName, const std::string& extension);

    // シングルトンインスタンス取得
    static ModelManager* GetInstance();

    // 終了
    void Finalize();
};
