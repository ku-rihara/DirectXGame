#pragma once
#include"Model.h"
#include"base/Material.h"

class BaseObject3d {
protected:
	///========================================================================================
	///  protected variant
	///========================================================================================

	Model* model_ = nullptr;
	Material material_;

public:

	///========================================================================================
	///  public method
	///========================================================================================

	//デバッグ表示
	virtual	void DebugImgui();

	// マテリアルリソース作成
	virtual void CreateMaterialResource();

	/*virtual void Debug() = 0;*/

    ///========================================================================================
    ///  setter method
   ///========================================================================================

	void SetModel(Model* model) {this->model_ = model;}

	void SetModel(const std::string& modelName, const std::string& extension);
};