#include "Score/Score.h"
#include "base/TextureManager.h"

#include"Frame/Frame.h"
#include"MathFunction.h"
#include<imgui.h>

void Score::Init() {
	numSprite_.resize(5);  // スコアは最大5桁と仮定する
	addNumSprite_.resize(3);
	score_ = 0;
	size_ = 96.0f;
	scoreEaseTMax_ = 1.0f;

	// テクスチャ読み込み
	uint32_t numHandle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/number.png");
	uint32_t ScoreFrameHandle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/Score.png");

	// 各桁のスプライトを初期化
	for (uint32_t i = 0; i < numSprite_.size(); i++) {
		numSprite_[i].reset(Sprite::Create(numHandle, Vector2(1010 + 37 * float(i), 480), Vector4(1, 1, 1, 1)));
		numSprite_[i]->textureSize_.x = size_;
	}

	// 各桁のスプライトを初期化
	for (uint32_t i = 0; i < addNumSprite_.size(); i++) {
		addNumSprite_[i].reset(Sprite::Create(numHandle, Vector2(1000 + 37 * float(i), 350), Vector4(1, 1, 1, 1)));
		addNumSprite_[i]->textureSize_.x = size_;
	}

	/// 枠
	scoreFrameSprite_.reset(Sprite::Create(ScoreFrameHandle, Vector2(842, 403), Vector4(1, 1, 1, 1)));
}

void Score::Update() {
	// スコアの数値を各桁ごとに分解
	scoreEaseT_ += Frame::DeltaTime();
	if (scoreEaseT_ >= scoreEaseTMax_ - 0.4f) {
		scoreEaseT_ = scoreEaseTMax_;
	}
	score_ = int(Lerp(float(score_), float(currentScore_), scoreEaseT_));

	int tempAddScore = addScore_;
	int tempScore = score_;

	for (uint32_t i = 0; i < numSprite_.size(); i++) {
		/// 合計スコア
		int digit = tempScore % 10;  // 各桁の値を取得
		tempScore /= 10;
		
		numSprite_[(uint32_t(numSprite_.size()) - i) - 1]->uvTransform_.pos.x = (digit / 10.0f);
	}


	for (uint32_t i = 0; i < addNumSprite_.size(); i++) {
		/// 加算スコア
		int upDigit = tempAddScore % 10;  // 各桁の値を取得
		tempAddScore /= 10;

		addNumSprite_[(uint32_t(addNumSprite_.size()) - i) - 1]->uvTransform_.pos.x = (upDigit / 10.0f);
	}


	scoreTime_ -= Frame::DeltaTime();
	if (scoreTime_ <= 0.0f) {
		scoreTime_ = 0.0f;
		isAddScoreDraw_ = false;
	}

}

void Score::Draw() {
	scoreFrameSprite_->Draw();
	for (size_t i = 0; i < numSprite_.size(); i++) {

		numSprite_[i]->Draw();
	}
	/// 加算するスコア
	for (size_t i = 0; i < addNumSprite_.size(); i++) {
		if (isAddScoreDraw_) {
			addNumSprite_[i]->Draw();
		}
	}
}

void Score::ScoreUp(int num) {
	isAddScoreDraw_ = true;
	scoreTime_ = scoreTimeMax_;
	scoreEaseT_ = 0.0f;
	addScore_ = num;
	currentScore_ += num;
}

void Score::Debug() {
#ifdef _DEBUG
	if (ImGui::TreeNode("Score")) {
		ImGui::DragInt("Score", &score_);
		scoreFrameSprite_->DebugImGui();
		ImGui::TreePop();
	}
#endif
}

//void Score::SetScore(int score) {
//    score_ = score;
//}
