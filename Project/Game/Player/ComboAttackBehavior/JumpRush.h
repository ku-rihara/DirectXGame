#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"
#include"CollisionBox/AttackCollisionBox.h"

class RailManager;
class JumpRush : public BaseComboAattackBehavior {
private:
	enum class Order {
		CALUCRATION,
		RUSH,
		NEXTBEHAVIOR,
	};
private:

	/// ===================================================
	///private varians
	/// ===================================================

	Order order_;

	Vector3 forwardDirection_;

	Easing assaultEase_;
	Vector3 initPos_;
	Vector3 preWorldPos_;
	Vector3 frontPos_;


public:
	//コンストラクタ
	JumpRush(Player* boss);
	~JumpRush();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;

};
