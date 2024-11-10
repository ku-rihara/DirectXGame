#include"BaseEnemy.h"
/// imgui
#include<imgui.h>
/// std
#include<string>
/// class
#include"Player/PlayerBeam.h"

void  BaseEnemy::Init(const std::string& instanceName, const std::string& extension) {
	objct3D_.reset(Object3d::CreateModel(instanceName, extension));
	/*instanceID_ = instanceNum_++;*/
	transform_.Init();
}
//更新
void  BaseEnemy::Update() {
	Debug();
	Move();
	transform_.UpdateMatrix();
}
//描画
void  BaseEnemy::Draw(const ViewProjection& viewProjection) {
	objct3D_->Draw(transform_,viewProjection);
}

void  BaseEnemy::Debug() {
	/*ImGui::Begin("Positions");
	if (ImGui::TreeNode(("Enemy" + std::to_string(instanceID_)).c_str())) {
		ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &transform_.translation_.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();*/
}


Vector3 BaseEnemy::GetBaseCenterPosition() const {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = transform_.matWorld_.m[3][0];
	worldPos.y = transform_.matWorld_.m[3][1];
	worldPos.z = transform_.matWorld_.m[3][2];

	return worldPos;
}

void BaseEnemy::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	if (dynamic_cast<PlayerBeam*>(other)) {
		isDeath_ = true;
	}
}