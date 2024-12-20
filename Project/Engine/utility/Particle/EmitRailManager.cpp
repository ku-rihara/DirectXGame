#include"EmitRailManager.h"
//std
#include<cmath>
//Function
#include"MathFunction.h"
//imgui
#include<imgui.h>


   ///=====================================================
   /// 初期化
   ///=====================================================
void EmitRailManager::Init(SrvManager* srvManager) {
    viewProjection_.Init();
    worldTransform_.Init();

    worldTransform_.UpdateMatrix();
    viewProjection_.UpdateMatrix();
    // レールの初期化（オブジェクト数を指定）
    rail_.Init(srvManager,5);
   

    /// 現在位置モデル
    obj3D_.reset(Object3d::CreateModel("DebugCube",".obj"));
  /*  obj3D_->Init();*/
}

///=====================================================
///更新
///=====================================================
void EmitRailManager::Update(const std::vector<Vector3>& controlPos,const float&speed) {
    rail_.Update(controlPos);

    // カメラの移動とレールに沿った描画
    railMoveTime_ += speed / rail_.GetTotalLength();
    if (railMoveTime_ >= 1.0f) {
        railMoveTime_ = 0.0f;
    }

    // Y軸のオフセット
    float offsetY = 0.0f; // オフセットの値をここで設定
    Vector3 cameraPos = rail_.GetPositionOnRail(railMoveTime_);

    // ここでオフセットを加算
    cameraPos.y += offsetY;

    // カメラの方向を計算
    const auto& pointsDrawing = rail_.GetPointsDrawing();
    float railProgress = railMoveTime_ * rail_.GetTotalLength();  // レール全体の進行率
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

    // 線形補間で進行中の位置を計算
    float segmentT = (railProgress - traveledLength) / Vector3::Length(pointsDrawing[cameraIndex + 1] - pointsDrawing[cameraIndex]);
    Vector3 interpolatedPos = Lerp(pointsDrawing[cameraIndex], pointsDrawing[cameraIndex + 1], segmentT);

    // interpolatedPosのY成分にもオフセットを加える
    interpolatedPos.y += offsetY;

    //// カメラの進行方向を計算
    //Vector3 forward = pointsDrawing[cameraIndex + 1] - pointsDrawing[cameraIndex];
    //forward = Vector3::Normalize(forward);

    //// 目標回転角度を求めて補間
    //float targetRotateY = std::atan2(forward.x, forward.z);
    //float targetRotateX = std::atan2(-forward.y, std::sqrt(forward.x * forward.x + forward.z * forward.z));

    //// 緩やかな回転補間
    //cameraRotate_.y += (targetRotateY - cameraRotate_.y) * 0.1f;
    //cameraRotate_.x += (targetRotateX - cameraRotate_.x) * 0.1f;

    // 行列の更新
    worldTransform_.matWorld_ = MakeAffineMatrix(scale_, cameraRotate_, interpolatedPos);
  /*  viewProjection_.matView_ = Inverse(worldTransform_.matWorld_);*/
}

///=====================================================
/// 現在位置描画
///=====================================================
void EmitRailManager::Draw(const ViewProjection& viewProjection) {
    obj3D_->Draw(worldTransform_, viewProjection);
}

///=====================================================
/// レール描画
///=====================================================
void EmitRailManager::RailDraw(const ViewProjection& viewProjection) {
    /*rail_.Draw(viewProjection);*/
    viewProjection;
}


///=====================================================
/// WorldPos取得
///=====================================================
Vector3 EmitRailManager::GetWorldPos() const {

    return Vector3(
        worldTransform_.matWorld_.m[3][0], // X成分
        worldTransform_.matWorld_.m[3][1], // Y成分
        worldTransform_.matWorld_.m[3][2]  // Z成分
    );
}

//void EmitRailManager::Debug() {
//#ifdef _DEBUG
//
//    if (ImGui::TreeNode("CameraDebug")) {
//        ImGui::DragFloat("move", &railMoveTime_, 0.001f);
//
//        ImGui::TreePop();
//    }
//#endif
//}