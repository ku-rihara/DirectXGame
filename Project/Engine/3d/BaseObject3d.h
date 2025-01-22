#pragma once
#include"Model.h"
#include"base/Material.h"
#include"base/Object3DCommon.h"

enum class Object3DCommon::BlendMode;
class BaseObject3d {
public:
	Material material_;
protected:
	///========================================================================================
	///  protected variant
	///========================================================================================

	Model* model_ = nullptr;
	Object3DCommon::BlendMode blendMode = Object3DCommon::BlendMode::None;

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
	void SetBlendMode(Object3DCommon::BlendMode mode) { blendMode = mode; }
	void SetModel(const std::string& modelName, const std::string& extension);
};