#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"

class ComboAttackSecond : public BaseComboAattackBehavior {
private:
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

	/// パンチ
	Easing punchEase_;
	Vector3 lHandStartPos_;    /// ハンドスタート座標
	Vector3 lHandTargetPos_;   /// ハンドターゲット座標
	Vector3 punchPosition_;

	float waitTine_;          /// 次コンボまでの待機時間

public:
	//コンストラクタ
	ComboAttackSecond(Player* boss);
	~ComboAttackSecond();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;
};
