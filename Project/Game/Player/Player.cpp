#include"Player.h"
#include"Frame/Frame.h"

void Player::Init() {
	obj3d_.reset(Object3d::CreateModel("Cube", ".obj"));
	transform_.Init();
	speed_ = 10.0f;
}
void Player::Update() {
	transform_.UpdateMatrix();
}
void Player::Draw(const ViewProjection& viewPro) {
	obj3d_->Draw(transform_, viewPro);
}


void Player::MoveRight() {
	this->transform_.translation_.x += this->speed_ * Frame::DeltaTime();
}
void Player::MoveLeft() {
	this->transform_.translation_.x -= this->speed_ * Frame::DeltaTime();
}