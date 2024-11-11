#pragma once

//math
#include"Vector3.h"
//3d
#include"3d/ViewProjection.h"
//obj
#include"3d/Object3d.h"
#include "Player/PlayerBeam.h"
#include"Colider/AABBCollider.h"
class Reticle;
class GameCamera;
class Player:public AABBCollider {
private:

	///============================================
	///private variants
	/// ===========================================

	/// other class
	Reticle* pReticle_;
	GameCamera* pGameCamera_;
  
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
    void SetGameCamera(GameCamera* gamecamera);


	/// Collider
	virtual	Vector3 GetBaseCenterPosition() const override;
	virtual	void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;


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