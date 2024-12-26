#include"BaseCollisionBox.h"
#include<imgui.h>

void BaseCollisionBox::Init() {
	transform_.Init();
}
void BaseCollisionBox::Update() {
	
	transform_.UpdateMatrix();
}
void BaseCollisionBox::Draw() {

}

void BaseCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionEnter(other);
}

void BaseCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionStay(other);

}
void BaseCollisionBox::OnCollisionExit([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionExit(other);

}



Vector3 BaseCollisionBox::GetCollisionPos() const {
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = MatrixTransform(offset, transform_.matWorld_);
	return worldPos;
}