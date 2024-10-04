#pragma once
//#include"Model.h"
#include"BaseObject3d.h"

class Object3d:public BaseObject3d  {
public:
	

public:
	static Object3d* CreateModel(const std::string& instanceName);
	//更新
	void Update();
	//描画
	void Draw( const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt, const Vector4& color = { 1,1,1,1 });
	//デバッグ表示
	void DebugImgui()override;
	//WVPリソース作成
	 void CreateWVPResource()override;
	////setter
	//void SetModel(Model* model) { this->model_ = model; }
};