#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"
#include"CollisionBox/AttackCollisionBox.h"
#include"utility/RailEditor/RailManager.h"

class ThrustPunch : public BaseComboAattackBehavior {
private:
	///　振る舞いオーダー
	enum class Order {
		FIRSTWAIT,
		RPUNCH,
		RBACKPUNCH,
		LPUNCH,
		LBACKPUNCH,
		WAIT,
	};
private:

	/// ===================================================
	///private varians
	/// ===================================================

	Order order_;                /// 振る舞い順序

	RailManager* stopRailManager_;
	RailManager* thrustRailManager_;

	/// 
	float firstWaitTime_;
	float firstWaitTimeMax_;
	float fallInitSpeed_;

	float timeDownTime_;
	float kTimeDownTime_;
	bool istimeSlow_;

	// collision
	std::unique_ptr<AttackCollisionBox>collisionBox_;
	
	float waitTine_;          /// 次コンボまでの待機時間

public:

	//コンストラクタ
    ThrustPunch(Player* player);
	~ThrustPunch();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;
	void ChangeSlow();

	void ThrustCollisionSet();
};
