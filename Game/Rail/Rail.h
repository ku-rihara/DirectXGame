#pragma once

//math
#include"Vector3.h"
//3d
#include"ViewProjection.h"
//obj
#include"Object3d.h"
class Rail {
	static int instanceNum_;
public://メンバ変数
	std::unique_ptr<Object3d>objct3D_;
private:
	
	bool isDeath_;
public://メンバ関数

	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);

	bool GetIsDeath() { return isDeath_; }

	//setter

	/// <summary>
	/// ポジションセット
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(const Vector3& pos) { objct3D_->transform_.translation_ = pos; }
};