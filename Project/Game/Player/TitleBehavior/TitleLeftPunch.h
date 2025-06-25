#pragma once

#include"BaseTitleBehavior.h"
#include"Easing.h"


class TitleLeftPunch : public BaseTitleBehavior {
public:
    // コンストラクタ
    TitleLeftPunch(Player* player);
    ~TitleLeftPunch();

    /// 更新、デバッグ
    void Update() override;
    void Debug() override;
    void EasingInit();

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
    Easingw<Vector3> scalingEase_;
	Easingw<Vector3>  punchEase_;        
    Easingw<Vector3>  backPunchEase_; 

	Vector3 tempScale_;
 
	Vector3 lHandStartPos_;    /// ハンドスタート座標
	Vector3 lHandTargetPos_;   /// ハンドターゲット座標
	Vector3 punchPosition_;    /// パンチ位置

	float waitTine_;          /// 次コンボまでの待機時間


};
