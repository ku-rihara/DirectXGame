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
	object3d_.resize(70);
    transform_.resize(70);
	for (uint32_t i = 0; i < uint32_t(object3d_.size()); i++) {
		object3d_[i].reset(Object3d::CreateModel("Rail", ".obj"));
	}
}
void GameCamera::Update(const std::vector<Vector3>& controlPos) {
    /// レール全体の長さを初期化
    float totalRailLength = 0.0f;
    pointsDrawing.clear();

    /// 制御点を補間する点を追加
    for (size_t i = 0; i < IndexCount + 1; i++) {
        float t = 1.0f / IndexCount * i;
        Vector3 pos = CatmullRomPosition(controlPos, t);
        pointsDrawing.push_back(pos);/// リスト登録

        if (i > 0) {/// 点と点との長さを加算していく
            totalRailLength += Vector3::Length(pointsDrawing[i] - pointsDrawing[i - 1]);
        }
    }

    // モデルごとの位置を再計算
    float segmentLength = totalRailLength / float(object3d_.size() - 1);
    float currentLength = 0.0f;
    size_t currentIndex = 0;

    for (size_t i = 0; i < object3d_.size(); i++) {
        while (currentIndex < pointsDrawing.size() - 1 &&
            currentLength + Vector3::Length(pointsDrawing[currentIndex + 1] - pointsDrawing[currentIndex]) < segmentLength * i) {
            currentLength += Vector3::Length(pointsDrawing[currentIndex + 1] - pointsDrawing[currentIndex]);
            currentIndex++;
        }

        float t = (segmentLength * i - currentLength) / Vector3::Length(pointsDrawing[currentIndex + 1] - pointsDrawing[currentIndex]);
        Vector3 interpolatedPos = Lerp(pointsDrawing[currentIndex], pointsDrawing[currentIndex + 1], t);

        Vector3 direction = pointsDrawing[currentIndex + 1] - interpolatedPos;
        direction = Vector3::Normalize(direction);

        float rotateY = std::atan2(direction.x, direction.z);
        float rotateX = std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z));

        transform_[i].translation_ = interpolatedPos;
        transform_[i].rotation_ = { rotateX, rotateY, 0.0f };
        transform_[i].UpdateMatrix();
    }

    // レール全体の移動速度を設定
   
    railMoveTime_ += railMoveSpeed_ / totalRailLength;

    if (railMoveTime_ >= 1.0f) {
        railMoveTime_ = 0.0f;
    }

    // カメラの移動
    float railProgress = railMoveTime_ * totalRailLength;
    float traveledLength = 0.0f;
    size_t cameraIndex = 0;

    for (size_t i = 0; i < pointsDrawing.size() - 1; i++) {
        float segment = Vector3::Length(pointsDrawing[i + 1] - pointsDrawing[i]);
        if (traveledLength + segment >= railProgress) {
            cameraIndex = i;
            break;
        }
        traveledLength += segment;
    }

    float segmentT = (railProgress - traveledLength) / Vector3::Length(pointsDrawing[cameraIndex + 1] - pointsDrawing[cameraIndex]);
    Vector3 cameraPos = Lerp(pointsDrawing[cameraIndex], pointsDrawing[cameraIndex + 1], segmentT);
    cameraPos.y += viewProjection_.translation_.y;

    // カメラの方向設定を修正
    Vector3 forward = pointsDrawing[cameraIndex + 1] - pointsDrawing[cameraIndex];
    forward = Vector3::Normalize(forward);

  // スムーズな回転補間 (Slerp) を使用
    float targetRotateY = std::atan2(forward.x, forward.z);
    float targetRotateX = std::atan2(-forward.y, std::sqrt(forward.x * forward.x + forward.z * forward.z));
    
    // 角度の補間 (滑らかにするため、現在角度と目標角度の間をSlerp)
    cameraRotate_.y = cameraRotate_.y + (targetRotateY - cameraRotate_.y) * 0.1f; // 補間率は調整可
    cameraRotate_.x = cameraRotate_.x + (targetRotateX - cameraRotate_.x) * 0.1f;

    // ワールド行列とビュー行列を更新
    worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, cameraRotate_, cameraPos);
    viewProjection_.matView_ = Inverse(worldTransform_.matWorld_);
}



//レール描画
void GameCamera::RailDraw(const ViewProjection& viewProjection) {
	
	for (size_t i = 0; i < object3d_.size(); i++) {
		object3d_[i]->Draw(transform_[i], viewProjection);
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