#pragma once
#include"Model.h"

class BaseObject3d {
protected:
	std::unique_ptr<Model>model_ = nullptr;
	
public:
	
	//virtual	void Draw();
	//デバッグ表示
	virtual	void DebugImgui();
	
	//setter
	void SetModel(std::unique_ptr<Model> model) {
		this->model_ = std::move(model);}
  };