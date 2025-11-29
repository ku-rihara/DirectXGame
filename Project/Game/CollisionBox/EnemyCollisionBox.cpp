#include"EnemyCollisionBox.h"


void EnemyCollisionBox::Init() {
	BaseAABBCollisionBox::Init();
}

void EnemyCollisionBox::Update(const float& timeSpeed) {
    BaseAABBCollisionBox::Update(timeSpeed);
}


Vector3 EnemyCollisionBox::GetCollisionPos() const {
	return BaseAABBCollisionBox::GetCollisionPos();
}

void EnemyCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {

}


void EnemyCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {

}