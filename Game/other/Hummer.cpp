#include "Hummer.h"
#include"Easing.h"
#include"Pi.h"
#include"CollisionTypeIdDef.h"
#include<imgui.h>
//class
#include"Enemy.h"
#include"GameScene.h"

Hummer::Hummer() {}

void Hummer::SetParent(const WorldTransform& worldTransform) {
	worldTransform_.parent_ = &worldTransform;
}

void Hummer::Init(Model* models) {

	worldTransform_.Init();
	model_ = models;
	Colider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));
	// コライダー初期化
	Colider::Init();
}

void Hummer::Update() {
	ImGui::Begin("Hummer");
	ImGui::Text("effectNum:%d", (int)effects_.size());
	ImGui::End();
	HitEffectUpdate();
	worldTransform_.UpdateMatrix();
}

void Hummer::Attack(float easeT) {
	worldTransform_.rotation_.x = Lerp(-pi / 3, pi / 2, easeT);
}

void Hummer::Draw(const ViewProjection& viewProjection) {
	// 各エフェクトを更新
	effects_.reverse();
	for (std::unique_ptr<Effect>& effect : effects_) {
		if (effect) {
			effect->Draw(viewProjection);
		}
	}
	effects_.reverse();
	model_->Draw(worldTransform_, viewProjection);
}


Vector3 Hummer::GetBaseCenterPosition() const {
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 9.5f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transform(offset, worldTransform_.matWorld_);
	return worldPos;
}

void Hummer::OnCollision([[maybe_unused]] Colider* other) {

	//衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
		Enemy* enemy = static_cast<Enemy*>(other);
		uint32_t serialNum = enemy->GetSerialNum();
		//接触履歴があるなら何もせず抜ける
		if (collisionRecord_.CheckHistory(serialNum)) {
			return;
		}
		//接触履歴に登録
		collisionRecord_.AddHistory(serialNum);
		//エフェクト初期化
		HitEffectInit(enemy->GetCenterPos());
	}
}



void Hummer::HitEffectInit(const Vector3& pos) {
	std::unique_ptr<Effect> effect = std::make_unique<Effect>();

	effect->Init(GameScene::modelEffect_.get(), pos);

	effects_.push_back(std::move(effect));
}

void Hummer::HitEffectUpdate() {
	// 各エフェクトを更新
	for (std::unique_ptr<Effect>& effect : effects_) {
		if (effect) {
			effect->Update();
		}
	}
	// 完了したエフェクトを消す
	effects_.erase(std::remove_if(effects_.begin(), effects_.end(), [](const std::unique_ptr<Effect>& effect) { return effect->IsFinished(); }), effects_.end());
}

//履歴抹消
void Hummer::HistoryClear() {
	collisionRecord_.Clear();
}