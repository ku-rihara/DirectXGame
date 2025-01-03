#include"StopCollisionBox.h"
#include<imgui.h>

void StopCollisionBox::Init() {
	BaseAABBCollisionBox::Init();
}

void StopCollisionBox::Update() {
	BaseAABBCollisionBox::Update();
}

void StopCollisionBox::Draw() {
	BaseAABBCollisionBox::Draw();
}


Vector3 StopCollisionBox::GetCollisionPos() const {
	return BaseAABBCollisionBox::GetCollisionPos();
}

void StopCollisionBox::SetSize(const Vector3& size) {
	BaseAABBCollisionBox::SetSize(size);
}

void StopCollisionBox::SetPosition(const Vector3& position) {
	BaseAABBCollisionBox::SetPosition(position);
}

void StopCollisionBox::SetOffset(const Vector3& offset) {
	BaseAABBCollisionBox::SetOffset(offset);
}

void StopCollisionBox::IsAdapt(bool is) {
	BaseAABBCollisionBox::IsAdapt(is);
}