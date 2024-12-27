#include"BaseAABBCollisionBox.h"
#include<imgui.h>

void BaseAABBCollisionBox::Init() {
	transform_.Init();
	/*SetZeroSizeCollision();*/
}
void BaseAABBCollisionBox::Update() {

	transform_.UpdateMatrix();
}
void BaseAABBCollisionBox::Draw() {

}

void BaseAABBCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionEnter(other);
}

void BaseAABBCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionStay(other);

}
void BaseAABBCollisionBox::OnCollisionExit([[maybe_unused]] BaseCollider* other) {
	BaseCollider::OnCollisionExit(other);
}

Vector3 BaseAABBCollisionBox::GetCollisionPos() const {

	// ワールド座標に変換
	Vector3 worldPos = MatrixTransform(offset_, transform_.matWorld_);
	return worldPos;
}

void BaseAABBCollisionBox::SetZeroSizeCollision() {
	SetCollisionRadius(Vector3{ 0,0,0 });
}
void BaseAABBCollisionBox::SetSize(const Vector3& size) {
	SetCollisionRadius(size);
}

void BaseAABBCollisionBox::SetPosition(const Vector3& position) {
	transform_.translation_ = position;
}

void BaseAABBCollisionBox::SetOffset(const Vector3& offset) {
	offset_ = offset;
}

void BaseAABBCollisionBox::SetIsNotCollision(bool is) {
	SetIsNotCollision(is);
}