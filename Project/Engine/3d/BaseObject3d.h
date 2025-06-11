#pragma once
#include"Model.h"
#include"base/Material.h"
#include"base/Object3DCommon.h"

class BaseObject3d {
public:
	Material material_;
protected:
	///========================================================================================
	///  protected variant
	///========================================================================================

	Model* model_ = nullptr;
	BlendMode blendMode = BlendMode::None;

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
	void SetBlendMode(BlendMode mode) { blendMode = mode; }
	void SetModel(const std::string& modelName);
};