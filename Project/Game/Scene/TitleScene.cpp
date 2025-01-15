///scene
#include "TitleScene.h"
#include"JoyState/JoyState.h"
#include"Manager/SceneManager.h"
#include"utility/Particle/ParticleManager.h"
#include"utility/Particle/ParticleCommon.h"

#include "base/TextureManager.h"
#include"Frame/Frame.h"
//class
#include"utility/Particle/ParticleManager.h"

//math

#include<imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {

}

void TitleScene::Init() {

	BaseScene::Init();

	handle_ = TextureManager::GetInstance()->LoadTexture("./resources/Texture/Title.png");
	alpha_ = 0.0f;
	titleSprite_.reset(Sprite::Create(handle_, Vector2(0, 0), Vector4(1, 1, 1, 1)));


	shandle_ = TextureManager::GetInstance()->LoadTexture("./resources/Texture/screenChange.png");
	screenSprite_.reset(Sprite::Create(shandle_, Vector2(0, 0), Vector4(1, 1, 1, alpha_)));
	

	/*/// particleD
	damageName_ = "DamageParticle";
	damageEmitter_.reset(ParticleEmitter::CreateParticle(damageName_, "Plane", ".obj", 300, false));
	uint32_t handle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/circle.png");
	damageEmitter_->SetTextureHandle(handle);
	*/
}

void TitleScene::Update() {
	
	/*damageEmitter_->Update();
	damageEmitter_->EditorUpdate();
	damageEmitter_->Emit();*/
	
	screenSprite_->SetAlpha(alpha_);

	//　ジャンプに切り替え
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		isFinished_ = true;
	}
	else {
		ChangeForJoyState();//コントローラジャンプ
	}

	Debug();
	ViewProjectionUpdate();
	

	if (isFinished_) {
		alpha_ += Frame::DeltaTime();
		if (alpha_ >= 1.2f) {
			SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
		}
	}
}

void TitleScene::ChangeForJoyState() {
	if (!(Input::GetInstance()->GetJoystickState(0, joyState))) return;

	if (!((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A))) return;

	isFinished_ = true;

}


/// ===================================================
/// モデル描画
/// ===================================================
void TitleScene::ModelDraw() {

	
}

/// ===================================================
   /// パーティクル描画
   /// ===================================================
void TitleScene::ParticleDraw() {
	
	ParticleManager::GetInstance()->Draw(viewProjection_);
}

/// ===================================================
   /// スプライト描画
   /// ===================================================
void TitleScene::SpriteDraw() {
	titleSprite_->Draw();
	screenSprite_->Draw();
}

void TitleScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
	ImGui::End();
#endif
}


// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {
	BaseScene::ViewProjectionUpdate();
}

void TitleScene::ViewProssess() {
	viewProjection_.UpdateMatrix();
}