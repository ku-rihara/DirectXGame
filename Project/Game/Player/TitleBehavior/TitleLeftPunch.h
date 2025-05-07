#pragma once

#include"BaseTitleBehavior.h"
#include"Easing.h"


class TitleLeftPunch : public BaseTitleBehavior {
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
	
	/// パンチ
	Easing punchEase_;         /// パンチイージング
	Vector3 lHandStartPos_;    /// ハンドスタート座標
	Vector3 lHandTargetPos_;   /// ハンドターゲット座標
	Vector3 punchPosition_;    /// パンチ位置

	float waitTine_;          /// 次コンボまでの待機時間

public:

	//コンストラクタ
	TitleLeftPunch(Player* boss);
	~TitleLeftPunch();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;


};
