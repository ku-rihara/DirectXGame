#pragma once

//math
#include"Vector3.h"
//3d
#include"ViewProjection.h"
//obj
#include"Object3d.h"
class Player {
private:	
	Object3d* obj3D_;
	bool isDeath_;
public://メンバ関数

	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);

	bool GetIsDeath() { return isDeath_; }

	//setter
	void SetParent(const WorldTransform* parent);
};