#include "RibbonTrailPlayer.h"

using namespace KetaEngine;
#include "3D/RibbonTrail/RibbonTrail.h"
#include "RibbonTrailData.h"
#include "Frame/Frame.h"
#include <cmath>

///============================================================
/// 初期化
///============================================================
void RibbonTrailPlayer::Init() {
    BaseEffectPlayer::Init();
    trail_          = nullptr;
    emitTimer_      = 0.0f;
    isActive_       = false;
    followPosition_ = nullptr;
    basePos_        = nullptr;
    basePosValue_   = {0.0f, 0.0f, 0.0f};
}

///============================================================
/// 更新
///============================================================
void RibbonTrailPlayer::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
    }

    if (!isActive_ || !trail_) {
        return;
    }

    // エディタ編集をリアルタイムに反映
    effectData_->RefreshParams();
    SyncDataToTrail();

    const RibbonTrailData* data = GetData();
    if (!data) { return; }

    switch (data->GetFollowMode()) {

    case TrailFollowMode::Follow:
        // 追従ポインタが設定されていればポイントを自動追加
        if (followPosition_) {
            float dt = Frame::DeltaTime() * speedRate;
            emitTimer_ += dt;
            if (emitTimer_ >= GetEmitInterval()) {
                emitTimer_ = 0.0f;
                trail_->AddPoint(*followPosition_, GetStartColor(), GetStartWidth(), GetLifetime());
            }
        }
        break;

    case TrailFollowMode::Arc:
        // 弧モードは EmitArc() で一括生成済み。
        // basePos_ がフレームごとに動く（キャラクターに追従）場合は
        // 毎フレーム弧を再生成したいケースもあるので、
        // 追従が必要なら Update 内で ApplyArcToTrail() を呼ぶ。
        // デフォルトは生成後そのまま（自然消滅）。
        break;

    case TrailFollowMode::StayInPlace:
    default:
        // 何もしない（ポイントは自然消滅）
        break;
    }
}

///============================================================
/// Play
///============================================================
void RibbonTrailPlayer::Play(const std::string& presetName, const std::string& categoryName) {
    effectData_.reset();
    effectData_ = CreateEffectData();

    if (effectData_) {
        effectData_->Init(presetName, categoryName);
        effectData_->LoadData();
        effectData_->Play();
    }

    if (!trail_) {
        trail_ = RibbonTrail::Create(static_cast<size_t>(GetMaxPoints()));
    } else {
        trail_->Clear();
    }

    SyncDataToTrail();
    emitTimer_ = 0.0f;
    isActive_  = true;

    // 弧モードなら即時生成
    const RibbonTrailData* data = GetData();
    if (data && data->GetFollowMode() == TrailFollowMode::Arc) {
        ApplyArcToTrail();
    }
}

///============================================================
/// EmitArc（外部から任意タイミングで弧を生成する）
///============================================================
void RibbonTrailPlayer::EmitArc() {
    if (!trail_) { return; }
    ApplyArcToTrail();
}

///============================================================
/// ApplyArcToTrail（内部処理）
///============================================================
void RibbonTrailPlayer::ApplyArcToTrail() {
    const RibbonTrailData* data = GetData();
    if (!data || !trail_) { return; }

    // 基準座標：SetBasePos/SetBasePosValue で指定、未指定は原点
    static const Vector3 kOrigin = {0.0f, 0.0f, 0.0f};
    const Vector3& center = basePos_ ? *basePos_ : kOrigin;

    // 度数 → ラジアン変換
    constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
    float startRad = data->GetArcStartAngleDeg() * kDegToRad;
    float endRad   = data->GetArcEndAngleDeg()   * kDegToRad;

    ArcDirection dir   = (data->GetArcDirectionInt() == 1) ? ArcDirection::Clockwise
                                                           : ArcDirection::CounterClockwise;
    ArcPlane     plane = static_cast<ArcPlane>(data->GetArcPlaneInt());

    trail_->Clear();
    trail_->SetArc(center, startRad, endRad, data->GetArcRadius(), dir, plane);

}

///============================================================
/// StopEmit
///============================================================
void RibbonTrailPlayer::StopEmit() {
    isActive_  = false;
    emitTimer_ = 0.0f;
}

///============================================================
/// StopAndClear
///============================================================
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

///============================================================
/// SyncDataToTrail
///============================================================
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
