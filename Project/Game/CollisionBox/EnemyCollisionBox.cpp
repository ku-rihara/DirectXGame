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

void EnemyCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {

}


void EnemyCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {

}