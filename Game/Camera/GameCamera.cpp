#include"GameCamera.h"
//std
#include<cmath>
//Function
#include"MathFunction.h"

//Function
void GameCamera::Init() {
	viewProjection_.Init();
	worldTransform_.Init();
}
void GameCamera::Update(const std::vector<Vector3>& controlPos) {
	
	// 線分の数+1個分の頂点座標を計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPos, t);
		// 描画用頂点リストに追加
		pointsDrawing.push_back(pos);
	}

	railMoveTime_ += 0.5f;
	if (RailIndex < segmentCount) {
		Vector3 eye = pointsDrawing[RailIndex];
		Vector3 target = pointsDrawing[RailIndex + 1];
		Vector3 forward = target - eye;

		cameraRotate_.y = std::atan2(forward.x, forward.z);
		Matrix4x4 cameraRotateMatrix = MakeRotateYMatrix(-cameraRotate_.y);
		Vector3 velocityZ = forward*cameraRotateMatrix;
		cameraRotate_.x = std::atan2(-velocityZ.y, velocityZ.z);

		worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, cameraRotate_, SLerp(eye, target, railMoveTime_));
		viewProjection_.matView_ = Inverse(worldTransform_.matWorld_);
		if (railMoveTime_ >= 1.0f) {
			railMoveTime_ = 0;
			RailIndex++;
		}
	}
}
void GameCamera::ControlSpotDraw() {
	
}
