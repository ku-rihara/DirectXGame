#pragma once

#include"BaseObject3d.h"
#include"ObjectColor.h"
#include"base/Material.h"

class Object3d :public BaseObject3d {
public:
	ObjectColor objColor_;
	
private:
	//wvpリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResource_;
	TransformationMatrix* wvpDate_;

public:
	Object3d();
	~Object3d();
	/// 初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt);
	//デバッグ表示
	void DebugImgui()override;
	//WVPリソース作成
	void CreateWVPResource();
	// マテリアルリソース作成
	void CreateMaterialResource()override;

	static Object3d* CreateModel(const std::string& instanceName, const std::string& extension);
	void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
	void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }


};