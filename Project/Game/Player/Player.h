#pragma once

//math
#include"Vector3.h"
//3d
#include"3d/ViewProjection.h"
//obj
#include"3d/Object3d.h"
#include "Player/PlayerBeam.h"
class Reticle;
class Player {
private:

	///============================================
	///private variants
	/// ===========================================

	/// other class
	Reticle* pReticle_;
  
	WorldTransform transform_;
	std::unique_ptr<Object3d> obj3D_;
	bool isDeath_;/// 死亡フラグ
	std::unique_ptr<PlayerBeam> beam_;// 弾

public://メンバ関数

	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	void SpriteDraw();
	void Debug();
	void BulletDraw(const ViewProjection& viewProjection);

	/// レティクルセット
	void SetReticle(Reticle* reticle);

	Matrix4x4 GetMatWorld() const { return transform_.matWorld_; }
	/// <summary>
	/// 弾発射
	/// </summary>
	/// <returns></returns>
	void BeamShot();

	////////////////////////////////////////////////////////////////////////////////////////////
	//  getter method
	////////////////////////////////////////////////////////////////////////////////////////////

	bool GetIsDeath() { return isDeath_; }
	Vector3 GetWorldPos();
	////////////////////////////////////////////////////////////////////////////////////////////
	//    setter method
	////////////////////////////////////////////////////////////////////////////////////////////
	void SetParent(const WorldTransform* parent);
};