#include"BasePunchCollisionBox.h"
#include<imgui.h>

void BasePunchCollisionBox::Init() {
	transform_.Init();
}
void BasePunchCollisionBox::Update() {
	
	transform_.UpdateMatrix();
}
void BasePunchCollisionBox::Draw() {

}

void BasePunchCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionEnter(other);
}

void BasePunchCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionStay(other);

}
void BasePunchCollisionBox::OnCollisionExit([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionExit(other);

}



Vector3 BasePunchCollisionBox::GetCollisionPos() const {
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = MatrixTransform(offset, transform_.matWorld_);
	return worldPos;
}