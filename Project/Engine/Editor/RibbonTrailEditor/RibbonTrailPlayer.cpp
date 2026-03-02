#include "RibbonTrailPlayer.h"

using namespace KetaEngine;
#include "3D/RibbonTrail/RibbonTrail.h"
#include "RibbonTrailData.h"

///============================================================
/// 初期化
///============================================================
void RibbonTrailPlayer::Init() {
    BaseEffectPlayer::Init();
    trail_     = nullptr;
    emitTimer_ = 0.0f;
}

///============================================================
/// 更新
///============================================================
void RibbonTrailPlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
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
}


void RibbonTrailPlayer::Emit(const Vector3& position, float deltaTime) {
    if (!trail_) {
        return;
    }

    emitTimer_ += deltaTime;
    if (emitTimer_ >= GetEmitInterval()) {
        emitTimer_ = 0.0f;
        trail_->AddPoint(
            position,
            GetStartColor(),
            GetStartWidth(),
            GetLifetime());
    }
}


void RibbonTrailPlayer::StopAndClear() {
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
