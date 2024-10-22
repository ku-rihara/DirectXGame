#pragma once

//math
#include"Vector3.h"
//3d
#include"ViewProjection.h"
//obj
#include"Object3d.h"
class Enemy {
	static int instanceNum_;
public://メンバ変数
	Object3d* objct3D_;
private:
	int instanceID_;
	bool isDeath_;
public://メンバ関数

	void Init();
	void Update();
	void Debug();
	void Draw(const ViewProjection& viewProjection);

	Vector3 GetPos() { return objct3D_->transform_.translation_; }
	bool GetIsDeath() { return isDeath_; }

	//setter

	/// <summary>
	/// ポジションセット
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(const Vector3& pos) { objct3D_->transform_.translation_ = pos; }
};