#include"EnemyCollisionBox.h"


void EnemyCollisionBox::Init() {
	BaseAABBCollisionBox::Init();
}

void EnemyCollisionBox::Update() {
	BaseAABBCollisionBox::Update();
}

void EnemyCollisionBox::Draw() {
	BaseAABBCollisionBox::Draw();
}

Vector3 EnemyCollisionBox::GetCollisionPos() const {
	return BaseAABBCollisionBox::GetCollisionPos();
}

void EnemyCollisionBox::SetSize(const Vector3& size) {
	BaseAABBCollisionBox::SetSize(size);
}

void EnemyCollisionBox::SetPosition(const Vector3& position) {
	BaseAABBCollisionBox::SetPosition(position);
}

void EnemyCollisionBox::SetOffset(const Vector3& offset) {
	BaseAABBCollisionBox::SetOffset(offset);
}

void EnemyCollisionBox::IsAdapt(bool is) {
	BaseAABBCollisionBox::IsAdapt(is);
}

void EnemyCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {

}


void EnemyCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {

}