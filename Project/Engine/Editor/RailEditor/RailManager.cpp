#include"RailManager.h"
//std
#include<cmath>
//Function
#include"MathFunction.h"
//imgui
#include<imgui.h>
#include"Frame/Frame.h"

   ///=====================================================
   /// 初期化
   ///=====================================================
void RailManager::Init(const std::string& groupName) {
    ///グループネーム
    groupName_ = groupName;

    viewProjection_.Init();
    worldTransform_.Init();

    worldTransform_.UpdateMatrix();
    viewProjection_.UpdateMatrix();
    isRoop_ = true;
    // レールの初期化
    rail_.Init(5);
 
    lineDrawer_.reset(Line3D::Create(24));

    /// 制御点マネージャー
    emitControlPosManager_ = std::make_unique<ControlPosManager>();
    emitControlPosManager_->LoadFromFile(groupName_);

}

///===================================================================
///更新
///====================================================================
void RailManager::Update(float speed, const PositionMode& mode, const Vector3& Direction) {
    emitControlPosManager_->Update(Direction);

    if (mode == PositionMode::LOCAL) {
        rail_.Update(emitControlPosManager_->GetLocalPositions());
    }
    else {
        rail_.Update(emitControlPosManager_->GetWorldPositions());
    }

    // カメラの移動とレールに沿った描画
    railMoveTime_ += (speed *Frame::DeltaTimeRate()) / rail_.GetTotalLength();


    RoopOrStop();// ループか止まるか

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
        float segment = (pointsDrawing[i + 1] - pointsDrawing[i]).Length();
        if (traveledLength + segment >= railProgress) {
            cameraIndex = i;
            break;
        }
        traveledLength += segment;
    }

    // 線形補間で進行中の位置を計算
    float segmentT = (railProgress - traveledLength) / (pointsDrawing[cameraIndex + 1] - pointsDrawing[cameraIndex]).Length();
    Vector3 interpolatedPos = Lerp(pointsDrawing[cameraIndex], pointsDrawing[cameraIndex + 1], segmentT);

    // interpolatedPosのY成分にもオフセットを加える
    interpolatedPos.y += offsetY;

    // 行列の更新
    worldTransform_.matWorld_ = MakeAffineMatrix(scale_, cameraRotate_, interpolatedPos);
   
}

void RailManager::RoopOrStop() {
    if (railMoveTime_ < 1.0f) return;

    if (isRoop_) {
        railMoveTime_ = 0.0f;
    }
    else {
        railMoveTime_ = 1.0f;
    }

}

///=====================================================
/// 現在位置描画
///=====================================================
void RailManager::SetCubeLine(const Vector3& size) {
    lineDrawer_->SetCubeWireframe(worldTransform_.GetWorldPos(), size, Vector4::kWHITE());

}


void RailManager::ImGuiEdit() {
    emitControlPosManager_->ImGuiUpdate(groupName_);
}

void RailManager::SetParent(WorldTransform* parent) {
    emitControlPosManager_->SetParent(parent);
}


///=====================================================
/// WorldPos取得
///=====================================================
Vector3 RailManager::GetPositionOnRail() const {
    return  worldTransform_.GetWorldPos();
}

