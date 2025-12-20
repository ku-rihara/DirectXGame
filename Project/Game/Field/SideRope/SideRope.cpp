#include "SideRope.h"
#include <imgui.h>

void SideRope::Init(int32_t type) {

    // タイプ設定
    type_      = static_cast<SideRopeType>(type);
    groupName_ = SetGroupNameByType();

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void SideRope::Update() {
    // 衝突サイズ、位置設定
    SetCollisionScale(collisionSize_);
    transform_.translation_ = collisionPos_;
    transform_.UpdateMatrix();
}

void SideRope::Rebound(BaseEnemy* enemy, const Vector3& velocity) {
    if (!enemy) {
        return;
    }

    // ロープの法線方向を計算
    Vector3 ropeNormal=Cal
}

Vector3 SideRope::CalculateRopeNormal() const {
    switch (type_) {
    case SideRopeType::FRONT:
        return Vector3::ToForward();
    
    case SideRopeType::LEFT:
      
    case SideRopeType::RIGHT:
      
    case SideRopeType::BACK:
       
    case SideRopeType::COUNT:
       
   
    }
  }

Vector3 SideRope::GetCollisionPos() const {

    // ワールド座標に変換
    Vector3 worldPos = TransformMatrix(Vector3::ZeroVector(), transform_.matWorld_);
    return worldPos;
}

void SideRope::RegisterParams() {
    globalParameter_->Regist<Vector3>(groupName_, "CollisionSize", &collisionSize_);
    globalParameter_->Regist<Vector3>(groupName_, "CollisionPos", &collisionPos_);
}

void SideRope::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        /// Collision
        ImGui::SeparatorText("Collision");
        ImGui::DragFloat3("Pos", &collisionPos_.x, 0.1f);
        ImGui::DragFloat3("Size", &collisionSize_.x, 0.1f);

        /// セーブとロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

#endif // _DEBUG
}

std::string SideRope::SetGroupNameByType() {
    switch (type_) {
    case SideRopeType::FRONT:
        return "FrontRope";
    case SideRopeType::LEFT:
        return "LeftRope";
    case SideRopeType::RIGHT:
        return "RightRope";
    case SideRopeType::BACK:
        return "BackRope";
    }

    return "None";
}