#pragma once

#include"BaseComboAattackBehavior.h"

#include"CollisionBox/PlayerAttackController.h"
#include"utility/RailEditor/RailManager.h"

class ThrustPunch : public BaseComboAattackBehavior {
private:
	///　振る舞いオーダー
	enum class Order {
		START,
		LPUNCH,
		LBACKPUNCH,
		WAIT,
	};
private:

	/// ===================================================
	///private varians
	/// ===================================================

	Order order_;               

	RailManager* stopRailManager_;
	RailManager* thrustRailManager_;

	/// 
	float firstWaitTime_;
	float firstWaitTimeMax_;
	float fallInitSpeed_;

	float timeDownTime_;
	float kTimeDownTime_;
	bool istimeSlow_;

	
	
	float waitTine_;        

public:

	//コンストラクタ
    ThrustPunch(Player* player);
	~ThrustPunch();

	/// 更新、デバッグ
    void Init() override;
	void Update()override;
	void Debug()override;
	void ChangeSlow();


};
