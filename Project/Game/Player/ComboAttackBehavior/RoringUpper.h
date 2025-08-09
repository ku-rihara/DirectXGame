#pragma once

#include"BaseComboAattackBehavior.h"
#include"EasingFunction.h"
#include"CollisionBox/PlayerAttackController.h"
#include"Easing.h"

class RailManager;
class RoringUpper : public BaseComboAattackBehavior {

public:
    // コンストラクタ
    RoringUpper(Player* player);
    ~RoringUpper();

    /// 更新、デバッグ
    void Update() override;
    void Debug() override;

    void CollisionInit();
    void RailInit();
    void HitStopUpdate();
    void ChangeOrder();
    void AnimationMove();

	void EasingInit();

private:
	enum class Order {
		UPPER,
		WAIT,
		END,
	};
private:

	/// ===================================================
	/// private varians
	/// ===================================================

	Order order_;               /// 振る舞い順序
	RailManager* railManager_;
	/*std::unique_ptr<PlayerAttackController>collisionBox_;*/
    float kCollisionAliveTime_;
	float fallInitSpeed_;
	float collisionSize_;
	Vector3 forwardDirection_;

	/// パンチ
  /*  Easing<Vector3> punchEase_;*/
	float waitTine_;          /// 次コンボまでの待機時間
	Vector3 initPos_;

	///反動
	float jumpPower_;
	float gravity_;
	float fallSpeedLimit_;
	Vector3 tempWorldPos_;
	Vector3 backlashPos_;
	Easing<Vector3> backlashEase_;

	Easing<float> rotateEase_; 
	float xRotate_;
	
	//hitstop
	bool isHitStop_;
	/*float hitStopTime_;
	float kHitStopTime_;*/

	float collisionAdaptTime_;

};
