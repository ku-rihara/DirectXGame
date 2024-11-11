#pragma once

#include"ControlPoint/BaseSpeedControl.h"

class SlowPoint:public BaseSpeedControl {
	static int instanceNum_;

private:

	///============================================
	///private variants
	/// ===========================================
	
	std::unique_ptr<Object3d> objct3D_;
	WorldTransform transform_;
	int instanceID_;
	bool isDeath_;
public://メンバ関数

	void Init()override;
	void Update()override;
	void Debug()override;
	void Draw(const ViewProjection& viewProjection)override;

	Vector3 GetPos() { return transform_.translation_; }
	bool GetIsDeath() { return isDeath_; }

	//setter

	/// <summary>
	/// ポジションセット
	/// </summary>
	/// <param name="pos"></param>
	void SetPos(const Vector3& pos) { transform_.translation_ = pos; }
};