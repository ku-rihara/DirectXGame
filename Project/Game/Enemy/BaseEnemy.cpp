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

	distanceMax_ = 5;
}
//更新
void  BaseEnemy::Update(const Vector3& traget) {
	Debug();
	target_ = traget;
	// プレイヤー位置と敵位置の差を求める
	Vector3 diff = target_ - transform_.translation_;
	float distance= Vector3::Length(diff);
	if (distance <= distanceMax_) {
		isMove_ = true;
	}
	/// 行動開始
	if (isMove_) {
		Move();
	}

	/// 死亡
	if (hp_ <= 0) {
		isDeath_ = true;
	}

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
		hp_--;
		objct3D_->color_.SetColor(Vector4(1,0,0,1 ));
	}
	else {
		objct3D_->color_.SetColor(Vector4(1, 1, 1, 1));
	}
}