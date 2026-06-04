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
    currentScale_ = Vector2::ZeroVector();
    waitTimer_    = 0.0f;
    phase_        = Phase::kWait;
    phaseUpdate_  = [this](float dt) { UpdateWait(dt); };
}

void ResultRankSprite::SetConfig(const Config& cfg) {
    config_ = cfg;
    scaleEasing_.SetEndValue(cfg.baseScale);
}

void ResultRankSprite::Update(float dt) {
    if (!sprite_) { return; }
    sprite_->transform_.pos = config_.basePos;
    if (phaseUpdate_) { phaseUpdate_(dt); }
}

void ResultRankSprite::UpdateIdle(float) {
    ApplyScale(Vector2::ZeroVector());
}

void ResultRankSprite::UpdateWait(float dt) {
    ApplyScale(Vector2::ZeroVector());
    waitTimer_ += dt;
    if (waitTimer_ >= config_.startDelay) {
        phase_ = Phase::kScaleIn;
        scaleEasing_.Reset();
        phaseUpdate_ = [this](float dt) { UpdateScaleIn(dt); };
    }
}

void ResultRankSprite::UpdateScaleIn(float dt) {
    scaleEasing_.Update(dt);
    ApplyScale(currentScale_);
}

void ResultRankSprite::UpdateDone(float) {
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
