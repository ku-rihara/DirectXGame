#pragma once

#include"BaseTitleBehavior.h"
#include"Easing.h"
#include"CollisionBox/PunchCollisionBox.h"

class TitleSecondPunch : public BaseTitleBehavior {
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

	/// 突進
	Vector3 initPos_;            /// スタート座標
	Vector3 rushPos_;            /// 突進座標
	Vector3 forwardDirection_;   /// 向き
	Easing rushEase_;

	/// パンチ
	Easing  punchEase_;
	Vector3 rHandStartPos_;    /// ハンドスタート座標
	Vector3 rHandTargetPos_;   /// ハンドターゲット座標
	Vector3 punchPosition_;

	
	float waitTine_;          /// 次コンボまでの待機時間

public:
	//コンストラクタ
	TitleSecondPunch(Player* boss);
	~TitleSecondPunch();

	/// 更新、デバッグ
	void Update()override;
	void Debug()override;

};
