#include"CollisionTest3.h"
#include<imgui.h>

void CollisionTest3::Init() {
	transform_.Init();
}
void CollisionTest3::Update() {
	ImGui::Begin("c3");
	ImGui::DragFloat3("pos", &transform_.translation_.x, 0.1f);
	ImGui::End();

	transform_.UpdateMatrix();
}
void CollisionTest3::Draw() {

}

void CollisionTest3::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionEnter(other);
}

void CollisionTest3::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionStay(other);

}
void CollisionTest3::OnCollisionExit([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionExit(other);

}


Vector3 CollisionTest3::GetCollisionPos() const {
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = MatrixTransform(offset, transform_.matWorld_);
	return worldPos;
}