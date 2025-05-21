/// behavior
#include"TitleLeftPunch.h"
#include"TitleWait.h"

/// objs
#include"Player/Player.h"

/// input
#include"JoyState/JoyState.h"

/// frame
#include"Frame/Frame.h"

//初期化
TitleLeftPunch::TitleLeftPunch(Player* player)
	: BaseTitleBehavior("TitleLeftPunch", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	/// parm
	punchEase_.time = 0.0f;
	waitTine_ = 0.0f;

	
	/// パンチ座標セット
	lHandStartPos_ = pPlayer_->GetLeftHand()->GetTransform().translation_;
	lHandTargetPos_ = pPlayer_->GetLeftHand()->GetTransform().LookAt(Vector3::ToForward()) * 2.0f;

	///land
	startEasing_.maxTime = 0.5f;
	startEasing_.amplitude = 0.6f;
	startEasing_.period = 0.2f;

	// 振る舞い順序初期化
	order_ = Order::PUNCH;
}

TitleLeftPunch::~TitleLeftPunch() {

}

//更新
void TitleLeftPunch::Update() {

	/// スケール変化
	startEasing_.time += Frame::DeltaTimeRate();
	startEasing_.time = std::min(startEasing_.time, startEasing_.maxTime);
	pPlayer_->SetScale(EaseAmplitudeScale(Vector3::UnitVector(), startEasing_.time, startEasing_.maxTime,
		startEasing_.amplitude, startEasing_.period));



	switch (order_) {

	case Order::PUNCH:

		///----------------------------------------------------
		/// パンチ
		///----------------------------------------------------
	
		punchEase_.time += Frame::DeltaTimeRate();

		/// 拳を突き出す
		punchPosition_ =
			EaseInSine(lHandStartPos_, lHandTargetPos_, punchEase_.time, pPlayerParameter_->GetNormalComboParm(SECOND).attackEaseMax);


		// ハンドのローカル座標を更新
		pPlayer_->GetLeftHand()->SetWorldPosition(punchPosition_);

		// イージング終了時の処理
		if (punchEase_.time >= pPlayerParameter_->GetNormalComboParm(SECOND).attackEaseMax) {
			punchEase_.time = pPlayerParameter_->GetNormalComboParm(SECOND).attackEaseMax;
			order_ = Order::BACKPUNCH;
		}
		

		break;

	case Order::BACKPUNCH:
		///----------------------------------------------------
		/// バックパンチ
		///----------------------------------------------------
		
		punchEase_.time -= Frame::DeltaTimeRate();

		punchPosition_ =
			EaseInSine(lHandStartPos_, lHandTargetPos_, punchEase_.time, pPlayerParameter_->GetNormalComboParm(SECOND).attackEaseMax);

		// ハンドのローカル座標を更新
		pPlayer_->GetLeftHand()->SetWorldPosition(punchPosition_);


		// イージング終了時の処理
		if (punchEase_.time <= 0.0f) {
			punchEase_.time = 0.0f;
			order_ = Order::WAIT;
		}
		break;

	case Order::WAIT:
		waitTine_ += Frame::DeltaTime();

		/// コンボ途切れ
		if (waitTine_ >= pPlayerParameter_->GetNormalComboParm(SECOND).waitTime) {
			pPlayer_->ChangeTitleBehavior(std::make_unique<TitleWait>(pPlayer_));
		}

		break;
	}

}

void TitleLeftPunch::Debug() {

}