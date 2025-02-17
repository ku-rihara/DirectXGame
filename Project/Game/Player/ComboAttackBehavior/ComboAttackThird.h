#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"
#include"CollisionBox/AttackCollisionBox.h"

class RailManager;
class ComboAttackThird : public BaseComboAattackBehavior {
private:
	enum class Order {
		UPPER,
		WAIT,
		FALL,
	};
private:

	/// ===================================================
	///private varians
	/// ===================================================

	Order order_;               /// 振る舞い順序
	RailManager* railManager_;
	std::unique_ptr<AttackCollisionBox>collisionBox_;
	const float kCollisionAliveTime_=0.3f;
	float fallInitSpeed_;

	/// パンチ
	Easing punchEase_;
	float upperJumpEaseT_;
	float waitTine_;          /// 次コンボまでの待機時間
	float initPosY_;

	float zRotate_;

	Easing rotateEase_;

public:
	//コンストラクタ
	ComboAttackThird(Player* boss);
	~ComboAttackThird();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;
};
