#include"RushCollisionBox.h"
#include<imgui.h>

void RushCollisionBox::Init() {
	BaseAABBCollisionBox::Init();
}

void RushCollisionBox::Update() {
	BaseAABBCollisionBox::Update();
}

void RushCollisionBox::Draw() {
	BaseAABBCollisionBox::Draw();
}

Vector3 RushCollisionBox::GetCollisionPos() const {
	return BaseAABBCollisionBox::GetCollisionPos();
}

void RushCollisionBox::SetSize(const Vector3& size) {
	BaseAABBCollisionBox::SetSize(size);
}

void RushCollisionBox::SetPosition(const Vector3& position) {
	BaseAABBCollisionBox::SetPosition(position);
}

void RushCollisionBox::SetOffset(const Vector3& offset) {
	BaseAABBCollisionBox::SetOffset(offset);
}

void RushCollisionBox::IsAdapt(bool is) {
	BaseAABBCollisionBox::IsAdapt(is);
}