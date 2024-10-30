#pragma once

//math
#include"Vector3.h"
//3d
#include"3d/ViewProjection.h"
//obj
#include"3d/Object3d.h"
#include "PlayerBullet.h"
class Reticle;
class Player {
private:
	/// other class
	Reticle* pReticle_;
  
	WorldTransform transform_;
	std::unique_ptr<Object3d> obj3D_;
	bool isDeath_;
	// 弾リスト
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

public://メンバ関数

	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	void BulletDraw(const ViewProjection& viewProjection);

	/// レティクルセット
	void SetReticle(Reticle* reticle);

	Matrix4x4 GetMatWorld() const { return transform_.matWorld_; }
	/// <summary>
	/// 弾発射
	/// </summary>
	/// <returns></returns>
	void BulletShot();

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