/// behavior
#include"ComboAttackJumpSecond.h"
#include"ComboAttackRoot.h"

/// Player
#include"Player/Player.h"

#include<imgui.h>
#include"Frame/Frame.h"

//初期化
ComboAttackJumpSecond::ComboAttackJumpSecond(Player* player)
	: BaseComboAattackBehavior("ComboAttackJumpSecond", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------
	handMoveEasing_.maxTime = 0.2f;

	///初期化座標とターゲット座標
	initRHandPos_ = pPlayer_->GetRightHand()->GetTransform().translation_;
	initLHandPos_ = pPlayer_->GetLeftHand()->GetTransform().translation_;
	targetRPos_ = initRHandPos_ +(Vector3::ToForward()*2.0f);
	targetLPos_ = initLHandPos_ +(Vector3::ToForward()*2.0f);

	collisionBox_ = std::make_unique<AttackCollisionBox>();
	collisionBox_->Init();
	collisionBox_->attackType_ = AttackCollisionBox::AttackType::RUSH;
	collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
	collisionBox_->SetSize(Vector3(2.0f, 2.0f, 2.0f));// 当たり判定サイズ
	collisionBox_->Update();
	collisionBox_->IsAdapt(false);

	pPlayer_->FaceToTarget();
	initPos_ = pPlayer_->GetWorldPosition();
	direction_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
	rushTargetPos_ = initPos_ + (direction_ * pPlayer_->GetJumpComboParm(Player::ComboNum::SECOND).attackReach);
	
	rushEaseTime_ = 0.0f;

	emitter_.reset(ParticleEmitter::CreateParticle("rushParticle", "Plane", ".obj", 800));
	emitter_->SetTextureHandle(pPlayer_->GetCircleTexture());


	step_ = STEP::RUSH; // 突進

}

ComboAttackJumpSecond::~ComboAttackJumpSecond() {

}

//更新
void ComboAttackJumpSecond::Update() {
	switch (step_)
	{

	case STEP::RUSH:
		///---------------------------------------------------------
		/// 着地
		///---------------------------------------------------------
		emitter_->SetTargetPosition(pPlayer_->GetWorldPosition());
		emitter_->Update();
		emitter_->EditorUpdate();
		emitter_->Emit();

		handMoveEasing_.time += Frame::DeltaTimeRate();
		rushEaseTime_ += Frame::DeltaTimeRate();

		/// ハンド
		handMoveEasing_.time = std::min(handMoveEasing_.time, handMoveEasing_.maxTime);

		pPlayer_->GetRightHand()->SetWorldPosition(
			EaseInExpo(initRHandPos_,targetRPos_, handMoveEasing_.time, handMoveEasing_.maxTime)
		);

		pPlayer_->GetLeftHand()->SetWorldPosition(
			EaseInExpo(initLHandPos_, targetLPos_, handMoveEasing_.time, handMoveEasing_.maxTime)
		);

		/// 突進
		pPlayer_->SetWorldPosition(
			EaseOutQuart(initPos_, rushTargetPos_, rushEaseTime_, pPlayer_->GetJumpComboParm(Player::ComboNum::SECOND).attackEaseMax)
		);

		/// 当たり判定座標
		collisionBox_->IsAdapt(true);
		collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
		collisionBox_->Update();

		// 早期break
		if (rushEaseTime_ < pPlayer_->GetJumpComboParm(Player::ComboNum::SECOND).attackEaseMax)break;
		pPlayer_->GetRightHand()->SetWorldPosition(initRHandPos_);
		pPlayer_->GetLeftHand()->SetWorldPosition(initLHandPos_);
		step_ = STEP::WAIT;

		break;
	case STEP::WAIT:
		///---------------------------------------------------------
		/// 待機
		///---------------------------------------------------------
		collisionBox_->IsAdapt(false);
		waitTime_ += Frame::DeltaTime();
		if (waitTime_ < pPlayer_->GetJumpComboParm(Player::ComboNum::SECOND).waitTime)break;
		step_ = STEP::RETURNROOT;
		break;

	case STEP::RETURNROOT:
		pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
		break;

	default:
		break;
	}

}


void  ComboAttackJumpSecond::Debug() {
	ImGui::Text("ComboAttackJumpSecond");
}