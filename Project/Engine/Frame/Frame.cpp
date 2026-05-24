// Frame.cpp
#include "Frame.h"

using namespace KetaEngine;

std::chrono::steady_clock::time_point Frame::lastTime_  = std::chrono::steady_clock::now();
float Frame::deltaTime_                                 = 0.0f;
float Frame::deltaTimeRate_                             = 0.0f;
float Frame::timeScale_                                 = 1.0f;
const float Frame::kFPS                                 = 60.0f;

void Frame::Init() {
    lastTime_  = std::chrono::steady_clock::now();
    deltaTime_ = 0.0f;
    timeScale_ = 1.0f;
}

void Frame::Update() {
    // 経過時間（秒）を計算
    auto currentTime                       = std::chrono::steady_clock::now();
    std::chrono::duration<float> frameTime = currentTime - lastTime_;

    // シーン遷移やフリーズによる極端な値を制限
    float frameTimeCount = frameTime.count();
    if (frameTimeCount > 0.1f) {
        deltaTime_ = 1.0f / kFPS;
    } else {
        deltaTime_ = frameTimeCount;
    }

    deltaTimeRate_ = deltaTime_ * timeScale_;
    lastTime_      = currentTime;
}

float Frame::DeltaTime() {
    return deltaTime_;
}

float Frame::DeltaTimeRate() {
    return deltaTimeRate_;
}

/// <summary>
/// 秒からフレームに変換
/// </summary>
int32_t Frame::TimeToFrame(float seconds) {
    return static_cast<int32_t>(seconds * kFPS);
}

/// <summary>
/// フレームから秒に変換
/// </summary>
float Frame::FrameToTime(int32_t frame) {
    return static_cast<float>(frame) / kFPS;
}

void Frame::SetTimeScale(float scale) {

    timeScale_ = scale;
}

float Frame::GetTimeScale() {
    return timeScale_;
}

void Frame::ResetDeltaTime() {
    deltaTime_ = 0.0f;
}
