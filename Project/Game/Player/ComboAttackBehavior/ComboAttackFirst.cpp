/// behavior
#include"ComboAttackFirst.h"
#include"ComboAttackSecond.h"
#include"ComboAttackRoot.h"

/// objs
#include"Player/Player.h"
#include"LockOn/LockOn.h"

/// input
#include"input/Input.h"

/// frame
#include"Frame/Frame.h"

#include<imgui.h>

//初期化
ComboAttackFirst::ComboAttackFirst(Player* player)
	: BaseComboAattackBehavior("ComboAttackFirst", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	///　突進の動き
	initPos_ = pPlayer_->GetWorldPosition();
	forwardDirection_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());

	/// collisionBox
	collisionBox_ = std::make_unique<PunchCollisionBox>();
	collisionBox_->Init();
	collisionBox_->SetSize(Vector3::UnitVector()*2.5f);// 当たり判定サイズ
	collisionBox_->IsAdapt(false);
	/// parm
	rushEase_.time = 0.0f;
	punchEase_.time = 0.0f;
	speed_ = pPlayer_->GetRushDistance();
	waitTine_ = 0.0f;
	
	///land
	startEasing_.maxTime = 0.5f;
	startEasing_.amplitude = 0.6f;
	startEasing_.period = 0.2f;

	// 振る舞い順序初期化
	order_ = Order::RUSH;
}

ComboAttackFirst::~ComboAttackFirst() {

}

//更新
void ComboAttackFirst::Update() {
	/// スケール変化
	startEasing_.time += Frame::DeltaTimeRate();
	startEasing_.time = std::min(startEasing_.time, startEasing_.maxTime);
	pPlayer_->SetScale(EaseAmplitudeScale(Vector3::UnitVector(), startEasing_.time, startEasing_.maxTime,
		startEasing_.amplitude, startEasing_.period));

	pPlayer_->Move(0.01f);

	switch (order_) {

	case Order::RUSH:

		
		///----------------------------------------------------
		/// 突進
		///----------------------------------------------------

		ChangeSpeedForLockOn();// ロックオンによる突進スピードの変化

		rushPos_ = initPos_ + (forwardDirection_ * speed_); // 突進座標を決める

		rushEase_.time += Frame::DeltaTimeRate();

		// 突進の動き
		pPlayer_->SetWorldPosition(
			EaseInSine(initPos_, rushPos_, rushEase_.time, pPlayer_->GetRushEaseMax()));

		/// パンチオーダーに移行
		if (rushEase_.time >= pPlayer_->GetRushEaseMax()) {
			rushEase_.time = pPlayer_->GetRushEaseMax();

			/// パンチ座標セット
			rHandStartPos_ = pPlayer_->GetRightHand()->GetTransform().translation_;
			rHandTargetPos_ = pPlayer_->GetRightHand()->GetTransform().LookAt(Vector3::ToForward()) * pPlayer_->GetPunchReach(Player::FIRST);

			order_ = Order::PUNCH;
		}

		break;

	case Order::PUNCH:
		///----------------------------------------------------
		/// パンチ
		///----------------------------------------------------

		collisionBox_->IsAdapt(true);
		
		punchEase_.time += Frame::DeltaTimeRate();

		/// 拳を突き出す
		punchPosition_ =
			EaseInSine(rHandStartPos_, rHandTargetPos_, punchEase_.time, pPlayer_->GetPunchEaseMax(Player::FIRST));


		// ハンドのローカル座標を更新
		pPlayer_->GetRightHand()->SetWorldPosition(punchPosition_);

		// イージング終了時の処理
		if (punchEase_.time >= pPlayer_->GetPunchEaseMax(Player::FIRST)) {
			punchEase_.time = pPlayer_->GetPunchEaseMax(Player::FIRST);
			order_ = Order::BACKPUNCH;
		}

		/// 当たり判定座標
		collisionBox_->SetPosition(pPlayer_->GetRightHand()->GetWorldPosition());
		collisionBox_->SetOffset(forwardDirection_ * 1.0f);
		collisionBox_->Update();


		break;

	case Order::BACKPUNCH:
		///----------------------------------------------------
		/// バックパンチ
		///----------------------------------------------------
		BaseComboAattackBehavior::ChangeNextComboFragForButton();

		collisionBox_->IsAdapt(false);

		punchEase_.time -= Frame::DeltaTimeRate();

		punchPosition_ =
			EaseInSine(rHandStartPos_, rHandTargetPos_, punchEase_.time, pPlayer_->GetPunchEaseMax(Player::FIRST));

		// ハンドのローカル座標を更新
		pPlayer_->GetRightHand()->SetWorldPosition(punchPosition_);

		// イージング終了時の処理
		if (punchEase_.time <= 0.0f) {
			punchEase_.time = 0.0f;
			order_ = Order::WAIT;
		}
		break;

	case Order::WAIT:
		waitTine_ += Frame::DeltaTime();

		/// コンボ途切れ
		if (waitTine_ >= pPlayer_->GetWaitTime(Player::FIRST)) {
			pPlayer_->ChangeComboBehavior
			(std::make_unique<ComboAttackRoot>(pPlayer_));
		}
		else {
			/// ボタンで次のコンボ
		BaseComboAattackBehavior::ChangeNextComboFragForButton();
		BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<ComboAttackSecond>(pPlayer_));
		}
	}
}

void ComboAttackFirst::ChangeSpeedForLockOn() {
	// ターゲット追い越し防止
	if (!(pPlayer_->GetLockOn() &&
		pPlayer_->GetLockOn()->GetEnemyTarget())) {
		return;
	}

	Vector3 differectialVector = pPlayer_->GetLockOn()->GetTargetPosition() - pPlayer_->GetWorldPosition();
	// 距離
	float distance = differectialVector.Length();
	// 距離しきい値
	const float threshold = 1.2f;

	// しきい値より離れていない場合は終了
	if (distance <= threshold) {
		return;
	}

	// Y軸回り角度
	pPlayer_->SetRotationY(std::atan2(differectialVector.x, differectialVector.z));
	// しきい値を超える速さなら補正する
	if (speed_ > distance - threshold) {
		speed_ = distance - threshold;
	}
}


void  ComboAttackFirst::Debug() {
	ImGui::Text("ComboAttackFirst");
}

