#pragma once
//#include"Model.h"
#include"BaseObject3d.h"

class Object3d:public BaseObject3d  {
public:
	
	WorldTransform transform_;
private:
	//wvpリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResource_;
	TransformationMatrix* wvpDate_;

public:
	static Object3d* CreateModel(const std::string& instanceName, const std::string& extension);
	//更新
	void Update();
	//描画
	void Draw( const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt, const Vector4& color = { 1,1,1,1 });
	//デバッグ表示
	void DebugImgui()override;
	//WVPリソース作成
	 void CreateWVPResource();

	 void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
	 void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }

	
};