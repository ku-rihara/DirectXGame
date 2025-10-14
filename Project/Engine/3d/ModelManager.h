#pragma once
#include "Model.h"
#include <map>
#include <memory>
#include <string>

class ModelManager {
private:
    ///============================================================
    /// Singleton Restrictions
    ///============================================================
    ModelManager()                               = default;
    ~ModelManager()                              = default;
    ModelManager(const ModelManager&)            = delete;
    ModelManager& operator=(const ModelManager&) = delete;

public:
    ///============================================================
    /// public method
    ///============================================================

    // シングルトンインスタンス取得
    static ModelManager* GetInstance();

    /// 初期化
    void Initialize(DirectXCommon* dxCommon);

    /// モデルファイル読み込み
    static Model* LoadModel(const std::string& modelName);

    /// モデルの検索
    Model* FindModel(const std::string& modelName);

    // 終了
    void Finalize();

private:
    ///============================================================
    /// private variants
    ///============================================================

    static ModelManager* instance_;
    static std::map<std::string, std::unique_ptr<Model>> models_;
    std::unique_ptr<ModelCommon> modelCommon = nullptr;
};
