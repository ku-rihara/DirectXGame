///scene
#include "TitleScene.h"
#include"Manager/SceneManager.h"
#include"utility/Particle/ParticleManager.h"
#include"utility/Particle/ParticleCommon.h"

#include "base/TextureManager.h"
//class

//math

#include<imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {

}

void TitleScene::Init() {

	BaseScene::Init();

	thrustName_ = "TrushParticle";
	damageEmitter_.reset(ParticleEmitter::CreateParticle(thrustName_, "cube", ".obj", 200, false));
	damageEmitter_->SetBlendMode(BlendMode::None);
	damageEmitter_->SetIsBillBord(false);
}

void TitleScene::Update() {
	damageEmitter_->Update();
	damageEmitter_->EditorUpdate();
	

	damageEmitter_->Emit();
	
	ParticleManager::GetInstance()->Update(&viewProjection_);

	Debug();
	ViewProjectionUpdate();

	if (input_->TrrigerKey(DIK_RETURN)) {

		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	
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