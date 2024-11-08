#include "Score/Score.h"
#include "base/TextureManager.h"
#include<imgui.h>

void Score::Init() {
    numSprite_.resize(5);  // スコアは最大5桁と仮定する
    score_ = 0;
    size_ = 144.0f;

    // テクスチャ読み込み
    uint32_t numHandle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/number.png");
    uint32_t ScoreFrameHandle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/Score.png");

    // 各桁のスプライトを初期化
    for (size_t i = 0; i < numSprite_.size(); i++) {
        numSprite_[i].reset(Sprite::Create(numHandle, Vector2(520, 320), Vector4(1, 1, 1, 1)));
        numSprite_[i]->textureSize_.x = size_;
    }

    scoreFrameSprite_.reset(Sprite::Create(ScoreFrameHandle, Vector2(520, 320), Vector4(1, 1, 1, 1)));
}

void Score::Update() {
    // スコアの数値を各桁ごとに分解
    int tempScore = score_;
   
    for (uint32_t i = 0; i < numSprite_.size(); i++) {
        int digit = tempScore % 10;  // 各桁の値を取得
        tempScore /= 10;

        numSprite_[i]->uvTransform_.pos.x = digit * size_;
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
    score_ += num;
}

void Score::Debug() {
    if (ImGui::TreeNode("Score")) {
        scoreFrameSprite_->DebugImGui();
        ImGui::TreePop();
    }

}

//void Score::SetScore(int score) {
//    score_ = score;
//}
