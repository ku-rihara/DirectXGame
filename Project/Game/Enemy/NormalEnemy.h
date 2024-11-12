#pragma once

#include"Enemy/BaseEnemy.h"

class NormalEnemy :public BaseEnemy {
	static int instanceNum_;
private:
	WorldTransform transfrom2_;
	std::unique_ptr<Object3d>ojb3d_;

public:
	///==================================================
	///public method
	///==================================================
	/// 
	/// 
	

	/// 初期化、更新、デバッグ、描画
	virtual void Init(const std::string& instanceName, const std::string& extension);
	virtual void Update();
	virtual void Debug();
	virtual void Draw(const ViewProjection& viewProjection);

	void Move()override;

	/*/// Collider
	virtual	Vector3 GetBaseCenterPosition() const override;*/
	//virtual	void OnCollisionEnter([[maybe_unused]] Collider* other)override;
};