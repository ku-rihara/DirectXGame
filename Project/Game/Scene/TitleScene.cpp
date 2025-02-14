///scene
#include "TitleScene.h"
#include"Lighrt/Light.h"
#include"JoyState/JoyState.h"
#include"Manager/SceneManager.h"
#include"utility/Particle/ParticleManager.h"
#include"utility/Particle/ParticleCommon.h"
#include"Player/TitleBehavior/TitleSecondPunch.h"

#include "base/TextureManager.h"
#include"Frame/Frame.h"


//math

#include<imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {

}

void TitleScene::Init() {
	//// グローバル変数の読み込み
	GlobalParameter::GetInstance()->LoadFiles();
	BaseScene::Init();
	///=======================================================================================
	/// 生成
	///=======================================================================================

	field_ = std::make_unique<Field>();
	player_ = std::make_unique<Player>();
	skydome_ = std::make_unique<Skydome>();
	titleFont_ = std::make_unique<TitleFont>();


	///=======================================================================================
	/// 初期化
	///=======================================================================================
	field_->Init();
	skydome_->Init();
	player_->Init();
	titleFont_->Init();
		
	alpha_ = 0.0f;
	
	viewProjection_.translation_ = { 7.8f,3.6f,8.3f };
	viewProjection_.rotation_.y= 3.8f;

	shandle_ = TextureManager::GetInstance()->LoadTexture("./resources/Texture/screenChange.png");
	screenSprite_.reset(Sprite::Create(shandle_, Vector2(0, 0), Vector4(1, 1, 1, alpha_)));
	
	player_->SetTitleBehavior();

	player_->SetWorldPositionY(30.0f);
	player_->UpdateMatrix();

	
	/*/// particleD
	damageName_ = "DamageParticle";
	damageEmitter_.reset(ParticleEmitter::CreateParticle(damageName_, "Plane", ".obj", 300, false));
	uint32_t handle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/circle.png");
	damageEmitter_->SetTextureHandle(handle);
	*/
}

void TitleScene::Update() {
	
	
	screenSprite_->SetAlpha(alpha_);

	player_->TitleUpdate();
	field_->Update();
	skydome_->Update();

	if (dynamic_cast<TitleSecondPunch*>(player_->GetTitleBehavior())) {
		if (!isFontUpdate_) {
			isFontUpdate_ = true;
		}
	}
	if (isFontUpdate_) {
		titleFont_->Update();
	}

	ParticleManager::GetInstance()->Update(viewProjection_);

	//　ゲーム遷移
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

	/// commandList取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

	Model::PreDraw(commandList);
	skydome_->Draw(viewProjection_);
	field_->Draw(viewProjection_);
	player_->Draw(viewProjection_);

	ParticleManager::GetInstance()->Draw(viewProjection_);
	Model::PreDraw(commandList);
	player_->EffectDraw(viewProjection_);
}

/// ===================================================
   /// スプライト描画
   /// ===================================================
void TitleScene::SpriteDraw() {
	titleFont_->Draw();

	screenSprite_->Draw();
}

void TitleScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
	titleFont_->Debug();
	Light::GetInstance()->DebugImGui();
	/*player_->AdjustParm();*/
	ImGui::End();


#endif
}


// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {
	BaseScene::ViewProjectionUpdate();
	Light::GetInstance()->SetWorldCameraPos(player_->GetWorldPosition());
}

void TitleScene::ViewProssess() {
	viewProjection_.UpdateMatrix();
}