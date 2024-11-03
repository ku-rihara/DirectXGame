
#include"Colider/BaseCollider.h"



void BaseCollider::DrawDebugCube(const ViewProjection& viewProjection) {
	// AABBのデバッグ表示
	if (object3d_) {
		object3d_->Draw(transform_, viewProjection);
	}
}

Vector3 BaseCollider::GetBaseCenterPosition() const {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = transform_.matWorld_.m[3][0];
	worldPos.y = transform_.matWorld_.m[3][1];
	worldPos.z = transform_.matWorld_.m[3][2];

	return worldPos;
}

