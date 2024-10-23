#include"Player.h"
#include"ModelManager.h"

void  Player::Init() {
	/// プレイヤーのモデル
	obj3D_.reset(Object3d::CreateModel("suzanne", ".obj"));
	
}
void  Player::Update() {
	obj3D_->Update();
}
void  Player::Draw(const ViewProjection& viewProjection) {
	obj3D_->Draw(viewProjection);
}

//setter
//親子関係を結ぶ
void Player::SetParent(const WorldTransform* parent) { obj3D_->transform_.parent_ = parent; }