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

Vector3 BaseAABBCollisionBox::GetCollisionPos() const {

	// ワールド座標に変換
	Vector3 worldPos = TransformMatrix(offset_, transform_.matWorld_);
	return worldPos;
}


void BaseAABBCollisionBox::SetSize(const Vector3& size) {
	SetCollisionScale(size);
}

void BaseAABBCollisionBox::SetPosition(const Vector3& position) {
	transform_.translation_ = position;
}

void BaseAABBCollisionBox::SetOffset(const Vector3& offset) {
	offset_ = offset;
}

void BaseAABBCollisionBox::IsAdapt(bool is) {
	SetIsCollision(is);
}

void  BaseAABBCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {

}

void  BaseAABBCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {

}