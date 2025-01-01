#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"
#include"CollisionBox/UpperCollisionBox.h"

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
	std::unique_ptr<UpperCollisionBox>collisionBox_;
	const float kCollisionAliveTime_=0.3f;

	/// パンチ
	Easing punchEase_;
	float upperJumpEaseT_;
	float waitTine_;          /// 次コンボまでの待機時間
	float initPosY_;

public:
	//コンストラクタ
	ComboAttackThird(Player* boss);
	~ComboAttackThird();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;
};
