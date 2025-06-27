///scene
#include "TitleScene.h"
#include"Lighrt/Light.h"
#include"JoyState/JoyState.h"
#include"Manager/SceneManager.h"
#include"utility/ParticleEditor/ParticleManager.h"
#include"utility/ParticleEditor/ParticleCommon.h"
#include"Player/TitleBehavior/TitleRightPunch.h"

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
    titleRogo_ = std::make_unique<TitleRogo>();
    skyBox_ = std::make_unique<SkyBox>();

	///=======================================================================================
	/// 初期化
	///=======================================================================================
	field_->Init();
	skydome_->Init();
    skyBox_->Init();
	player_->Init();
	titleRogo_->Init();
		
	alpha_ = 0.0f;
	
	viewProjection_.translation_ = { 7.8f,3.6f,8.3f };
	viewProjection_.rotation_.y= 3.8f;

	shandle_ = TextureManager::GetInstance()->LoadTexture("Resources/Texture/screenChange.png");
	screenSprite_.reset(Sprite::Create(shandle_, Vector2(0, 0), Vector4(1, 1, 1, alpha_)));
	
	player_->SetTitleBehavior();
	player_->SetWorldPositionY(30.0f);

	ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
	
	/*/// particleD
	damageName_ = "DamageParticle";
	damageEmitter_.reset(ParticleEmitter::CreateParticle(damageName_, "Plane", ".obj", 300, false));
	uint32_t handle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/circle.png");
	damageEmitter_->SetTextureHandle(handle);
	*/
}

void TitleScene::Update() {
	
	screenSprite_->SetAlpha(alpha_);

	player_->TitleUpdate();
	field_->Update();
    skyBox_->Update();
	skydome_->Update();

	if (dynamic_cast<TitleRightPunch*>(player_->GetTitleBehavior())) {
		if (!isTitleRogoUpdate) {
			isTitleRogoUpdate = true;
		}
	}
	if (isTitleRogoUpdate) {
		titleRogo_->Update();
	}

	ParticleManager::GetInstance()->Update();

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
	
	if (!((Input::IsTriggerPad(0, XINPUT_GAMEPAD_A)))) {
        return;
    }

	isFinished_ = true;

}

/// ===================================================
/// モデル描画
/// ===================================================
void TitleScene::ModelDraw() {
    /// commandList取得
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
    Model::PreDraw(commandList);

	
	Model::PreDraw(commandList);
	//skydome_->Draw(viewProjection_);
	field_->Draw(viewProjection_);
	player_->Draw(viewProjection_);

	ParticleManager::GetInstance()->Draw(viewProjection_);
	Model::PreDraw(commandList);
	player_->EffectDraw(viewProjection_);
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void TitleScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}


/// ===================================================
   /// スプライト描画
   /// ===================================================
void TitleScene::SpriteDraw() {
	titleRogo_->Draw();

	screenSprite_->Draw();
}

void TitleScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
	titleRogo_->Debug();
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