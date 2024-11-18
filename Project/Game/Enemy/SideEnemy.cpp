
#include"Enemy/SideEnemy.h"
#include<imgui.h>
#include"Frame/Frame.h"

int  SideEnemy::instanceNum_ = 0;


void  SideEnemy::Init(const std::string& instanceName, const std::string& extension) {
	BaseEnemy::Init(instanceName, extension);
	instanceID_ = instanceNum_ + 1;
	instanceNum_++;
	scoreValue_ = 250;
	hp_ = 1;
	distanceMax_ = 40;
	SetRadiusVector(Vector3(2.5f, 2.5f, 2.5f));
}
//更新
void   SideEnemy::Update() {
	BaseEnemy::Update();
	
}
//描画
void   SideEnemy::Draw(const ViewProjection& viewProjection) {
	objct3D_->Draw(transform_, viewProjection);
}

void   SideEnemy::Debug() {
#ifdef _DEBUG
	if (ImGui::TreeNode(("SideEnemy" + std::to_string(instanceID_)).c_str())) {
		ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &transform_.translation_.x, 0.01f);
		objct3D_->DebugImgui();
		ImGui::TreePop();
	}
#endif
}


void SideEnemy::Move() {
	// サイン波の振幅と周波数を定義
	const float amplitude = 2.0f;     // 振幅（サイン波の高さ）
	const float frequency = 2.0f;     // 周波数（サイン波の速さ）
 
	sideMoveTime_ += Frame::DeltaTime();
	transform_.translation_.x = initialX_+amplitude * std::sin(frequency * sideMoveTime_);

	// 上下移動などを追加したい場合は、他の軸にも同様の計算を適用
	transform_.UpdateMatrix();
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
