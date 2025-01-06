#pragma once
#include"Model.h"
#include"base/Material.h"

class BaseObject3d {
public:
	Material material_;
protected:
	///========================================================================================
	///  protected variant
	///========================================================================================

	Model* model_ = nullptr;


public:

	///========================================================================================
	///  public method
	///========================================================================================

	//デバッグ表示
	virtual	void DebugImgui();

	// マテリアルリソース作成
	virtual void CreateMaterialResource();

	
    ///========================================================================================
    ///  setter method
   ///========================================================================================

	void SetModel(Model* model) {this->model_ = model;}
	void SetModel(const std::string& modelName, const std::string& extension);
};