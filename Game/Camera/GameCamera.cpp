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

    ///　値セット
    viewProjection_.translation_.y = 3;
    worldTransform_.UpdateMatrix();
    viewProjection_.UpdateMatrix();

    //モデルの分割数を設定
	object3d_.resize(40);
	for (uint32_t i = 0; i < uint32_t(object3d_.size()); i++) {
		object3d_[i].reset(Object3d::CreateModel("Rail", ".obj"));
	}
}
void GameCamera::Update(const std::vector<Vector3>& controlPos) {
    // レール全体の長さを計算
    float totalRailLength = 0.0f;
  /*  pointsDrawing.clear();*/
    for (size_t i = 0; i < IndexCount + 1; i++) {
        float t = 1.0f / IndexCount * i;
        Vector3 pos = CatmullRomPosition(controlPos, t);
        pointsDrawing.push_back(pos);

        if (i > 0) {
            totalRailLength += Vector3::Length(pointsDrawing[i] - pointsDrawing[i - 1]);
        }
    }

    // モデルごとの位置を再計算
    float segmentLength = totalRailLength / float(object3d_.size() - 1);
    float currentLength = 0.0f;
    size_t currentIndex = 0;

    // モデルの配置を更新
    for (size_t i = 0; i < object3d_.size(); i++) {
        // 現在の位置に最も近いレール上の点を探す
        while (currentIndex < pointsDrawing.size() - 1 &&
            currentLength + Vector3::Length(pointsDrawing[currentIndex + 1] - pointsDrawing[currentIndex]) < segmentLength * i) {
            currentLength += Vector3::Length(pointsDrawing[currentIndex + 1] - pointsDrawing[currentIndex]);
            currentIndex++;
        }

        // 線形補間で正確な位置を取得
        float t = (segmentLength * i - currentLength) / Vector3::Length(pointsDrawing[currentIndex + 1] - pointsDrawing[currentIndex]);
        Vector3 interpolatedPos = Lerp(pointsDrawing[currentIndex], pointsDrawing[currentIndex + 1], t);

        // オブジェクトの位置を更新
        object3d_[i]->transform_.translation_ = interpolatedPos;
        object3d_[i]->Update();
    }

    // カメラの移動処理（変わらず）
    railMoveTime_ += 0.5f;
    if (RailIndex < IndexCount) {
        Vector3 eye = pointsDrawing[RailIndex];
        Vector3 target = pointsDrawing[RailIndex + 1];
        Vector3 forward = target - eye;

        cameraRotate_.y = std::atan2(forward.x, forward.z);
        Matrix4x4 cameraRotateMatrix = MakeRotateYMatrix(-cameraRotate_.y);
        Vector3 velocityZ = forward * cameraRotateMatrix;
        cameraRotate_.x = std::atan2(-velocityZ.y, velocityZ.z);

        // カメラのY方向の位置を初期設定
        Vector3 cameraPos = SLerp(eye, target, railMoveTime_);
        cameraPos.y += viewProjection_.translation_.y; // Y軸方向のオフセットを反映

        // オブジェクトのワールド行列を更新
        worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, cameraRotate_, cameraPos);
        viewProjection_.matView_ = Inverse(worldTransform_.matWorld_);

        if (railMoveTime_ >= 1.0f) {
            railMoveTime_ = 0;
            RailIndex++;
        }
    }
}
//レール描画
void GameCamera::RailDraw(const ViewProjection& viewProjection) {
	
	for (size_t i = 0; i < object3d_.size(); i++) {
		object3d_[i]->Draw(viewProjection);
	}

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