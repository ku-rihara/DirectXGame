#include "ResultRankSprite.h"

void ResultRankSprite::Init(int32_t rank, const Config& cfg) {
    config_ = cfg;
    phase_  = Phase::kIdle;

    // スプライトの作成
    sprite_.reset(KetaEngine::Sprite::Create(textureFileName_));
    sprite_->SetIsDraw(true);

    // ランク数に合わせてUVスケールの計算
    float uvW = 1.0f / static_cast<float>(cfg.rankCount);

    // UVパラメータの適応
    sprite_->SetUVScale(Vector2(uvW, 1.0f));
    sprite_->SetUVPosition(Vector2(rank * uvW, 0.0f));

    // 初期位置とスケールを設定
    sprite_->transform_.pos = cfg.basePos;
    ApplyScale(Vector2::ZeroVector());

    phaseUpdate_ = [this](float dt) { UpdateIdle(dt); };

    // イージングの初期化
    scaleEasing_.Init("ResultItemScaleIn.json");
    scaleEasing_.SetAdaptValue(&currentScale_);
    scaleEasing_.SetEndValue(cfg.baseScale);
    scaleEasing_.SetOnFinishCallback([this]() {
        phase_       = Phase::kDone;
        phaseUpdate_ = [this](float dt) { UpdateDone(dt); };
    });
}

void ResultRankSprite::Start() {

    // アニメーション開始
    currentScale_ = Vector2::ZeroVector();
    waitTimer_    = 0.0f;
    phase_        = Phase::kWait;
    phaseUpdate_  = [this](float dt) { UpdateWait(dt); };
}

void ResultRankSprite::Update(float dt) {
    if (!sprite_) { 
        return; 
    }

    // 常に基本位置を維持
    sprite_->transform_.pos = config_.basePos;

    // フェーズごとの更新処理を呼び出す
    if (phaseUpdate_) { 
        phaseUpdate_(dt);
    }
}

void ResultRankSprite::UpdateIdle(float) {    
   
    // 表示しないようにスケールを0に設定
    ApplyScale(Vector2::ZeroVector());
}

void ResultRankSprite::UpdateWait(float dt) {
    ApplyScale(Vector2::ZeroVector());

    // タイマーを更新
    waitTimer_ += dt;

    // 待機が終わったら登場フェーズに移行
    if (waitTimer_ >= config_.startDelay) {
        phase_ = Phase::kScaleIn;
        scaleEasing_.Reset();
        phaseUpdate_ = [this](float dt) { UpdateScaleIn(dt); };
    }
}

void ResultRankSprite::UpdateScaleIn(float dt) {
    // イージングを更新してスケールに反映
    scaleEasing_.Update(dt);
    ApplyScale(currentScale_);
}

void ResultRankSprite::UpdateDone(float) {

    // 元々のスケールに設定
    ApplyScale(config_.baseScale);
}

void ResultRankSprite::ApplyScale(const Vector2& scale) {
    if (sprite_) {
        sprite_->transform_.scale = scale;
    }
}

bool ResultRankSprite::IsFinished() const {
    return phase_ == Phase::kDone;
}
