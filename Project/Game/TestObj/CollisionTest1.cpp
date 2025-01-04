#include"CollisionTest1.h"
#include<imgui.h>

void CollisionTest1::Init() {
	transform_.Init();
	SetIsCollision(false);
}
void CollisionTest1::Update() {
#ifdef _DEBUG

	ImGui::Begin("c1");
	ImGui::DragFloat3("pos", &transform_.translation_.x, 0.1f);
	ImGui::End();
#endif // DEBUG
	transform_.UpdateMatrix();
}
void CollisionTest1::Draw() {

}

void CollisionTest1::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionEnter(other);
}

void CollisionTest1::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionStay(other);

}
void CollisionTest1::OnCollisionExit([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionExit(other);

}



Vector3 CollisionTest1::GetCollisionPos() const {
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = MatrixTransform(offset, transform_.matWorld_);
	return worldPos;
}