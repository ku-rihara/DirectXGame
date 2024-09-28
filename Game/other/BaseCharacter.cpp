#include "BaseCharacter.h"

void BaseCharacter::Init(const std::vector<Model*>& models) {
	models_ = models; // モデル
	// ベースとなるワールドトランスフォームの初期化
	baseWorldTransform_.Init();
	// 各パーツのワールドトランスフォームの初期化
	for (auto& worldTransform : partsWorldTransforms_) {
		worldTransform->Init();
	}
	//コライダー初期化
	Colider::Init();
}

void BaseCharacter::Update() {
	baseWorldTransform_.UpdateMatrix();
	for (auto& worldTransform : partsWorldTransforms_) {
		worldTransform->UpdateMatrix();
	}
}

void BaseCharacter::Draw(const ViewProjection& viewProjecion) {
	// モデルの個数分配列にして描画する
	for (int i = 0; i < models_.size(); i++) {
		models_[i]->Draw(*partsWorldTransforms_[i].get(), viewProjecion);
	}
}

Vector3 BaseCharacter::GetBaseCenterPosition()const  {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = baseWorldTransform_.matWorld_.m[3][0];
	worldPos.y = baseWorldTransform_.matWorld_.m[3][1];
	worldPos.z = baseWorldTransform_.matWorld_.m[3][2];

	return worldPos;
}