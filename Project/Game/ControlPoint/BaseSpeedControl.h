#pragma once
//std
#include<memory>
//math
#include"Vector3.h"
//3d
#include"3d/ViewProjection.h"
//obj
#include"3d/Object3d.h"
#include"Camera/GameCamera.h"


class BaseSpeedControl {
	
protected://メンバ変数
	std::unique_ptr<Object3d> objct3D_;
	WorldTransform transform_;	
	bool isDeath_;

	

public://メンバ関数

	BaseSpeedControl();
virtual ~BaseSpeedControl();

	virtual void Init();
	virtual void Update();
	virtual void Debug()=0;
	virtual void Draw(const ViewProjection& viewProjection);

	Vector3 GetPos() { return transform_.translation_; }
	bool GetIsDeath() { return isDeath_; }

	
	/// <summary>
	/// ポジションセット
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(const Vector3& pos) { transform_.translation_ = pos; }
};
//
//int instanceID_; static int instanceNum_;