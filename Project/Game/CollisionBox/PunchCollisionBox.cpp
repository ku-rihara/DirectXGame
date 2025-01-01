#include"PunchCollisionBox.h"
#include<imgui.h>

void PunchCollisionBox::Init() {
	BaseAABBCollisionBox::Init();
}

void PunchCollisionBox::Update() {
	BaseAABBCollisionBox::Update();
}

void PunchCollisionBox::Draw() {
	BaseAABBCollisionBox::Draw();
}

Vector3 PunchCollisionBox::GetCollisionPos() const {
	return BaseAABBCollisionBox::GetCollisionPos();
}

void PunchCollisionBox::SetSize(const Vector3& size) {
	BaseAABBCollisionBox::SetSize(size);
}

void PunchCollisionBox::SetPosition(const Vector3& position) {
	BaseAABBCollisionBox::SetPosition(position);
}

void PunchCollisionBox::SetOffset(const Vector3& offset) {
	BaseAABBCollisionBox::SetOffset(offset);
}

void PunchCollisionBox::IsAdapt(bool is) {
	BaseAABBCollisionBox::IsAdapt(is);
}