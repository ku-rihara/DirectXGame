#pragma once
#include"Model.h"

class BaseObject3d {
protected:
	Model* model_ = nullptr;

public:
	/*BaseObject3d() = default;
	~BaseObject3d() = default;*/

	//デバッグ表示
	virtual	void DebugImgui();

	//setter
	void SetModel(Model* model) {this->model_ = model;}

	void SetModel(const std::string& modelName, const std::string& extension);
};