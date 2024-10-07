#pragma once
#include"Model.h"
/// <summary>
/// モデルマネージャー
/// </summary>
class ModelManager {
private:
	static ModelManager* instance_;
	std::unique_ptr<ModelCommon> modelCommon = nullptr;
	//モデルデータ
	std::map<std::string, std::unique_ptr<Model>>models_;
	
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
	/// モデルファイル読み込み
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="extension"></param>
	void LoadModel(const std::string& modelName, const std::string& extension);

	/// <summary>
	/// モデルパーティクルファイル読み込み
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="extension"></param>
	void LoadModelParticle(const std::string& modelName, const std::string& extension);

	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="modelName"></param>
	/// <returns></returns>
	Model* FindModel(const std::string& modelName, const std::string& extension);

	//シングルトンインスタンス取得
	static ModelManager* GetInstance();
	//終了
	void Finalize();

};