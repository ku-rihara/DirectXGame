#include "Score/Score.h"
#include "base/TextureManager.h"

#include"Frame/Frame.h"
#include"MathFunction.h"
#include<imgui.h>

void Score::Init() {
    numSprite_.resize(5);  // スコアは最大5桁と仮定する
    score_ = 0;
    size_ = 96.0f;
    scoreEaseTMax_ = 1.0f;

    // テクスチャ読み込み
    uint32_t numHandle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/number.png");
    uint32_t ScoreFrameHandle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/Score.png");
    
    // 各桁のスプライトを初期化
    for (uint32_t i = 0; i < numSprite_.size(); i++) {
        numSprite_[i].reset(Sprite::Create(numHandle, Vector2(946 +32*float(i), 494), Vector4(1, 1, 1, 1)));
        numSprite_[i]->textureSize_.x = size_;
    }

    scoreFrameSprite_.reset(Sprite::Create(ScoreFrameHandle, Vector2(946, 444), Vector4(1, 1, 1, 1)));
}

void Score::Update() {
    // スコアの数値を各桁ごとに分解
    scoreEaseT_ += Frame::DeltaTime();
    if (scoreEaseT_ >= scoreEaseTMax_) {
        scoreEaseT_=scoreEaseTMax_;
    }
     score_ = int(Lerp(float(score_), float(currentScore_),scoreEaseT_));
     int tempScore = score_;
   
    for (uint32_t i = 0; i < numSprite_.size(); i++) {
        int digit = tempScore % 10;  // 各桁の値を取得
        tempScore /= 10;

        numSprite_[(uint32_t(numSprite_.size())-i)-1]->uvTransform_.pos.x = (digit / 10.0f);
        // UVオフセットを適用（各桁ごとに横方向にずらす）
     /*   sprite_[i]->SetUVTransform((digit * uvOffsetX_, 0, uvOffsetX_, 1));*/
    }
}

void Score::Draw() {
    scoreFrameSprite_->Draw();
    for (size_t i = 0; i < numSprite_.size(); i++) {
        numSprite_[i]->Draw();
    }
}

void Score::ScoreUp(int num) {
    scoreEaseT_ = 0.0f;
    currentScore_ += num;
}

void Score::Debug() {
    if (ImGui::TreeNode("Score")) {
        ImGui::DragInt("Score", &score_);
        scoreFrameSprite_->DebugImGui();
        ImGui::TreePop();
    }
}

//void Score::SetScore(int score) {
//    score_ = score;
//}
