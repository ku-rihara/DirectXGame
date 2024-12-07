#pragma once
#include"BaseObject3d.h"
#include"3d/ViewProjection.h"
#include"3d/WorldTransform.h"
#include"struct/TransformationMatrix.h"
#include<list>

class SrvManager;

class Object3dSRV :public BaseObject3d {
public:
	
	 std::list<WorldTransform>particles_;
	
private:
	uint32_t instanceMax_;
	uint32_t instanceNum_;//インスタンス数
	
	SrvManager* pSrvManager_;

	//リソースとデータ
	uint32_t srvIndex_;
	TransformationMatrix* instancingData_;
	//パーティクル変数
	const float kDeltaTime_ = 1.0f / 60.0f;
	
private:
	/// <summary>
	/// リソース作成
	/// </summary>
	/// <param name="instanceNum"></param>
	/// <param name="randomEngine"></param>
	/// <param name="dist"></param>
	void CreateInstancingResource(const uint32_t& instanceNum);
	/// <summary>
	/// サイズ確保
	/// </summary>
	void Clear();

public:
	/// <summary>
	/// モデル生成
	/// </summary>
	/// <param name="instanceName"></param>
	/// <param name="instanceNum"></param>
	/// <param name="randomEngine"></param>
	/// <param name="dist"></param>
	/// <returns></returns>
	static Object3dSRV* CreateModel(
		const std::string& instanceName, const std::string& extension, 
		const uint32_t& instanceNum, SrvManager*srvManager );
	/// <summary>
	/// 更新
	/// </summary>
	void UpdateTransform(std::optional<const ViewProjection*>viewProjection=std::nullopt);
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	/// <param name="textureHandle"></param>
	void Draw(const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt);
	/// <summary>
	/// Debug表示
	/// </summary>
	void DebugImgui()override;
	// マテリアルリソース作成
	void CreateMaterialResource()override;
	
	
	//getter
	uint32_t GetKnumInstance()const { return instanceNum_; }
public:
	
};