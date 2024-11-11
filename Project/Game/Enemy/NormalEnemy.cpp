
#include"Enemy/NormalEnemy.h"
#include<imgui.h>
#include"Frame/Frame.h"

int  NormalEnemy::instanceNum_ = 0;


void   NormalEnemy::Init(const std::string& instanceName, const std::string& extension) {
	BaseEnemy::Init(instanceName, extension);
	instanceID_ = instanceNum_ + 1;
	instanceNum_++;
	scoreValue_ = 3;
	hp_ = 50;
	SetRadiusVector(Vector3(2.0f, 2.0f, 2.0f));
}
//更新
void   NormalEnemy::Update() {
	Debug();
	transform_.UpdateMatrix();
}
//描画
void   NormalEnemy::Draw(const ViewProjection& viewProjection) {
	objct3D_->Draw(transform_, viewProjection);
}

void   NormalEnemy::Debug() {
	ImGui::Begin("Positions");
	if (ImGui::TreeNode(("Enemy" + std::to_string(instanceID_)).c_str())) {
		ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &transform_.translation_.x, 0.01f);
		objct3D_->DebugImgui();
		ImGui::TreePop();
	}
	ImGui::End();
}


void NormalEnemy::Move() {
	transform_.translation_.y += 5.0f * Frame::DeltaTime();
}

//Vector3 BaseEnemy::GetBaseCenterPosition() const {
//	Vector3 worldPos;
//	// ワールド行列の平行移動成分を取得
//	worldPos.x = transform_.matWorld_.m[3][0];
//	worldPos.y = transform_.matWorld_.m[3][1];
//	worldPos.z = transform_.matWorld_.m[3][2];
//
//	return worldPos;
//}
//
