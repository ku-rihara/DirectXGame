#include "RibbonTrailPlayer.h"

using namespace KetaEngine;
#include "3D/RibbonTrail/RibbonTrail.h"
#include "RibbonTrailData.h"
#include "Frame/Frame.h"

///============================================================
/// 初期化
///============================================================
void RibbonTrailPlayer::Init() {
    BaseEffectPlayer::Init();
    trail_          = nullptr;
    emitTimer_      = 0.0f;
    isActive_       = false;
    followPosition_ = nullptr;
}

///============================================================
/// 更新
///============================================================
void RibbonTrailPlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
    }

    if (isActive_ && trail_) {
        // エディタ編集をリアルタイムに反映
        effectData_->RefreshParams();
        SyncDataToTrail();

        // followPosition_ が設定されていればAutoEmit
        if (followPosition_) {
            float dt = Frame::DeltaTime() * speedRate;
            emitTimer_ += dt;
            if (emitTimer_ >= GetEmitInterval()) {
                emitTimer_ = 0.0f;
                trail_->AddPoint(*followPosition_, GetStartColor(), GetStartWidth(), GetLifetime());
            }
        }
    }
}


void RibbonTrailPlayer::Play(const std::string& presetName, const std::string& categoryName) {
    // Data をリセットしてロード
    effectData_.reset();
    effectData_ = CreateEffectData();

    if (effectData_) {
        effectData_->Init(presetName, categoryName);
        effectData_->LoadData();
        effectData_->Play();
    }

    // Trail オブジェクトを生成
    if (!trail_) {
        trail_ = RibbonTrail::Create(static_cast<size_t>(GetMaxPoints()));
    } else {
        trail_->Clear();
    }

    SyncDataToTrail();
    emitTimer_ = 0.0f;
    isActive_  = true;
}


void RibbonTrailPlayer::StopAndClear() {
    isActive_ = false;
    if (trail_) {
        trail_->Clear();
    }
    emitTimer_ = 0.0f;
    if (effectData_) {
        effectData_->Reset();
    }
}

///============================================================
/// CreateEffectData
///============================================================
std::unique_ptr<BaseEffectData> RibbonTrailPlayer::CreateEffectData() {
    return std::make_unique<RibbonTrailData>();
}

///============================================================
/// GetData
///============================================================
RibbonTrailData* RibbonTrailPlayer::GetData() const {
    return dynamic_cast<RibbonTrailData*>(effectData_.get());
}

void RibbonTrailPlayer::SyncDataToTrail() {
    if (!trail_) {
        return;
    }
    trail_->SetEndColor(GetEndColor());
    trail_->SetEndWidth(GetEndWidth());
    trail_->SetTexture(GetTexturePath());
    trail_->SetUseDistortion(GetUseDistortion());
    trail_->SetDistortionStrength(GetDistortionStrength());
    trail_->SetDistortionTexture(GetDistortionTexturePath());
    trail_->SetUVScrollSpeed(GetUVScrollSpeed());
    trail_->SetDistortionUVScrollSpeed(GetDistortionUVScrollSpeed());
}

///============================================================
/// Getter
///============================================================
Vector4 RibbonTrailPlayer::GetStartColor() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetStartColor() : Vector4{1.0f, 1.0f, 1.0f, 1.0f};
}

Vector4 RibbonTrailPlayer::GetEndColor() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetEndColor() : Vector4{1.0f, 1.0f, 1.0f, 0.0f};
}

float RibbonTrailPlayer::GetStartWidth() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetStartWidth() : 0.15f;
}

float RibbonTrailPlayer::GetEndWidth() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetEndWidth() : 0.0f;
}

float RibbonTrailPlayer::GetLifetime() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetLifetime() : 0.3f;
}

int32_t RibbonTrailPlayer::GetMaxPoints() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetMaxPoints() : 48;
}

float RibbonTrailPlayer::GetEmitInterval() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetEmitInterval() : 0.005f;
}

const std::string& RibbonTrailPlayer::GetTexturePath() const {
    static const std::string empty;
    const RibbonTrailData*   data = GetData();
    return data ? data->GetTexturePath() : empty;
}

bool RibbonTrailPlayer::GetUseDistortion() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetUseDistortion() : false;
}

float RibbonTrailPlayer::GetDistortionStrength() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetDistortionStrength() : 0.1f;
}

const std::string& RibbonTrailPlayer::GetDistortionTexturePath() const {
    static const std::string empty;
    const RibbonTrailData*   data = GetData();
    return data ? data->GetDistortionTexturePath() : empty;
}

Vector2 RibbonTrailPlayer::GetUVScrollSpeed() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetUVScrollSpeed() : Vector2::ZeroVector();
}

Vector2 RibbonTrailPlayer::GetDistortionUVScrollSpeed() const {
    const RibbonTrailData* data = GetData();
    return data ? data->GetDistortionUVScrollSpeed() : Vector2::ZeroVector();
}
