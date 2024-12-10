/// behavior
#include"PlayerRoot.h"
#include"PlayerCompress.h"

/// boss
#include"Game/Player/Player.h"
#include"Game/Boss/Boss.h"
/// frame
#include"myEngine/Frame/Frame.h"
/// inupt
#include"myEngine/input/Input.h"

//初期化
PlayerCompress::PlayerCompress(Player* boss)
	: BasePlayerBehavior("PlayerCompress", boss) {

	/// ===================================================
	///変数初期化
	/// ===================================================
	
	patern_ = Patern::COMPRESS;
	// coll
	kCollTime_ = 0.5f;
	collTime_ = kCollTime_;

	ease_.time = 0.0f;
	ease_.maxTime = 0.5f;
	ease_.amplitude = 0.7f;
	ease_.period = 0.1f;

}

PlayerCompress ::~PlayerCompress() {

}

//更新
void PlayerCompress::Update() {

	//つぶれる処理
	switch (patern_)
	{
	case Patern::COMPRESS:
		pPlayer_->SetScale(Vector3(1.0f, 0.1f, 1.0f));
		pPlayer_->SetWorldPositionY(pPlayer_->GetBoss()->GetWorldPosition().y - 1);

		if (pPlayer_->GetBoss()->GetIsPressed()) {
			patern_ = Patern::COLLTIME;
			pPlayer_->GetBoss()->SetIsPressed(false);
		}
		break;

	case Patern::COLLTIME:
		collTime_ -= Frame::DeltaTime();
		pPlayer_->SetWorldPositionY(Player::InitY_);
		if (collTime_ <= 0.0f) {
			patern_ = Patern::REVERSE;
		}

		break;

	case Patern::REVERSE:
		pPlayer_->Move(0.3f);
		ease_.time += Frame::DeltaTime();
		if (ease_.time >= ease_.maxTime) {
			patern_ = Patern::CHANGEROOT;
		}
		/// 弾発射モーション
		pPlayer_->SetScale(EaseAmplitudeScale(Vector3(1, 1, 1),
			ease_.time, ease_.maxTime,
			ease_.amplitude, ease_.period));

		break;

	case Patern::CHANGEROOT:
		pPlayer_->ChangeBehavior(std::make_unique<PlayerRoot>(pPlayer_));
		break;
	default:
		break;
	}
	
}


void  PlayerCompress::Debug() {
	ImGui::DragFloat("EaseTMax", &ease_.maxTime, 0.1f);
	ImGui::DragFloat("Amplitude", &ease_.amplitude, 0.1f);
	ImGui::DragFloat("Period", &ease_.period, 0.1f);
	
}