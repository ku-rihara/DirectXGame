#include"GameCamera.h"
//std
#include<cmath>
//Function
#include"MathFunction.h"

//Function
void GameCamera::Init() {
	viewProjection_.Init();
	worldTransform_.Init();

	object3d_.resize(IndexCount+1);
	for (uint32_t i = 0; i < uint32_t(object3d_.size()); i++) {
		object3d_[i]=(Object3d::CreateModel("Rail", ".obj"));
	}
}
void GameCamera::Update(const std::vector<Vector3>& controlPos) {

	// オブジェクト用
	for (uint32_t i = 0; i <uint32_t(object3d_.size()); i++) {
		float t = 1.0f / IndexCount * i;
		Vector3 pos = CatmullRomPosition(controlPos, t);
		object3d_[i]->transform_.translation_=pos;
		object3d_[i]->Update();
	}
	
	// 線分の数+1個分の頂点座標を計算
	for (size_t i = 0; i < IndexCount + 1; i++) {
		float t = 1.0f / IndexCount * i;
		Vector3 pos = CatmullRomPosition(controlPos, t);
		// 描画用頂点リストに追加
		pointsDrawing.push_back(pos);
	}

	railMoveTime_ += 0.5f;
	if (RailIndex < IndexCount) {
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
void GameCamera::RailDraw(const ViewProjection& viewProjection) {
	
	for (size_t i = 0; i < object3d_.size(); i++) {
		object3d_[i]->Draw(viewProjection);
	}

}
