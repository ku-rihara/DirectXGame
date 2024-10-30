#include"GameCamera.h"
//std
#include<cmath>
//Function
#include"MathFunction.h"
//imgui
#include<imgui.h>

void GameCamera::Init() {
    viewProjection_.Init();
    worldTransform_.Init();
    viewProjection_.translation_.y = 3;
    worldTransform_.UpdateMatrix();
    viewProjection_.UpdateMatrix();
    // レールの初期化（オブジェクト数を指定）
    rail_.Init(70);
}

void GameCamera::Update(const std::vector<Vector3>& controlPos) {
    rail_.Update(controlPos);

    // カメラの移動とレールに沿った描画
    railMoveTime_ += railMoveSpeed_ / rail_.GetTotalLength();
    if (railMoveTime_ >= 1.0f) {
        railMoveTime_ = 0.0f;
    }
    Vector3 cameraPos = rail_.GetPositionOnRail(railMoveTime_);
    cameraPos.y += viewProjection_.translation_.y;

    // カメラの方向を計算
    const auto& pointsDrawing = rail_.GetPointsDrawing();
    size_t currentIndex = std::min(static_cast<size_t>(railMoveTime_ * pointsDrawing.size()), pointsDrawing.size() - 2);
    Vector3 forward = pointsDrawing[currentIndex + 1] - pointsDrawing[currentIndex];
    forward = Vector3::Normalize(forward);

    float targetRotateY = std::atan2(forward.x, forward.z);
    float targetRotateX = std::atan2(-forward.y, std::sqrt(forward.x * forward.x + forward.z * forward.z));
    cameraRotate_.y += (targetRotateY - cameraRotate_.y) * 0.1f;
    cameraRotate_.x += (targetRotateX - cameraRotate_.x) * 0.1f;

    worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, cameraRotate_, cameraPos);
    viewProjection_.matView_ = Inverse(worldTransform_.matWorld_);
}

// レール上のオブジェクトを描画
void GameCamera::RailDraw(const ViewProjection& viewProjection) {
    rail_.Draw(viewProjection);
}

void GameCamera::Debug() {
#ifdef _DEBUG
    /* ImGui::Begin("CameraDebug");

     if (ImGui::TreeNode("ViewProjection")) {

         ImGui::DragFloat3("Scale", &worldTransform_.scale_.x, 0.01f);
         ImGui::DragFloat3("Rotate", &worldTransform_.rotation_.x, 0.01f);
         ImGui::DragFloat3("Translate", &worldTransform_.translation_.x, 0.01f);
         ImGui::TreePop();
     }
     ImGui::End();
    */
#endif
}