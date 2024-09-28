#include "Colider.h"

void Colider::Init() { 
	worldTransform_.Init();
}

void Colider::UpdateWorldTransform() {
	//ワールド座標をワールドトランスフォームに適用
	worldTransform_.translation_ =GetBaseCenterPosition();
	worldTransform_.scale_ = {radius_, radius_, radius_};
	worldTransform_.UpdateMatrix();
}

void Colider::Draw(Model* model, const ViewProjection& viewProjection) {
	//モデルの描画
	model->Draw(worldTransform_,viewProjection);
}