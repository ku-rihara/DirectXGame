#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"

class RailManager;
class ComboAttackThird : public BaseComboAattackBehavior {
private:
	enum class Order {
		UPPER,
		BACKPUNCH,
		WAIT,
	};
private:

	/// ===================================================
	///private varians
	/// ===================================================

	Order order_;               /// 振る舞い順序
	RailManager* railManager_;

	/// パンチ
	Easing punchEase_;
	Vector3 lHandStartPos_;    /// ハンドスタート座標
	Vector3 lHandTargetPos_;   /// ハンドターゲット座標
	Vector3 punchPosition_;

	float waitTine_;          /// 次コンボまでの待機時間

public:
	//コンストラクタ
	ComboAttackThird(Player* boss);
	~ComboAttackThird();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;
};
