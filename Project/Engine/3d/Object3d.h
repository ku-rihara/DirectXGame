#pragma once

#include"BaseObject3d.h"
#include"ObjectColor.h"
//#include"base/Material.h"

#include"3d/WorldTransform.h"
#include"struct/TransformationMatrix.h"

class Object3d :public BaseObject3d {
public:
	ObjectColor objColor_;
private:

	///============================================================
	///private variant
	///============================================================
	
	//wvpリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResource_;
	TransformationMatrix* wvpDate_;

public:
	Object3d();
	~Object3d();

	///============================================================
	///public method
	///============================================================
	
	/// モデル作成
	static Object3d* CreateModel(const std::string& instanceName);

	/// 初期化、更新、描画
	void Init();
	void ColorUpdate();
	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt);
	void Draw(const Vector3& position, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt);

	void DebugImgui()override;//デバッグ表示
	void CreateWVPResource();//WVPリソース作成
	void CreateMaterialResource()override;// マテリアルリソース作成

	///============================================================
	/// setter method
	///============================================================
	void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
	void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }


};