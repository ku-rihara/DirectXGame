#pragma once

//math
#include"Vector3.h"
//3d
#include"ViewProjection.h"
//obj
#include"Object3d.h"
#include "PlayerBullet.h"
class Player {
private:	
	std::unique_ptr<Object3d> obj3D_;
	bool isDeath_;
	// 弾リスト
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	
public://メンバ関数

	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);

	bool GetIsDeath() { return isDeath_; }

	//setter
	void SetParent(const WorldTransform* parent);
};