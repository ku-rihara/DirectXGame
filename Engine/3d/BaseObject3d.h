#pragma once
#include"Model.h"

class BaseObject3d {
protected:
	std::unique_ptr<Model>model_ = nullptr;
	WorldTransform worldTransform_;

	//wvpリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResource_;
	TransformationMatrix* wvpDate_;

public:
	
	//virtual	void Draw();
	//デバッグ表示
	virtual	void DebugImgui();
	//WVPリソース作成
	virtual void CreateWVPResource();
	//setter
	void SetModel(std::unique_ptr<Model> model) {
		this->model_ = std::move(model);}
    void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
	void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }
};