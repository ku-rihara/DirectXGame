#pragma once
//std
#include<memory>
//math
#include"Vector3.h"
//3d
#include"3d/ViewProjection.h"
//obj
#include"3d/Object3d.h"
class ControlPoint {
	static int instanceNum_;
public://メンバ変数
	std::unique_ptr<Object3d> objct3D_;
	WorldTransform transform_;
private:
	int instanceID_;
	bool isDeath_;
public://メンバ関数

	void Init();
	void Update();
	void Debug();
	void Draw(const ViewProjection& viewProjection);

	Vector3 GetPos() { return transform_.translation_; }
	bool GetIsDeath() { return isDeath_; }

	//setter

	/// <summary>
	/// ポジションセット
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(const Vector3& pos) { transform_.translation_ = pos; }
};