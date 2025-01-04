#include"CollisionTest2.h"
#include<imgui.h>

void CollisionTest2::Init() {
	transform_.Init();
}
void CollisionTest2::Update() {
#ifdef _DEBUG
	ImGui::Begin("c2");
	ImGui::DragFloat3("pos", &transform_.translation_.x, 0.1f);
	ImGui::End();
#endif
	transform_.UpdateMatrix();
}
void CollisionTest2::Draw() {

}

void CollisionTest2::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionEnter(other);
}

void CollisionTest2::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionStay(other);

}
void CollisionTest2::OnCollisionExit([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionExit(other);

}


Vector3 CollisionTest2::GetCollisionPos() const {
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = MatrixTransform(offset, transform_.matWorld_);
	return worldPos;
}