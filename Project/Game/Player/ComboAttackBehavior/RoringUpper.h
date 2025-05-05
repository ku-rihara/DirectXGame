#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"
#include"CollisionBox/AttackCollisionBox.h"

class RailManager;
class RoringUpper : public BaseComboAattackBehavior {
private:
	enum class Order {
		UPPER,
		WAIT,
		END,
	};
private:

	/// ===================================================
	///private varians
	/// ===================================================

	Order order_;               /// 振る舞い順序
	RailManager* railManager_;
	std::unique_ptr<AttackCollisionBox>collisionBox_;
    float kCollisionAliveTime_;
	float fallInitSpeed_;
	float collisionSize_;
	Vector3 forwardDirection_;

	/// パンチ
	Easing punchEase_;
	float upperJumpEaseT_;
	float waitTine_;          /// 次コンボまでの待機時間
	Vector3 initPos_;

	///反動
	float xRotate_;
	float jumpPower_;
	float gravity_;
	float fallSpeedLimit_;
	Vector3 preWorldPos_;
	Vector3 backlashPos_;
	Easing backlashEase_;

	//hitstop
	bool isHitStop_;
	float hitStopTime_;
	float kHitStopTime_;


public:
	//コンストラクタ
	RoringUpper(Player* boss);
	~RoringUpper();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;

	void CollisionInit();
	void RailInit();
	void HitStopUpdate();
	void ChangeOrder();
	void AnimationMove();
};
