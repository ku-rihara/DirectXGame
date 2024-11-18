#include "BaseEnemy.h"
/// imgui
#include <imgui.h>
/// std
#include <string>
/// class
#include "Player/PlayerBeam.h"
#include "Easing.h"
#include "Frame/Frame.h"
#include <cmath>

void BaseEnemy::Init(const std::string& instanceName, const std::string& extension) {
    objct3D_.reset(Object3d::CreateModel(instanceName, extension));
    transform_.Init();
    transform_.scale_ = { 0,0,0 };

    transform_.UpdateMatrix();
}

void BaseEnemy::Update() {
#ifdef _DEBUG
    ImGui::Begin("EnemyPositions");
    Debug();
    ImGui::End();
#endif // _DEBUG

  
    // プレイヤー位置と敵位置の差を求める
    Vector3 diff = target_ - transform_.translation_;
    float distance = Vector3::Length(diff);
    Vector3 direction = Vector3::Normalize(diff);

    // Y 軸周りの回転角度を計算して設定
    transform_.rotation_.y = std::atan2(direction.x, direction.z);

    // 距離によって行動
    if (distance <= distanceMax_) {
        isMove_ = true;
    }

    // 出現イージング
    if (isMove_) {
        easeT_ += Frame::DeltaTime();
        if (easeT_ >= easeTMax_) {
            easeT_ = easeTMax_;
        }
        transform_.scale_ = EaseInOutCubic<Vector3>(Vector3{}, Vector3{ 1, 1, 1 }, easeT_, easeTMax_);
        Move();
    }

    // 死亡処理
    if (hp_ <= 0) {
        isDeath_ = true;
    }
    objct3D_->Update();
    transform_.UpdateMatrix();
}

// 描画
void BaseEnemy::Draw(const ViewProjection& viewProjection) {
    objct3D_->Draw(transform_, viewProjection);
}

// Debug
void BaseEnemy::Debug() {
    if (ImGui::TreeNode(("Enemy" + std::to_string(instanceID_)).c_str())) {
        ImGui::DragFloat3(("pos" + std::to_string(instanceID_)).c_str(), &transform_.translation_.x, 0.01f);
        ImGui::TreePop();
    }
}

// ワールド座標の取得
Vector3 BaseEnemy::GetWorldPos() {
    Vector3 worldPos;
    worldPos.x = transform_.matWorld_.m[3][0];
    worldPos.y = transform_.matWorld_.m[3][1];
    worldPos.z = transform_.matWorld_.m[3][2];
    return worldPos;
}

// 衝突処理
void BaseEnemy::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
    if (PlayerBeam* beam = dynamic_cast<PlayerBeam*>(other)) {
        if (isMove_&&beam->GetScale().x>0&&beam->GetIsShot()) {
            hp_--;
            objct3D_->objColor_.SetColor(Vector4(1, 0, 0, 1));
        }      
    }
    else {
        objct3D_->objColor_.SetColor(Vector4(1, 1, 1, 1));
    }
}



Vector3 BaseEnemy::GetBaseCenterPosition() const {
    Vector3 worldPos;
    // ワールド行列の平行移動成分を取得
    worldPos.x = transform_.matWorld_.m[3][0];
    worldPos.y = transform_.matWorld_.m[3][1];
    worldPos.z = transform_.matWorld_.m[3][2];

    return worldPos;
}


void BaseEnemy::SetPos(const Vector3& pos) {
    transform_.translation_ = pos; 
    transform_.UpdateMatrix();
    initialX_ = transform_.translation_.x;


}

void BaseEnemy::SetTarget(const Vector3& pos) {
    target_ = pos;
}