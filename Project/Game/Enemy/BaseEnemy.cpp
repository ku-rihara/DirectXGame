#include"BaseEnemy.h"
/// imgui
#include<imgui.h>
/// std
#include<string>
/// class
#include"Player/PlayerBullet.h"
int  BaseEnemy::instanceNum_ = 0;
void  BaseEnemy::Init() {
	objct3D_.reset(Object3d::CreateModel("axis", ".obj"));
	instanceID_ = instanceNum_++;
}
//更新
void  BaseEnemy::Update() {
	objct3D_->Update();
	Debug();
	transform_.UpdateMatrix();
}
//描画
void  BaseEnemy::Draw(const ViewProjection& viewProjection) {
	objct3D_->Draw(transform_,viewProjection);
}

void  BaseEnemy::Debug() {
	ImGui::Begin("Positions");
	if (ImGui::TreeNode(("Enemy" + std::to_string(instanceID_)).c_str())) {
		ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &transform_.translation_.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();
}


Vector3 BaseEnemy::GetBaseCenterPosition() const {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = transform_.matWorld_.m[3][0];
	worldPos.y = transform_.matWorld_.m[3][1];
	worldPos.z = transform_.matWorld_.m[3][2];

	return worldPos;
}

void BaseEnemy::OnCollisionEnter([[maybe_unused]] Collider* other) {
	if (dynamic_cast<PlayerBullet*>(other)) {
		isDeath_ = true;
	}
}