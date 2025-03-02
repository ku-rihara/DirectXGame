///scene
#include"Manager/SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"

#include "base/TextureManager.h"
//class
#include"utility/Particle/ParticleManager.h"

//math
#include"Frame/Frame.h"
#include<imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {

}

void TitleScene::Init() {

	BaseScene::Init();

	//uint32_t t = TextureManager::GetInstance()->LoadTexture("Resources/Texture/boal.png");
	uint32_t circleHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/circle.png");
	/*uint32_t defaultHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/default.png");*/
	uint32_t starHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/star.png");
	uint32_t starFrameHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/circleFrame.png");


	/*smokeName_ = "EnemyDeathSmoke";
	smokeEmitter_.reset(ParticleEmitter::CreateParticle(smokeName_, "Plane", ".obj", 900));
	smokeEmitter_->SetTextureHandle(t);
	smokeEmitter_->SetBlendMode(ParticleCommon::BlendMode::None);

	fireSmokeName_ = "EnemyDeathFireSmoke";
	fireSmokeEmitter_.reset(ParticleEmitter::CreateParticle(fireSmokeName_, "Plane", ".obj", 900));
	fireSmokeEmitter_->SetTextureHandle(t2);
	fireSmokeEmitter_->SetBlendMode(ParticleCommon::BlendMode::Add);

	sparkName_ = "EnemyDeathSpark";
	sparkEmitter_.reset(ParticleEmitter::CreateParticle(sparkName_, "Plane", ".obj", 900));
	sparkEmitter_->SetTextureHandle(t2);
	sparkEmitter_->SetBlendMode(ParticleCommon::BlendMode::Add);

	miniSparkName_ = "EnemyDeathMiniSpark";
	miniSparkEmitter_.reset(ParticleEmitter::CreateParticle(miniSparkName_, "Plane", ".obj", 900));
	miniSparkEmitter_->SetTextureHandle(t2);
	miniSparkEmitter_->SetBlendMode(ParticleCommon::BlendMode::Add);*/

	/*debriName_ = "DebriName";
	debriEmitter_.reset(ParticleEmitter::CreateParticle(debriName_, "debri", ".obj", 900));
	debriEmitter_->SetTextureHandle(defaultHandle);
	debriEmitter_->SetBlendMode(ParticleCommon::BlendMode::None);*/

	starName_ = "StarEffect";
	starEmitter_.reset(ParticleEmitter::CreateParticle(starName_, "plane", ".obj", 10));
	starEmitter_->SetTextureHandle(starHandle);
	starEmitter_->SetBlendMode(ParticleCommon::BlendMode::None);

	centerStar_ = "CenterStarEffect";
	centerStarEmitter_.reset(ParticleEmitter::CreateParticle(centerStar_, "plane", ".obj", 10));
	centerStarEmitter_->SetTextureHandle(circleHandle);
	centerStarEmitter_->SetBlendMode(ParticleCommon::BlendMode::Add);

	starFrame_ = "StarFrame";
	starFrameEmitter_.reset(ParticleEmitter::CreateParticle(starFrame_, "plane", ".obj", 10));
	starFrameEmitter_->SetTextureHandle(starFrameHandle);
	starFrameEmitter_->SetBlendMode(ParticleCommon::BlendMode::None);

	ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
}

void TitleScene::Update() {

	/*debriEmitter_->Update();
	debriEmitter_->EditorUpdate();
	debriEmitter_->Emit();*/

	starEmitter_->Update();
	starEmitter_->EditorUpdate();
	starEmitter_->Emit();

	centerStarEmitter_->Update();
	centerStarEmitter_->EditorUpdate();
	centerStarEmitter_->Emit();

	starFrameEmitter_->Update();
	starFrameEmitter_->EditorUpdate();
	starFrameEmitter_->Emit();

	ParticleManager::GetInstance()->Update();

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
	/// commandList取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	Model::PreDraw(commandList);

	//debriEmitter_->DebugDraw(viewProjection_);
	//debriEmitter_->RailDraw(viewProjection_);
	ParticleManager::GetInstance()->Draw(viewProjection_);

}

   /// ===================================================
   /// パーティクル描画
   /// ===================================================
//void TitleScene::ParticleDraw() {
//	
//}

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
