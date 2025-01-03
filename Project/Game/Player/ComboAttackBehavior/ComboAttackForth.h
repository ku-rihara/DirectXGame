#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"
#include"CollisionBox/ThrustCollisionBox.h"
#include"CollisionBox/StopCollisionBox.h"
#include"utility/Editor/RailManager.h"

class ComboAttackForth : public BaseComboAattackBehavior {
private:
	///　振る舞いオーダー
	enum class Order {
		RPUNCH,
		LPUNCH,
		BACKPUNCH,
		WAIT,
	};
private:

	/// ===================================================
	///private varians
	/// ===================================================

	Order order_;                /// 振る舞い順序

	RailManager* stopRailManager_;
	RailManager* thrustRailManager_;

	// collision
	std::unique_ptr<StopCollisionBox>stopCollisionBox_;
	std::unique_ptr<ThrustCollisionBox>thrustCollisionBox_;

	float waitTine_;          /// 次コンボまでの待機時間

public:

	//コンストラクタ
	ComboAttackForth(Player* boss);
	~ComboAttackForth();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;


};
