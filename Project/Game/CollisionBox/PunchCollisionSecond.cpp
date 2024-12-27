#include"PunchCollisionSecond.h"
#include<imgui.h>

void PunchCollisionSecond::Init() {
	BaseAABBCollisionBox::Init();
}

void PunchCollisionSecond::Update() {
	BaseAABBCollisionBox::Update();
}

void PunchCollisionSecond::Draw() {
	BaseAABBCollisionBox::Draw();
}


Vector3 PunchCollisionSecond::GetCollisionPos() const {
	return BaseAABBCollisionBox::GetCollisionPos();
}

void PunchCollisionSecond::SetZeroSizeCollision() {
	BaseAABBCollisionBox::SetZeroSizeCollision();
}

void PunchCollisionSecond::SetSize(const Vector3& size) {
	BaseAABBCollisionBox::SetSize(size);
}

void PunchCollisionSecond::SetPosition(const Vector3& position) {
	BaseAABBCollisionBox::SetPosition(position);
}

void PunchCollisionSecond::SetOffset(const Vector3& offset) {
	BaseAABBCollisionBox::SetOffset(offset);
}

void PunchCollisionSecond::IsAdapt(bool is) {
	BaseAABBCollisionBox::IsAdapt(is);
}