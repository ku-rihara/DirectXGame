
/// behavior
#include"EnemyChasePlayer.h"

/// math
#include"MathFunction.h"
/// obj
#include"Enemy/BaseEnemy.h"
#include"Player/Player.h"/// プレイヤーの座標取得に使ってるので消さないこと
/// frame
#include"Frame/Frame.h"
/// imgui
#include<imgui.h>
/// std
#include<cmath> 

//初期化
EnemyChasePlayer::EnemyChasePlayer(BaseEnemy* boss)
	: BaseEnemyBehaivor("EnemyChasePlayer", boss) {
	

	//パラメータ初期化
	chaseSpeedNormal_ = 0.2f;
	waveAttackStartPos_ = 25.0f;/// ボス
	normalAttackStartPos_ = 7.0f;
	attackCoolTime_ = 1.0f;

	isChase_ = true;//	デバッグ用
}

EnemyChasePlayer::~EnemyChasePlayer() {

}

void EnemyChasePlayer::Update() {
	if (!isChase_) return;
		
		// ターゲットへのベクトル
		Vector3 direction =pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());

		// 距離
		distance_ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

		// 近すぎる場合は追従を停止して攻撃
		if (distance_ < 2.0f) {
			// 攻撃開始処理を書くべきところ
			return;
		}

		// 正規化
		direction.y = 0.0f;
		direction.Normalize();

		/// 変位加算
		pBaseEnemy_->AddPosition(direction * (chaseSpeedNormal_ * Frame::DeltaTime()));

		// 目標角度を計算
		float objectiveAngle = std::atan2(-direction.x, -direction.z);

		// 最短角度補間でプレイヤーの回転を更新
		pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetTransform().rotation_.y, objectiveAngle, 0.3f));

}

void EnemyChasePlayer::Debug() {
	 ImGui::Checkbox("isChase", &isChase_);

		ImGui::DragFloat("ChaseSpeedNroaml", &chaseSpeedNormal_, 0.1f);
		ImGui::DragFloat("ChaseSpeedMax", &chaseSpeedMax_, 0.1f);
		ImGui::DragFloat("WaveAttackStartPos", &waveAttackStartPos_, 0.1f);
		ImGui::DragFloat("NormalAttackStartPos", &normalAttackStartPos_, 0.1f);
		ImGui::DragFloat("Distance", &distance_, 0.1f);
		
}
