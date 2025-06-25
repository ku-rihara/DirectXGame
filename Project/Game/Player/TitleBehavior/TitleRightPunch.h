#pragma once

#include"BaseTitleBehavior.h"
#include"Easing.h"


class TitleRightPunch : public BaseTitleBehavior {
public:
    // コンストラクタ
    TitleRightPunch(Player* player);
    ~TitleRightPunch();

    /// 更新、デバッグ
    void Update() override;
    void Debug() override;
    void EasingInit();

private:
	enum class Order{
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
	Vector3 rHandStartPos_;    /// ハンドスタート座標
	Vector3 rHandTargetPos_;   /// ハンドターゲット座標
	Vector3 punchPosition_;

	Easingw<Vector3> scalingEase_;
    Easingw<Vector3> punchEase_;
    Easingw<Vector3> backPunchEase_;

    Vector3 tempScale_;
	
	float waitTine_;          /// 次コンボまでの待機時間


};
