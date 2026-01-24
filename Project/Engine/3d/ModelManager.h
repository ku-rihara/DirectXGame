#pragma once
#include "3D/Model.h"
#include <map>
#include <memory>
#include <string>

/// <summary>
/// モデル管理クラス
/// </summary>
namespace KetaEngine {

class ModelManager {
private:
    ModelManager()                               = default;
    ~ModelManager()                              = default;
    ModelManager(const ModelManager&)            = delete;
    ModelManager& operator=(const ModelManager&) = delete;

public:
    static ModelManager* GetInstance(); //< シングルトンインスタンス取得

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommonのインスタンス</param>
    void Initialize(DirectXCommon* dxCommon);

    /// <summary>
    /// モデルファイル読み込み
    /// </summary>
    /// <param name="modelName">モデル名</param>
    /// <returns>モデルのポインタ</returns>
    static Model* LoadModel(const std::string& modelName);

    /// <summary>
    /// モデルの検索
    /// </summary>
    /// <param name="modelName">モデル名</param>
    /// <returns>モデルのポインタ</returns>
    Model* FindModel(const std::string& modelName);

    void Finalize(); //< 終了処理

private:
    static ModelManager* instance_;
    static std::map<std::string, std::unique_ptr<Model>> models_;
    std::unique_ptr<ModelCommon> modelCommon = nullptr;
};

}; // KetaEngine
