#include"Player.h"

void  Player::Init() {

}
void  Player::Update() {
	obj3D_->Update();
}
void  Player::Draw(const ViewProjection&viewProjection) {
	obj3D_->Draw(viewProjection);
}