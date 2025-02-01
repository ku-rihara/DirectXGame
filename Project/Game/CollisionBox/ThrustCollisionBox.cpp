#include"ThrustCollisionBox.h"
#include"Enemy/BaseEnemy.h"
#include"Frame/Frame.h"
#include<imgui.h>

void ThrustCollisionBox::Init() {
	BaseAABBCollisionBox::Init();
}

void ThrustCollisionBox::Update() {

	BaseAABBCollisionBox::Update();
}

void ThrustCollisionBox::Draw() {
	BaseAABBCollisionBox::Draw();
}

Vector3 ThrustCollisionBox::GetCollisionPos() const {
	return BaseAABBCollisionBox::GetCollisionPos();
}

void ThrustCollisionBox::SetSize(const Vector3& size) {
	BaseAABBCollisionBox::SetSize(size);
}

void ThrustCollisionBox::SetPosition(const Vector3& position) {
	BaseAABBCollisionBox::SetPosition(position);
}

void ThrustCollisionBox::SetOffset(const Vector3& offset) {
	BaseAABBCollisionBox::SetOffset(offset);
}

void ThrustCollisionBox::IsAdapt(bool is) {
	BaseAABBCollisionBox::IsAdapt(is);
}

void ThrustCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	
}

void ThrustCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	if (dynamic_cast<BaseEnemy*>(other)) {
		isSlow_ = true;

	}
}