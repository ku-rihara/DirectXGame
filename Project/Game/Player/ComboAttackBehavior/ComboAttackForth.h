#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"
#include"CollisionBox/PunchCollisionBox.h"

class ComboAttackForth : public BaseComboAattackBehavior {
private:
	///　振る舞いオーダー
	enum class Order {
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


	/// パンチ
	Easing punchEase_;         /// パンチイージング
	Vector3 lHandStartPos_;    /// ハンドスタート座標
	Vector3 lHandTargetPos_;   /// ハンドターゲット座標
	Vector3 punchPosition_;    /// パンチ位置

	float waitTine_;          /// 次コンボまでの待機時間

public:

	//コンストラクタ
	ComboAttackForth(Player* boss);
	~ComboAttackForth();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;


};
