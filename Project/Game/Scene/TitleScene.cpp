#include "TitleScene.h"
#include"base/TextureManager.h"
//class
#include"JoyState/JoyState.h"


TitleScene::TitleScene() {}

TitleScene::~TitleScene() {

}

void TitleScene::Init() {

	handle_ = TextureManager::GetInstance()->LoadTexture("./resources/Texture/Title.png");
	titleSprite_.reset(Sprite::Create(handle_, Vector2(0, 0), Vector4(1, 1, 1, 1)));
}

void TitleScene::Update() {
	if (Input::GetInstance()->TrrigerKey(DIK_SPACE) || joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {

		isFinished_ = true;
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

}
/// ===================================================
/// スプライト描画
/// ===================================================
void TitleScene::SpriteDraw() {
	titleSprite_->Draw();
}

void TitleScene::Debug() {
#ifdef _DEBUG

#endif
}


// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {


}