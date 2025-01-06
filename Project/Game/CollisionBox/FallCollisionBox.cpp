#include"FallCollisionBox.h"
#include<imgui.h>

void FallCollisionBox::Init() {
	BaseAABBCollisionBox::Init();
}

void FallCollisionBox::Update() {
	BaseAABBCollisionBox::Update();
}

void FallCollisionBox::Draw() {
	BaseAABBCollisionBox::Draw();
}

Vector3 FallCollisionBox::GetCollisionPos() const {
	return BaseAABBCollisionBox::GetCollisionPos();
}

void FallCollisionBox::SetSize(const Vector3& size) {
	BaseAABBCollisionBox::SetSize(size);
}

void FallCollisionBox::SetPosition(const Vector3& position) {
	BaseAABBCollisionBox::SetPosition(position);
}

void FallCollisionBox::SetOffset(const Vector3& offset) {
	BaseAABBCollisionBox::SetOffset(offset);
}

void FallCollisionBox::IsAdapt(bool is) {
	BaseAABBCollisionBox::IsAdapt(is);
}