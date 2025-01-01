#include"UpperCollisionBox.h"
#include<imgui.h>

void UpperCollisionBox::Init() {
	BaseAABBCollisionBox::Init();
}

void UpperCollisionBox::Update() {
	BaseAABBCollisionBox::Update();
}

void UpperCollisionBox::Draw() {
	BaseAABBCollisionBox::Draw();
}


Vector3 UpperCollisionBox::GetCollisionPos() const {
	return BaseAABBCollisionBox::GetCollisionPos();
}

void UpperCollisionBox::SetZeroSizeCollision() {
	BaseAABBCollisionBox::SetZeroSizeCollision();
}

void UpperCollisionBox::SetSize(const Vector3& size) {
	BaseAABBCollisionBox::SetSize(size);
}

void UpperCollisionBox::SetPosition(const Vector3& position) {
	BaseAABBCollisionBox::SetPosition(position);
}

void UpperCollisionBox::SetOffset(const Vector3& offset) {
	BaseAABBCollisionBox::SetOffset(offset);
}

void UpperCollisionBox::IsAdapt(bool is) {
	BaseAABBCollisionBox::IsAdapt(is);
}