#include "Reticle.h"

#include "base/TextureManager.h"
#include <algorithm>
/// math
#include"Matrix4x4.h"
// class
#include"Player/Player.h"
//input 
#include"input/input.h"
#include"JoyState/JoyState.h"

Reticle::Reticle() {}

Reticle::~Reticle() {

}

void Reticle::Init() {
	// レティクル用テクスチャ取得
	textureHandle_ = TextureManager::GetInstance()->LoadTexture("./resources/Texture/anchorPoint.png");
	sprite_.reset(Sprite::Create(textureHandle_, Vector2(640, 320), Vector4(1, 1, 1, 1)));
	sprite_->anchorPoint_ = { 0.5f,0.5f };
}

void Reticle::Updata(const ViewProjection& viewProjection) {
	
	
	Vector2 spritePos = sprite_->GetPosition();
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		spritePos.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * 15.0f;
		spritePos.y -= (float)joyState.Gamepad.sThumbLY / SHRT_MAX * 15.0f;
		sprite_->SetPosition(spritePos);
	} else {
		sprite_->SetPosition(Input::GetMousePos());
	}


	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	// VPV行列
	Matrix4x4 matVPV =viewProjection.matView_* (viewProjection.matProjection_*matViewport);
	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Inverse(matVPV);
	// スクリーン座標
	Vector3 posNear = Vector3(float(sprite_->GetPosition().x), float(sprite_->GetPosition().y), 0);
	Vector3 posFar = Vector3(float(sprite_->GetPosition().x), float(sprite_->GetPosition().y), 1);
	// スクリーンからワールド
	posNear = MatrixTransform(posNear, matInverseVPV);
	posFar = MatrixTransform(posFar, matInverseVPV);

	// マウスレイの方向
	Vector3 mouseDirection = posFar - posNear;
	mouseDirection = Vector3::Normalize(mouseDirection);
	// カメラから標準オブジェクトの距離
	float kDistanceTestObject = 150;
	worldTransform3DReticle_.translation_.x = posNear.x + mouseDirection.x * kDistanceTestObject;
	worldTransform3DReticle_.translation_.y = posNear.y + mouseDirection.y * kDistanceTestObject;
	worldTransform3DReticle_.translation_.z = posNear.z + mouseDirection.z * kDistanceTestObject;
	worldTransform3DReticle_.UpdateMatrix();
}

void Reticle::Draw() {

	sprite_->Draw();
}

Vector3 Reticle::GetWorld3DRecticlPos() {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos.z = worldTransform3DReticle_.matWorld_.m[3][2];
	return worldPos;
}


void Reticle::SetPlayer(Player* player) {
	pPlayer_ = player;
}