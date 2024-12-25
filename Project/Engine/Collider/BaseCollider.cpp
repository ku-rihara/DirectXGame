
#include"Collider/BaseCollider.h"



void BaseCollider::DrawDebugCube(const ViewProjection& viewProjection) {
	
	// AABBのデバッグ表示
	if (cObject3d_) {
		cObject3d_->Draw(cTransform_, viewProjection);
	}
}

Vector3 BaseCollider::GetCollisionPos() const {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = cTransform_.matWorld_.m[3][0];
	worldPos.y = cTransform_.matWorld_.m[3][1];
	worldPos.z = cTransform_.matWorld_.m[3][2];

	return worldPos;
}

void BaseCollider::RevertColor() {
	cObject3d_->objColor_.SetColor(Vector4(1, 1, 1, 1));
}

void BaseCollider::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	cObject3d_->objColor_.SetColor(Vector4(1, 1, 0, 1));
}

void BaseCollider::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	cObject3d_->objColor_.SetColor(Vector4(1, 0, 0, 1));

}
void BaseCollider::OnCollisionExit([[maybe_unused]] BaseCollider* other) {
	cObject3d_->objColor_.SetColor(Vector4(1, 0, 1, 1));

}
