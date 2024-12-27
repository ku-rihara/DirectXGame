#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"
#include"CollisionBox/PunchCollisionBox.h"

class ComboAttackFirst : public BaseComboAattackBehavior {
private:
	enum class Order{
		RUSH,
		PUNCH,
		BACKPUNCH,
		WAIT,
	};
private:
	
	/// ===================================================
	///private varians
	/// ===================================================

	Order order_;                /// 振る舞い順序

	// collision
	std::unique_ptr<PunchCollisionBox>collisionBox_;

	/// 突進
	Vector3 initPos_;            /// スタート座標
	Vector3 rushPos_;            /// 突進座標
	Vector3 forwardDirection_;   /// 向き
	float speed_;
	Easing rushEase_;

	/// パンチ
	Easing  punchEase_;
	Vector3 rHandStartPos_;    /// ハンドスタート座標
	Vector3 rHandTargetPos_;   /// ハンドターゲット座標
	Vector3 punchPosition_;

	Vector3 collisionSize_;
	
	float waitTine_;          /// 次コンボまでの待機時間

public:
	//コンストラクタ
	ComboAttackFirst(Player* boss);
	~ComboAttackFirst();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;


	void ChangeSpeedForLockOn();
};
